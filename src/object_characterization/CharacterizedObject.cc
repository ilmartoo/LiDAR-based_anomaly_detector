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
#include "app/CLI.hh"
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

    std::vector<std::vector<size_t>> clusters = DBScan::clusters(points);  // Clusterización

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

    clusters = DBScan::normals(opoints);  // Detección de las caras

    // Salida si no se han detectado caras del objeto
    if (clusters.size() == 0) {
        return {false, {}};
    }

    // Guardado de puntos del cluster
    CharacterizedObject charObject;
    charObject.setPoints(opoints);

    /// DEBUG PRINT CARAS
    DEBUG_CODE({
        std::ofstream of("tmp/caras_object.csv");
        of << LidarPoint::LivoxCSVHeader() << "\n";
        uint32_t partial = 255 / (clusters.size() + 1);
        for (size_t i = 0; i < opoints.size(); ++i) {
            of << LidarPoint({0, 0}, partial * (opoints[i].getClusterID() < 0 ? 0 : opoints[i].getClusterID() + 1), opoints[i]).LivoxCSV() << "\n";
        }
        of.close();
    });
    ///

    /// Caras
    std::vector<std::vector<Point *>> facepoints(clusters.size(), std::vector<Point *>());  // Vector de vectores de referencias puntos de cada cara
    for (size_t i = 0; i < clusters.size(); ++i) {
        facepoints[i].reserve(clusters[i].size());
        for (size_t j = 0; j < clusters[i].size(); ++j) {
            facepoints[i].push_back(&charObject.getPoints()[clusters[i][j]]);
        }
    }
    std::vector<Face> faces(clusters.size(), Face());  // Vector de caras

    if (chrono) {
        end_face_detection = std::chrono::high_resolution_clock::now();
    }

    //////////////////////////////////////
    // Cálculo de la mejor bounding box //
    //////////////////////////////////////

    std::pair<BBox, Vector> bbmin = Geometry::minimumBBoxRotTrans(charObject.getPoints());  // Bounding box mínima

    DEBUG_STDOUT("Best bounding box rotation angles: " << bbmin.second);

    std::vector<std::pair<BBox, Vector>> fbbmin = Geometry::minimumBBoxes(facepoints);

    for (size_t i = 0; i < fbbmin.size(); ++i) {
        faces[i] = Face(clusters[i], Geometry::computeNormal(facepoints[i]), fbbmin[i].first, fbbmin[i].second);

        DEBUG_STDOUT("Face " << i << " best bounding box rotation angles: " << faces[i].getMinBBoxRotAngles());
    }

    // Guardado de caras y bounding boxes
    charObject.setBBox(bbmin.first);
    charObject.setFaces(faces);

    // Cronometro
    if (chrono) {
        end = std::chrono::high_resolution_clock::now();

        double cl_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_agrupation - start).count()) / 1.e9;
        double fd_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_face_detection - end_agrupation).count()) / 1.e9;
        double bb_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - end_face_detection).count()) / 1.e9;

        CLI_STDOUT("Object characterization lasted " << std::setprecision(6) << (cl_duration + fd_duration + bb_duration) << "s (clustering: " << cl_duration << "s, face detection:  " << fd_duration << "s, bounding box selection: " << bb_duration << std::setprecision(2) << "s)");
    }

    DEBUG_STDOUT("Characterized object with " << facepoints.size() << " faces");

    return {true, charObject};
}

bool CharacterizedObject::write(const std::string &filename) {
    std::ofstream outfile(filename);
    if (outfile.is_open()) {
        outfile.write((char *)&bbox, sizeof(BBox));  // Bounding box
        size_t len = points.size();
        // Puntos
        outfile.write((char *)&len, sizeof(size_t));  // Numero de puntos
        for (auto &p : points) {
            outfile.write((char *)&p, sizeof(Point));  // Índice del punto de la cara
        }
        // Caras
        len = faces.size();
        outfile.write((char *)&len, sizeof(size_t));  // Numero de caras
        for (auto &f : faces) {
            outfile.write((char *)&f.getNormal(), sizeof(Vector));            // Normal de la cara
            outfile.write((char *)&f.getMinBBox(), sizeof(BBox));             // Bounding box de la cara
            outfile.write((char *)&f.getMinBBoxRotAngles(), sizeof(Vector));  // Ángulo de rotación de la cara
            len = f.getIndices().size();
            outfile.write((char *)&len, sizeof(size_t));  // Numero de puntos de la cara
            // Indices de los puntos
            for (auto &idx : f.getIndices()) {
                outfile.write((char *)&idx, sizeof(size_t));  // Índice del punto de la cara
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
    Timestamp tmstp(0, 0);

    unsigned ncolors = faces.size();

    uint32_t colors[ncolors + 1];
    size_t color_step = 255 / ncolors;

    size_t half = ncolors / 2;

    // Colores para cada cara
    colors[0] = 0;
    if (ncolors > 0) {
        for (size_t i = 1, j = half + 1, k = 1; i < ncolors + 1; ++i) {
            colors[i] = color_step * (i & 1 ? j++ : k++);
        }
    }

    // Impresión a archivo
    outfile << LidarPoint::LivoxCSVHeader() << "\n";
    unsigned color;
    for (auto &p : points) {
        color = (p.getClusterID() < 0 ? 0 : p.getClusterID() + 1);
        outfile << LidarPoint(tmstp, colors[color], p).LivoxCSV() << "\n";
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
        // Puntos
        infile.read((char *)&npoints, sizeof(size_t));  // Numero de puntos
        std::vector<Point> points(npoints, Point());
        for (size_t i = 0; i < npoints; ++i) {
            infile.read((char *)&points[i], sizeof(Point));  // Punto de la cara
        }
        // Caras
        infile.read((char *)&nfaces, sizeof(size_t));  // Numero de caras
        std::vector<Face> faces(nfaces, Face());
        Vector normal;
        BBox fbbox;
        Vector frotdeg;
        for (size_t i = 0; i < nfaces; ++i) {
            infile.read((char *)&normal, sizeof(Vector));   // Normal de la cara
            infile.read((char *)&fbbox, sizeof(BBox));      // Bounding box de la cara
            infile.read((char *)&frotdeg, sizeof(Vector));  // Ángulo de rotación de la cara
            // Referencias a los puntos de la cara
            infile.read((char *)&npoints, sizeof(size_t));  // Numero de puntos de la cara
            std::vector<size_t> indices(npoints, 0);
            size_t index;
            for (size_t j = 0; j < npoints; ++j) {
                infile.read((char *)&index, sizeof(size_t));  // Índice del punto de la cara
                indices[j] = index;
            }

            faces[i] = Face(indices, normal, fbbox, frotdeg);
        }

        infile.close();
        return {!infile.fail(), CharacterizedObject(points, bbox, faces)};

    } else {
        return {false, {}};
    }
}