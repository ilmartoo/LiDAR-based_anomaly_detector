/**
 * @file DBScan.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 18/05/2022
 *
 * Implementación del algoritmo DBScan
 *
 */

#include <utility>
#include <vector>

#include "models/Point.hh"
#include "models/Octree.hh"
#include "models/Kernel.hh"
#include "object_characterization/DBScan.hh"
#include "object_characterization/PlaneUtils.hh"

std::vector<std::vector<size_t>> DBScan::clusters(double maxDistance, unsigned int minPoints, std::vector<Point> &points) {
    int clusterID = 1;
    std::vector<std::vector<size_t>> clusters;

    for (auto &p : points) {
        if (p.getClusterID() == cUnclassified) {
            std::pair<bool, std::vector<size_t>> expansion = expandCluster(p, clusterID, maxDistance, minPoints, points);
            if (expansion.first) {
                clusters.push_back(expansion.second);
                ++clusterID;
            }
        }
    }

    return clusters;
}

std::vector<std::vector<size_t>> DBScan::normals(double maxDistance, unsigned int minPoints, std::vector<Point> &points, double normalDispersion) {
    int clusterID = 1;
    std::vector<std::vector<size_t>> clusters;

    Octree map(points);
    for (auto &p : points) {
        if (p.getClusterID() == cUnclassified) {
            std::vector<Point *> neighbours = map.searchNeighbors(p, maxDistance, Kernel_t::sphere);

            // Debe tener mínimo 3 vecinos para el cálculo de la normal, sino el punto se descarta
            if (neighbours.size() > 2) {
                Vector normal = PlaneUtils::computeNormal(neighbours);
                if (normal.getX() < 0) {
                    normal = normal * -1.0;
                }

                std::pair<bool, std::vector<size_t>> expansion = expandNormal(normal, p, clusterID, maxDistance, minPoints, map, points, normalDispersion);
                if (expansion.first) {
                    clusters.push_back(expansion.second);
                    ++clusterID;
                }
            }
        }
    }

    return clusters;
}

std::pair<bool, std::vector<size_t>> DBScan::expandCluster(Point &centroid, int clusterID, double maxDistance, unsigned int minPoints, std::vector<Point> &points) {
    std::vector<size_t> clusterSeeds = calculateCluster(centroid, maxDistance, points);

    // Centroide no contiene la cantidad mínima de puntos
    if (clusterSeeds.size() < minPoints) {
        centroid.setClusterID(cNoise);
        return {false, {}};
    }

    // Expandimos el cluster
    else {
        std::vector<size_t> clusterPoints = clusterSeeds;

        size_t index = 0, indexCorePoint = 0;
        for (auto &i : clusterSeeds) {
            points[i].setClusterID(clusterID);
            if (points[i] == centroid) {
                indexCorePoint = index;
            }
            ++index;
        }
        clusterSeeds.erase(clusterSeeds.begin() + indexCorePoint);  // Eliminamos el centroide para el calculo de vecinos

        // Expandimos a través de los puntos vecinos al centroide
        for (size_t i = 0, seedsSize = clusterSeeds.size(); i < seedsSize; ++i) {
            std::vector<size_t> clusterNeighbours = calculateCluster(points[clusterSeeds[i]], maxDistance, points);

            if (clusterNeighbours.size() >= minPoints) {
                for (auto &i : clusterNeighbours) {
                    if (points[i].getClusterID() == cUnclassified || points[i].getClusterID() == cNoise) {
                        if (points[i].getClusterID() == cUnclassified) {
                            clusterSeeds.push_back(i);
                            seedsSize = clusterSeeds.size();
                        }
                        points[i].setClusterID(clusterID);

                        clusterPoints.push_back(i);  // Añadimos punto al vector de indices totales
                    }
                }
            }
        }

        return {true, clusterPoints};
    }
}

std::pair<bool, std::vector<size_t>> DBScan::expandNormal(const Vector &cnormal, Point &centroid, int clusterID, double maxDistance, unsigned int minPoints, const Octree &pointsMap, std::vector<Point> &points, double normalDispersion) {
    std::vector<size_t> clusterSeeds = calculateNormal(cnormal, centroid, maxDistance, pointsMap, points, normalDispersion);

    // Centroide no contiene la cantidad mínima de puntos
    if (clusterSeeds.size() < minPoints) {
        centroid.setClusterID(cNoise);
        return {false, {}};
    }

    // Expandimos el cluster
    else {
        std::vector<size_t> clusterPoints = clusterSeeds;

        size_t index = 0, indexCorePoint = 0;
        for (auto &i : clusterSeeds) {
            points[i].setClusterID(clusterID);
            if (points[i] == centroid) {
                indexCorePoint = index;
            }
            ++index;
        }
        clusterSeeds.erase(clusterSeeds.begin() + indexCorePoint);  // Eliminamos el centroide para el calculo de vecinos

        // Expandimos a través de los puntos vecinos al centroide
        for (size_t i = 0, seedsSize = clusterSeeds.size(); i < seedsSize; ++i) {
            std::vector<Point *> neighbours = pointsMap.searchNeighbors(points[clusterSeeds[i]], maxDistance, Kernel_t::sphere);

            // Debe tener mínimo 3 vecinos para el cálculo de la normal, sino el punto se descarta
            if (neighbours.size() > 2) {
                Vector normal = PlaneUtils::computeNormal(neighbours);
                if (normal.getX() < 0) {
                    normal = normal * -1.0;
                }

                std::vector<size_t> clusterNeighbours = calculateNormal(normal, points[clusterSeeds[i]], maxDistance, pointsMap, points, normalDispersion);

                if (clusterNeighbours.size() >= minPoints) {
                    for (auto &i : clusterNeighbours) {
                        if (points[i].getClusterID() == cUnclassified || points[i].getClusterID() == cNoise) {
                            if (points[i].getClusterID() == cUnclassified) {
                                clusterSeeds.push_back(i);
                                seedsSize = clusterSeeds.size();
                            }
                            points[i].setClusterID(clusterID);

                            clusterPoints.push_back(i);  // Añadimos punto al vector de indices totales
                        }
                    }
                }
            }
        }

        return {true, clusterPoints};
    }
}

std::vector<size_t> DBScan::calculateCluster(const Point &centroid, double maxDistance, const std::vector<Point> &points) {
    int index = 0;
    std::vector<size_t> clusterIndex;

    for (auto &p : points) {
        if (centroid.distance3D(p) <= maxDistance) {
            clusterIndex.push_back(index);
        }
        ++index;
    }

    return clusterIndex;
}

std::vector<size_t> DBScan::calculateNormal(const Vector &cnormal, const Point &centroid, double maxDistance, const Octree &map, const std::vector<Point> &points, double normalDispersion) {
    int index = 0;
    std::vector<size_t> clusterIndex;

    for (auto &p : points) {
        std::vector<Point *> neighbours = map.searchNeighbors(p, maxDistance, Kernel_t::sphere);

        // Debe tener mínimo 3 vecinos para el cálculo de la normal, sino el punto se descarta
        if (neighbours.size() > 2) {
            Vector normal = PlaneUtils::computeNormal(neighbours);
            if (normal.getX() < 0) {
                normal = normal * -1.0;
            }

            if (cnormal.distance3D(normal) <= normalDispersion && centroid.distance3D(p) <= maxDistance) {
                clusterIndex.push_back(index);
            }
        }
        ++index;
    }

    return clusterIndex;
}
