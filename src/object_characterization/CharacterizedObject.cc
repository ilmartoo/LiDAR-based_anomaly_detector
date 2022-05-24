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
#include <omp.h>

#include "armadillo"

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
    std::chrono::system_clock::time_point start, end_agrupation, end_face_detection, end;

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

    std::vector<std::vector<size_t>> clusters = DBScan::clusters(clusterPointProximity, minClusterPoints, points);

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

    //////////////////////////
    // Cálculo de las caras //
    //////////////////////////

    std::vector<Point> opoints;
    for (auto i : clusters[bestGroup]) {
        points[i].setClusterID(cUnclassified);
        opoints.push_back(points[i]);
    }

    clusters = DBScan::normals(facePointProximity, minFacePoints, opoints, normalVariance);

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

    /// Caras
    std::vector<Point *> references;
    std::vector<std::vector<Point>> faces;
    for (size_t i = 0; i < clusters.size(); ++i) {
        faces.push_back({});
        for (auto &j : clusters[i]) {
            faces[i].push_back(opoints[j]);
            references.push_back(&opoints[i]);
        }
    }

    if (chrono) {
        end_face_detection = std::chrono::high_resolution_clock::now();
    }

    //////////////////////////////////////
    // Cálculo de la mejor bounding box //
    //////////////////////////////////////

    // Rotaciones en las tres dimensiones
    arma::mat rotmin(3, 3, arma::fill::eye);  // Matriz identidad
    BBox bbmin(references, rotmin);           // Bbox sin rotacion
    std::vector<arma::mat> rotminPart(NORMAL_CALCULATION_THREADS, rotmin);
    std::vector<BBox> bbminPart(NORMAL_CALCULATION_THREADS, bbmin);
#pragma omp parallel for collapse(3) num_threads(NORMAL_CALCULATION_THREADS) schedule(guided)
    for (int a = 0; a < 360; ++a) {
        for (int b = 0; b < 360; ++b) {
            for (int c = 0; c < 360; ++c) {
                if (a == 0 && b == 0 && c == 0) {
                    continue;
                }
                double rad[3] = {a * M_PI / 180, b * M_PI / 180, c * M_PI / 180};  // Ángulo a radianes
                arma::mat rot = {{cos(rad[1]) * cos(rad[2]),
                                  sin(rad[0]) * sin(rad[1]) * cos(rad[2]) - cos(rad[0]) * sin(rad[1]),
                                  cos(rad[0]) * sin(rad[1]) * cos(rad[2]) + sin(rad[0]) * sin(rad[1])},
                                 {cos(rad[1]) * sin(rad[2]),
                                  sin(rad[0]) * sin(rad[1]) * sin(rad[2]) + cos(rad[0]) * cos(rad[1]),
                                  cos(rad[0]) * sin(rad[1]) * sin(rad[2]) - sin(rad[0]) * cos(rad[1])},
                                 {-sin(rad[1]),
                                  sin(rad[0]) * cos(rad[1]),
                                  cos(rad[0]) * cos(rad[1])}};
                BBox bb(references, rot);
                if (bb < bbminPart[omp_get_thread_num()]) {
                    bbminPart[omp_get_thread_num()] = bb;
                    rotminPart[omp_get_thread_num()] = rot;
                }
            }
        }
    }
    // Comparación de mejores rotaciones por hilo
    bbmin = bbminPart[0];
    rotmin = rotminPart[0];
    for (int i = 1; i < NORMAL_CALCULATION_THREADS; ++i) {
        if (bbminPart[i] < bbmin) {
            bbmin = bbminPart[i];
            rotmin = rotminPart[i];
        }
    }

    // Cronometro
    if (chrono) {
        end = std::chrono::high_resolution_clock::now();

        double cl_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_agrupation - start).count()) / 1.e9;
        double fd_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_face_detection - end_agrupation).count()) / 1.e9;
        double bb_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - end_face_detection).count()) / 1.e9;

        CLI_STDOUT("Object characterization lasted " << std::setprecision(6) << (cl_duration + fd_duration + bb_duration) << "s (clustering: " << cl_duration << "s, face detection:  " << fd_duration << "s, bounding box selection: " << bb_duration << std::setprecision(2) << "s)");
    }

    DEBUG_STDOUT("Characterized object with " << faces.size() << " faces");

    return {true, {bbmin, faces}};
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
        BBox bbox;
        infile.read((char *)&bbox, sizeof(BBox));  // Bounding box

        size_t nfaces, npoints;
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