/**
 * @file CharacterizedObject.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 13/05/2022
 *
 * @brief Implementación del objeto CharacterizedObject
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
#include "models/Geometry.hh"
#include "object_characterization/DBScan.hh"
#include "models/Octree.hh"
#include "models/Point.hh"
#include "models/LidarPoint.hh"
#include "models/Timestamp.hh"
#include "app/CLICommand.hh"
#include "app/config.h"

#include "logging/debug.hh"

std::pair<bool, CharacterizedObject> CharacterizedObject::parse(std::vector<Point> &points, bool chrono) {
    // Salida si no existen puntos en el objeto
    if (points.size() == 0) {
        return {false, {}};
    }

    std::chrono::system_clock::time_point start, end_agrupation, end_face_detection, end;

    /// DEBUG PRINT OBJECT
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

    std::vector<std::vector<size_t>> clusters = DBScan::clusters(CLUSTER_POINT_PROXIMITY, MIN_CLUSTER_POINTS, points);  // Clusterización

    // Salida si no se han detectado clústeres de puntos
    if (clusters.size() == 0) {
        return {false, {}};
    }

    /// DEBUG PRINT CLUSTERS
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

    std::vector<Point> opoints = {clusters[bestGroup].size(), Point()};
    for (size_t i = 0; i < clusters[bestGroup].size(); ++i) {
        opoints[i] = points[clusters[bestGroup][i]];
        opoints[i].setClusterID(cUnclassified);
    }

    clusters = DBScan::normals(FACE_POINT_PROXIMITY, MIN_FACE_POINTS, opoints, MAX_NORMAL_VECT_ANGLE_OC, MAX_MEAN_VECT_ANGLE);  // Detección de las caras

    // Salida si no se han detectado caras del objeto
    if (clusters.size() == 0) {
        return {false, {}};
    }

    /// DEBUG PRINT CARAS
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
    std::vector<Point *> references;             // Vector de referencias a todos los puntos del objeto
    std::vector<std::vector<Point>> facepoints;  // Vector de vectores de puntos de cada cara
    for (size_t i = 0, t = 0; i < clusters.size(); ++i) {
        facepoints.push_back({clusters[i].size(), Point()});        // Añadimos nuevo array de los puntos de una cara
        references.resize(references.size() + clusters[i].size());  // Ampliamos tamaño para añadir el nuevo array
        for (size_t j = 0; j < clusters[i].size(); ++j, ++t) {
            facepoints[i][j] = opoints[clusters[i][j]];
            references[t] = &facepoints[i][j];
        }
    }
    std::vector<Face> faces(facepoints.size(), Face());  // Vector de caras

    if (chrono) {
        end_face_detection = std::chrono::high_resolution_clock::now();
    }

    //////////////////////////////////////
    // Cálculo de la mejor bounding box //
    //////////////////////////////////////

    std::pair<BBox, Vector> bbmin = Geometry::minimumBBoxRotTrans(references);  // Bounding box mínima

    DEBUG_STDOUT("Best bounding box rotation angles: " << bbmin.second);

    std::vector<std::pair<BBox, Vector>> fbbmin = Geometry::minimumBBox(facepoints);

    for (size_t i = 0; i < fbbmin.size(); ++i) {
        faces[i] = Face(facepoints[i], Geometry::computeNormal(facepoints[i]), fbbmin[i].first, fbbmin[i].second);

        DEBUG_STDOUT("Face " << i << " best bounding box rotation angles: " << faces[i].getMinBBoxRotAngles());
    }

    // Cronometro
    if (chrono) {
        end = std::chrono::high_resolution_clock::now();

        double cl_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_agrupation - start).count()) / 1.e9;
        double fd_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_face_detection - end_agrupation).count()) / 1.e9;
        double bb_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - end_face_detection).count()) / 1.e9;

        CLI_STDOUT("Object characterization lasted " << std::setprecision(6) << (cl_duration + fd_duration + bb_duration) << "s (clustering: " << cl_duration << "s, face detection:  " << fd_duration << "s, bounding box selection: " << bb_duration << std::setprecision(2) << "s)");
    }

    DEBUG_STDOUT("Characterized object with " << facepoints.size() << " faces");

    return {true, {bbmin.first, faces}};
}

bool CharacterizedObject::write(const std::string &filename) {
    std::ofstream outfile(filename);
    if (outfile.is_open()) {
        outfile.write((char *)&bbox, sizeof(BBox));  // Bounding box
        size_t len = faces.size();
        outfile.write((char *)&len, sizeof(size_t));  // Numero de caras

        for (auto &f : faces) {
            outfile.write((char *)&f.getNormal(), sizeof(Vector));            // Normal de la cara
            outfile.write((char *)&f.getMinBBox(), sizeof(BBox));             // Bounding box de la cara
            outfile.write((char *)&f.getMinBBoxRotAngles(), sizeof(Vector));  // Ángulo de rotación de la cara
            len = f.getPoints().size();
            outfile.write((char *)&len, sizeof(size_t));  // Numero de puntos de la cara
            for (auto &p : f.getPoints()) {
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
    Timestamp tmst(0, 0);

    uint32_t colors[faces.size()];
    size_t color_step = 255 / faces.size();

    size_t half = faces.size() / 2;

    // Colores para cada cara
    if (faces.size() > 0) {
        for (size_t i = 0; i < half; ++i) {
            colors[i] = color_step * i;
            ++i;
            colors[i] = color_step * (half + i);
        }
        size_t last = faces.size() - 1;
        if (!(last & 1)) {
            colors[last] = color_step * last;
        }
    }

    // Impresión a archivo
    outfile << LidarPoint::LivoxCSVHeader() << "\n";
    for (size_t i = 0; i < faces.size(); ++i) {
        for (auto &p : faces[i].getPoints()) {
            outfile << LidarPoint(tmst, colors[i], p).LivoxCSV() << "\n";
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

        std::vector<Face> faces(nfaces, Face());
        Vector normal;
        BBox fbbox;
        Vector frotdeg;
        for (unsigned i = 0; i < nfaces; ++i) {
            infile.read((char *)&normal, sizeof(Vector));   // Normal de la cara
            infile.read((char *)&fbbox, sizeof(BBox));      // Bounding box de la cara
            infile.read((char *)&frotdeg, sizeof(Vector));  // Ángulo de rotación de la cara

            infile.read((char *)&npoints, sizeof(size_t));  // Numero de puntos de la cara
            std::vector<Point> points(npoints, Point());
            for (unsigned j = 0; j < npoints; ++j) {
                infile.read((char *)&points[j], sizeof(Point));  // Punto de la cara
            }

            faces[i] = Face(points, normal, fbbox, frotdeg);
        }

        infile.close();
        return std::move(std::pair<bool, CharacterizedObject>(!infile.fail(), CharacterizedObject(bbox, faces)));

    } else {
        return {false, {}};
    }
}