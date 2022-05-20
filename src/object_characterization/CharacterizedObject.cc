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
#include "models/LidarPoint.hh"
#include "models/Timestamp.hh"
#include "app/CLICommand.hh"

#include "logging/debug.hh"

std::pair<bool, CharacterizedObject> CharacterizedObject::parse(std::vector<Point> &points, bool chrono) {
    std::chrono::system_clock::time_point start, end_agrupation, end;

    ///
    DEBUG_CODE({
        std::ofstream of("tmp/raw_object.csv");
        of << LidarPoint::LivoxCSVHeader() << "\n";
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

    // Salida si no se han detectado clústeres de puntos
    if (clusters.size() == 0) {
        return {false, {}};
    }

    ///
    DEBUG_CODE({
        std::ofstream of("tmp/clusters_object.csv");
        of << LidarPoint::LivoxCSVHeader() << "\n";
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

    // Salida si no se han detectado caras del objeto
    if (clusters.size() == 0) {
        return {false, {}};
    }

    ///
    DEBUG_CODE({
        std::ofstream of("tmp/caras_object.csv");
        of << LidarPoint::LivoxCSVHeader() << "\n";
        uint32_t partial;
        for (size_t j = 0; j < clusters.size(); ++j) {
            partial = 255 / clusters.size() * j;
            for (auto &i : clusters[j]) {
                of << LidarPoint({0, 0}, partial, opoints[i]).LivoxCSV() << "\n";
            }
        }
        of.close();
    });
    ///

    /// Bounding box
    Octree opointsOct(opoints);
    BBox boundingBox = {opointsOct.getCenter(), opointsOct.getMax(), opointsOct.getMin()};
    /// Caras
    std::vector<std::vector<Point>> faces;
    for (size_t i = 0; i < clusters.size(); ++i) {
        faces.push_back({});
        for (auto &j : clusters[i]) {
            faces[i].push_back(opoints[j]);
        }
    }
    /// Objecto
    CharacterizedObject object(boundingBox, faces);

    // Cronometro
    if (chrono) {
        end = std::chrono::high_resolution_clock::now();

        double cl_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_agrupation - start).count()) / 1.e9;
        double fd_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - end_agrupation).count()) / 1.e9;

        CLI_STDOUT("Object characterization lasted " << std::setprecision(6) << (cl_duration + fd_duration) << "s (clustering: " << cl_duration << " s, face detection:  " <<  fd_duration << std::setprecision(2) << " s)");
    }

    DEBUG_CODE({
        std::stringstream outstr;
        outstr << "Characterized object with " << faces.size() << " faces:";
        for (size_t i = 0; i < faces.size(); ++i) {
            outstr << "\n[" << faces[i].size() << " points, normal vector: (" << object.normals[i].string() << ")]";
        }
        DEBUG_STDOUT(outstr.str());
    })

    return {true, std::move(object)};
}

bool CharacterizedObject::write(const std::string &filename) {
    std::ofstream outfile(filename);
    if (outfile.is_open()) {
        outfile.write((char *)&bbox, sizeof(BBox));  // Bounding box
        size_t len = faces.size();
        outfile.write((char *)&len, sizeof(size_t));  // Numero de caras

        for (auto &f : faces) {
            len = f.size();
            outfile.write((char *)&len, sizeof(size_t));  // Numero de puntos de la cara
            for (auto &p : f) {
                outfile.write((char *)&p, sizeof(Point));  // Punto de la cara
            }
        }

        outfile.close();
        return !outfile.fail();

    } else {
        return false;
    }
}
bool CharacterizedObject::writeLivoxCSV(const std::string &filename) {
    std::ofstream outfile(filename);
    uint32_t partial;
    Timestamp tmst(0, 0);

    outfile << LidarPoint::LivoxCSVHeader() << "\n";
    for (size_t i = 0; i < faces.size(); ++i) {
        partial = 255 / faces.size() * i;
        for (auto &p : faces[i]) {
            outfile << LidarPoint(tmst, partial, p).LivoxCSV() << "\n";
        }
    }
    outfile.close();

    return !outfile.fail();
}

std::pair<bool, CharacterizedObject> CharacterizedObject::load(const std::string &filename) {
    std::ifstream infile(filename);
    if (infile.is_open()) {
        size_t nfaces, npoints;
        BBox bbox;
        infile.read((char *)&bbox, sizeof(BBox));      // Bounding box
        infile.read((char *)&nfaces, sizeof(size_t));  // Numero de caras

        std::vector<std::vector<Point>> faces;
        Vector normal;
        Point point;
        for (unsigned i = 0; i < nfaces; ++i) {
            faces.push_back({});

            infile.read((char *)&npoints, sizeof(size_t));  // Numero de puntos de la cara
            for (unsigned j = 0; j < npoints; ++j) {
                infile.read((char *)&point, sizeof(Point));  // Punto de la cara
                faces[i].push_back(point);
            }
        }

        infile.close();
        return std::move(std::pair<bool, CharacterizedObject>(!infile.fail(), CharacterizedObject(bbox, faces)));

    } else {
        return {false, {}};
    }
}