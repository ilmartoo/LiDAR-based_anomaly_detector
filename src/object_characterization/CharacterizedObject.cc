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
#include "models/Octree.hh"
#include "models/Point.hh"
#include "app/CLICommand.hh"

#include "logging/debug.hh"

CharacterizedObject::CharacterizedObject(const std::vector<Point> &points, bool chrono) {
    std::chrono::system_clock::time_point start, end_agrupation, end;

    if (chrono) {
        start = std::chrono::high_resolution_clock::now();
    }

    //////////////////////////////////////////////
    // Eliminación de subagrupaciones de puntos //
    //////////////////////////////////////////////

    std::vector<Point> &centroids = *new std::vector<Point>();
    std::vector<std::vector<Point>> &groups = *new std::vector<std::vector<Point>>();
    std::list<Point> &unasigned = *new std::list<Point>();

    // Inicialización de primer centroide y agrupación de puntos
    centroids.push_back(points[0]);
    groups.push_back({1, centroids[0]});
    for (size_t i = 1; i < points.size(); ++i) {
        if (centroids[0].distance3D(points[i]) < proximityThreshold) {
            groups[0].push_back(points[i]);
            size_t new_gsize = groups[0].size();
            centroids[0] = ((centroids[0] * (new_gsize - 1)) + points[i]) / new_gsize;
        } else {
            unasigned.push_back(points[i]);
        }
    }

    // Inseción de los puntos restantes en sus respectivas agrupaciones
    size_t last_size;
    do {
        last_size = unasigned.size();

        auto itr = unasigned.begin();
        while (itr != unasigned.end()) {
            bool erased = false;

            // Comprobamos si el punto pertenece a algún grupo actual
            for (size_t i = 0; i < centroids.size(); ++i) {
                if (centroids[i].distance3D(*itr) < proximityThreshold) {
                    groups[i].push_back(*itr);
                    size_t new_gsize = groups[i].size();
                    centroids[i] = ((centroids[i] * (new_gsize - 1)) + *itr) / new_gsize;

                    itr = unasigned.erase(itr);
                    erased = true;
                    break;
                }
            }
            // Aumentamos iterador si se ha eliminado
            if (!erased) {
                ++itr;
            }
        }

        // Creamos nueva agrupación si no se ha modificado la lista de puntos no asignados
        if (last_size == unasigned.size()) {
            centroids.push_back(unasigned.front());
            groups.push_back({1, unasigned.front()});
            unasigned.pop_front();
        }

    } while (unasigned.size() > 0);

    // Escogemos el grupo de puntos con mayor número de puntos
    int bestGroup = 0;
    for (size_t i = 1; i < centroids.size(); ++i) {
        if (groups[bestGroup].size() < groups[i].size()) {
            bestGroup = i;
        }
    }

    if (chrono) {
        end_agrupation = std::chrono::high_resolution_clock::now();
    }

    ///////////////////////////////////////////////
    // Cálculo de normales y separación de caras //
    ///////////////////////////////////////////////

    std::vector<Point> &opoints = *new std::vector<Point>(groups[bestGroup]);
    Octree map(opoints);
    std::vector<Vector> &normals = centroids;
    groups = {};
    normals = {};
    unasigned = {};

    // Inicialización de la primera normal y cara de puntos
    for (size_t i = 0; i < opoints.size(); ++i) {
        std::vector<Point *> neighbours = map.searchNeighbors(opoints[i], proximityThreshold * 3, Kernel_t::cube);
        neighbours.push_back(&opoints[i]);

        // Necesitamos 3 puntos mínimo para calcular la normal
        if (neighbours.size() > 2) {
            Vector normal = PlaneUtils::computeNormal(neighbours);
            if (normal[0] < 0) {
                normal = normal * -1.0;
            }

            if (normals.empty()) {
                normals.push_back(normal);
                groups.push_back({1, opoints[i]});

            } else if (normals[0].distance3D(normal) < normalThreshold) {
                groups[0].push_back(opoints[i]);
                size_t new_gsize = groups[0].size();
                normals[0] = ((normals[0] * (new_gsize - 1)) + normal) / new_gsize;
            } else {
                unasigned.push_back(opoints[i]);
            }
        }
    }

    // Calculo de normales restantes y agrupación por caras
    while (unasigned.size() > 0) {
        last_size = unasigned.size();

        auto itr = unasigned.begin();
        while (itr != unasigned.end()) {
            bool erased = false;

            std::vector<Point *> neighbours = map.searchNeighbors(*itr, proximityThreshold * 2, Kernel_t::sphere);
            neighbours.push_back(&*itr);

            Vector normal = PlaneUtils::computeNormal(neighbours);
            if (normal[0] < 0) {
                normal = normal * -1.0;
            }

            // Comprobamos si el punto pertenece a alguna cara actual
            for (size_t i = 0; i < normals.size(); ++i) {
                if (normals[i].distance3D(normal) < normalThreshold) {
                    groups[i].push_back(*itr);
                    size_t new_gsize = groups[i].size();
                    normals[i] = ((normals[0] * (new_gsize - 1)) + normal) / new_gsize;

                    itr = unasigned.erase(itr);
                    erased = true;
                    break;
                }
            }
            // Aumentamos iterador si se ha eliminado
            if (!erased) {
                ++itr;
            }
        }

        // Creamos nueva cara si no se ha modificado la lista de puntos no asignados
        if (last_size == unasigned.size()) {
            normals.push_back(unasigned.front());
            groups.push_back({1, unasigned.front()});
            unasigned.pop_front();
        }
    }

    ////////////////////////////////////////////////
    // Inicialización de las variables del objeto //
    ////////////////////////////////////////////////

    for (size_t i = 0; i < normals.size(); ++i) {
        if (groups[i].size() > 2) {
            faces.insert({normals[i], groups[i]});
        }
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

        double ag_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_agrupation - start).count()) / 1.e9;
        double nc_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - end_agrupation).count()) / 1.e9;

        CLI_STDOUT("Point agrupation lasted " << std::setprecision(6) << ag_duration << std::setprecision(2) << " s");
        CLI_STDOUT("Normal calculation lasted " << std::setprecision(6) << nc_duration << std::setprecision(2) << " s");
    }

    // Liberación de memoria
    delete &groups;
    delete &unasigned;
    delete &centroids;
    delete &opoints;
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