/**
 * @file CharacterizedObject.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 13/05/2022
 *
 * Implementación del objeto CharacterizedObject
 *
 */

#include <utility>
#include <vector>
#include <list>
#include <fstream>
#include <chrono>
#include <iomanip>

#include "object_characterization/CharacterizedObject.hh"
#include "object_characterization/PlaneUtils.hh"
#include "object_characterization/DBScan.hh"
#include "models/Octree.hh"
#include "models/Point.hh"
#include "app/CLICommand.hh"

#include "logging/debug.hh"

CharacterizedObject::CharacterizedObject(std::vector<Point> &points, bool chrono) {
    std::chrono::system_clock::time_point start, end_agrupation, end;

    ///
    DEBUG_CODE({
        std::ofstream of("tmp/raw_object.csv");
        of << LidarPoint::LivoxCSVHeader();
        for (auto &p : points)
            of << LidarPoint({0, 0}, 100, p).LivoxCSV() << "\n";
        of.close();
    });
    ///

    if (chrono) {
        start = std::chrono::high_resolution_clock::now();
    }

    //////////////////////////////
    // Clusterización de puntos //
    //////////////////////////////

    std::vector<std::vector<size_t>> clusters = DBScan::clusters(proximityThreshold, 10, points);

    ///
    DEBUG_CODE({
        std::ofstream of("tmp/clusters_object.csv");
        of << LidarPoint::LivoxCSVHeader();
        uint32_t partial;
        for (size_t j = 0; j < clusters.size(); ++j) {
            partial = 255 / clusters.size() * j;
            for (auto &i : clusters[j]) {
                of << LidarPoint({0, 0}, partial, points[i]).LivoxCSV() << "\n";
            }
        }
        of.close();
    });
    ///

    // Escogemos el grupo de puntos con mayor número de puntos
    size_t bestGroup = 0;
    for (size_t i = 1; i < clusters.size(); ++i) {
        if (clusters[bestGroup].size() < clusters[i].size()) {
            bestGroup = i;
        }
    }

    if (chrono) {
        end_agrupation = std::chrono::high_resolution_clock::now();

        double ag_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_agrupation - start).count()) / 1.e9;

        CLI_STDOUT("Point agrupation lasted " << std::setprecision(6) << ag_duration << std::setprecision(2) << " s");

        end_agrupation = std::chrono::high_resolution_clock::now(); // Recalculamos despues de imprimir
    }

    ///////////////////////////////////////////////
    // Cálculo de normales y separación de caras //
    ///////////////////////////////////////////////

    std::vector<Point> opoints;
    for (auto i : clusters[bestGroup]) {
        points[i].setClusterID(cUnclassified);
        opoints.push_back(points[i]);
    }

    clusters = DBScan::normals(proximityThreshold * 2, 4, opoints, normalThreshold);

    DEBUG_CODE({
        std::ofstream of("tmp/caras_object.csv");
        of << LidarPoint::LivoxCSVHeader();
        uint32_t partial;
        for (size_t j = 0; j < clusters.size(); ++j) {
            partial = 255 / clusters.size() * j;
            for (auto &i : clusters[j]) {
                of << LidarPoint({0, 0}, partial, opoints[i]).LivoxCSV() << "\n";
            }
        }
        of.close();
    });

    for (size_t i = 0; i < clusters.size(); ++i) {
        std::vector<Point> fpoints;
        for (auto &j : clusters[i]) {
            fpoints.push_back(opoints[j]);
        }
        Vector normal = PlaneUtils::computeNormal(fpoints);
        faces.insert({normal[0] > 0 ? normal : normal * -1, fpoints});
    }

    Octree ot(opoints);
    bbox = {ot.getCenter(), ot.getMax(), ot.getMin()};

    DEBUG_CODE({
        DEBUG_STDOUT("Characterized object with " << faces.size() << " faces");
        for (auto &f : faces) {
            DEBUG_STDOUT("Normal vector: (" + f.first.string() + ") - " + std::to_string(f.second.size()));
        }
    })

    // Cronometro
    if (chrono) {
        end = std::chrono::high_resolution_clock::now();

        double nc_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - end_agrupation).count()) / 1.e9;

        CLI_STDOUT("Normal calculation lasted " << std::setprecision(6) << nc_duration << std::setprecision(2) << " s");
    }
}

bool CharacterizedObject::write(const std::string &filename) {
    std::ofstream outfile(filename);
    if (outfile.is_open()) {
        outfile.write((char *)&bbox, sizeof(BBox));  // Bounding box
        size_t len = faces.size();
        outfile.write((char *)&len, sizeof(size_t));  // Numero de caras

        for (auto &f : faces) {
            outfile.write((char *)&f.first, sizeof(Vector));  // Vector normal de la cara
            len = f.second.size();
            outfile.write((char *)&len, sizeof(size_t));  // Numero de puntos de la cara

            for (auto &p : f.second) {
                outfile.write((char *)&p, sizeof(Point));  // Punto de la cara
            }
        }

        outfile.close();
        return !outfile.fail();

    } else {
        return false;
    }
}

std::pair<bool, CharacterizedObject> CharacterizedObject::load(const std::string &filename) {
    std::ifstream infile(filename);
    if (infile.is_open()) {
        size_t nfaces, npoints;
        BBox bbox;
        infile.read((char *)&bbox, sizeof(BBox));      // Bounding box
        infile.read((char *)&nfaces, sizeof(size_t));  // Numero de caras

        std::map<Vector, std::vector<Point>> faces;
        Vector normal;
        Point point;
        for (unsigned i = 0; i < nfaces; ++i) {
            infile.read((char *)&normal, sizeof(Vector));  // Vector normal de la cara
            std::vector<Point> &vref = faces.insert({normal, {}}).first->second;

            infile.read((char *)&npoints, sizeof(size_t));  // Numero de puntos de la cara
            for (unsigned j = 0; j < npoints; ++j) {
                infile.read((char *)&point, sizeof(Point));  // Punto de la cara
                vref.push_back(point);
            }
        }

        infile.close();
        return {!infile.fail(), CharacterizedObject(bbox, faces)};

    } else {
        return {false, {}};
    }
}