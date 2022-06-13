/**
 * @file Geometry.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 12/05/2022
 *
 * @brief Implementación del objeto Geometry
 *
 */

#include <vector>
#include <utility>
#include <omp.h>

#include "armadillo"

#include "object_characterization/Face.hh"
#include "models/Geometry.hh"
#include "models/Point.hh"
#include "models/BBox.hh"
#include "app/config.h"

Point Geometry::computeCentroid(const std::vector<Point> &points) {
    double x = 0., y = 0., z = 0.;
    size_t numPoints = points.size();

    for (const auto &p : points) {
        x += p.getX();
        y += p.getY();
        z += p.getZ();
    }

    x /= numPoints;
    y /= numPoints;
    z /= numPoints;

    return Point(x, y, z);
}

Point Geometry::computeCentroid(const std::vector<Point *> &points) {
    double x = 0., y = 0., z = 0.;
    size_t numPoints = points.size();

    for (const auto &p : points) {
        x += p->getX();
        y += p->getY();
        z += p->getZ();
    }

    x /= numPoints;
    y /= numPoints;
    z /= numPoints;

    return Point(x, y, z);
}

void Geometry::computeSVD(const std::vector<Point> &points, arma::mat &U, arma::vec &s, arma::mat &V) {
    arma::mat P(3, points.size());

    size_t idx = 0;

    for (const auto &p : points) {
        P(0, idx) = p.getX();
        P(1, idx) = p.getY();
        P(2, idx) = p.getZ();
        ++idx;
    }

    arma::vec m = arma::mean(P, 1);
    P.row(0) -= m(0);
    P.row(1) -= m(1);
    P.row(2) -= m(2);

    arma::svd_econ(U, s, V, P);
}

void Geometry::computeSVD(const std::vector<Point *> &points, arma::mat &U, arma::vec &s, arma::mat &V) {
    arma::mat P(3, points.size());

    size_t idx = 0;

    for (const auto &p : points) {
        P(0, idx) = p->getX();
        P(1, idx) = p->getY();
        P(2, idx) = p->getZ();
        ++idx;
    }

    arma::vec m = arma::mean(P, 1);
    P.row(0) -= m(0);
    P.row(1) -= m(1);
    P.row(2) -= m(2);

    arma::svd_econ(U, s, V, P);
}

Vector Geometry::computeNormal(const std::vector<Point> &points) {
    arma::vec vnormal(3);

    arma::mat U, V;
    arma::vec s;
    computeSVD(points, U, s, V);

    vnormal = U.col(2);

    return Vector(vnormal[0], vnormal[1], vnormal[2]);
}

Vector Geometry::computeNormal(const std::vector<Point *> &points) {
    arma::vec vnormal(3);

    arma::mat U, V;
    arma::vec s;
    computeSVD(points, U, s, V);

    vnormal = U.col(2);

    return Vector(vnormal[0], vnormal[1], vnormal[2]);
}

std::vector<Vector> Geometry::computeNormals(std::vector<Point> &points, const Octree &map, double distance) {
    std::vector<Vector> normals(points.size(), Vector(0, 0, 0));

#pragma omp parallel for schedule(OMP_SCHEDULE_TYPE, OMP_CHUNK_SIZE)
    for (size_t i = 0; i < points.size(); ++i) {
        std::vector<Point *> neighbours = map.searchNeighbors(points[i], distance, Kernel_t::sphere);

        // Para el cálculo de la normal se necesitan un mínimo de 3 puntos vecinos
        // En el caso de no cumplir este requerimiento el punto no tendrá una normal válida asignada
        if (neighbours.size() > 2) {
            normals[i] = Geometry::computeNormal(neighbours);
            if (normals[i].getX() < 0) {
                normals[i] = normals[i] * -1;
            }
        }
    }

    return normals;
}

arma::vec4 Geometry::computePlane(const Vector &vnormal, const Point &centroid) {
    arma::vec4 plane;

    plane[0] = vnormal.getX();
    plane[1] = vnormal.getY();
    plane[2] = vnormal.getZ();
    plane[3] = (-plane[0] * centroid.getX() + plane[1] * centroid.getY() + plane[2] * centroid.getZ());

    return plane;
}

arma::vec4 Geometry::computePlane(const std::vector<Point> &points) {
    arma::vec4 plane;
    Vector vnormal = computeNormal(points);
    Point centroid = computeCentroid(points);

    plane[0] = vnormal.getX();
    plane[1] = vnormal.getY();
    plane[2] = vnormal.getZ();
    plane[3] = (-plane[0] * centroid.getX() + plane[1] * centroid.getY() + plane[2] * centroid.getZ());

    return plane;
}

arma::vec4 Geometry::computePlane(const std::vector<Point *> &points) {
    arma::vec4 plane;
    Vector vnormal = computeNormal(points);
    Point centroid = computeCentroid(points);

    plane[0] = vnormal.getX();
    plane[1] = vnormal.getY();
    plane[2] = vnormal.getZ();
    plane[3] = -(plane[0] * centroid.getX() + plane[1] * centroid.getY() + plane[2] * centroid.getZ());

    return plane;
}

arma::mat33 Geometry::rotationMatrix(double xdeg, double ydeg, double zdeg) {
    double gamma = xdeg * RAD_PER_DEG;
    double beta = ydeg * RAD_PER_DEG;
    double alpha = zdeg * RAD_PER_DEG;
    double ca = cos(alpha);
    double sa = sin(alpha);
    double cb = cos(beta);
    double sb = sin(beta);
    double cg = cos(gamma);
    double sg = sin(gamma);
    return {{ca * cb,
             ca * sb * sg - sa * cg,
             ca * sb * cg + sa * sg},
            {sa * cb,
             sa * sb * sg + ca * cg,
             sa * sb * cg - ca * sg},
            {-sb,
             cb * sg,
             cb * cg}};
}

arma::mat33 Geometry::rotationMatrix(const Vector &deg) {
    Vector rad = deg * RAD_PER_DEG;
    double ca = cos(rad.getZ());
    double sa = sin(rad.getZ());
    double cb = cos(rad.getY());
    double sb = sin(rad.getY());
    double cg = cos(rad.getX());
    double sg = sin(rad.getX());
    return {{ca * cb,
             ca * sb * sg - sa * cg,
             ca * sb * cg + sa * sg},
            {sa * cb,
             sa * sb * sg + ca * cg,
             sa * sb * cg - ca * sg},
            {-sb,
             cb * sg,
             cb * cg}};
}

std::pair<BBox, Vector> Geometry::minimumBBoxRotTrans(std::vector<Point> &points) {
    Vector rotmin(0, 0, 0);  // Ángulos de rotación iniciales
    BBox bbmin(points);      // BBox sin rotacion
    arma::mat33 rotmatrix, orirotmatrix;
    Point trans;

#pragma omp parallel
    {
        // Rotaciones amplias en las tres dimensiones
#pragma omp for collapse(3) schedule(OMP_SCHEDULE_TYPE, OMP_CHUNK_SIZE)
        for (int i = 0; i < 90; i += 6) {
            for (int j = 0; j < 90; j += 6) {
                for (int k = 0; k < 90; k += 6) {
                    // El primer elemento es el mínimo por defecto
                    if (i == 0 && j == 0 && k == 0) {
                        continue;
                    } else {
                        BBox bb(points, rotationMatrix(i, j, k));
#pragma omp critical
                        if (bb < bbmin) {
                            bbmin = bb;
                            rotmin = Vector(i, j, k);
                        }
                    }
                }
            }
        }
        // Rotaciones pequeñas dentro del radio de la mejor rotación grande
#pragma omp for collapse(3) schedule(OMP_SCHEDULE_TYPE, OMP_CHUNK_SIZE)
        for (int i = (int)rotmin.getX() - 5; i < (int)rotmin.getX() + 6; ++i) {
            for (int j = (int)rotmin.getY() - 5; j < (int)rotmin.getY() + 6; ++j) {
                for (int k = (int)rotmin.getZ() - 5; k < (int)rotmin.getZ() + 6; ++k) {
                    // El primer elemento es el mínimo por defecto
                    if (i == (int)rotmin.getX() && j == (int)rotmin.getY() && k == (int)rotmin.getZ()) {
                        continue;
                    } else {
                        BBox bb(points, rotationMatrix(i, j, k));
#pragma omp critical
                        if (bb < bbmin) {
                            bbmin = bb;
                            rotmin = Vector(i, j, k);
                        }
                    }
                }
            }
        }
#pragma omp single
        {
            // Matriz de rotación para obtener la posición de menor volumen
            rotmatrix = Geometry::rotationMatrix(rotmin);

            // Translación para llevar el centro de la bounding box al (0, 0, 0)
            trans = Point(0, 0, 0) - ((bbmin.getDelta() / 2) + bbmin.getMin());

            // Obtener la mejor orientación de la bounding box con < largo, < ancho y < alto en este orden
            auto bestOri = bestOrientation(bbmin);

            // Rotación con la bounding box en (0, 0, 0) para obtener la mejor orientación
            orirotmatrix = Geometry::rotationMatrix(bestOri.second);

            // Calculo de la bounding box despues de las rotacion, translacion y rotación hacia la mejor orientación
            Point halfDim(bestOri.first.getDelta() / 2);
            bbmin = {halfDim, Point(0, 0, 0) - halfDim};
        }
        // Implicit barrier
#pragma omp for schedule(OMP_SCHEDULE_TYPE, OMP_CHUNK_SIZE)
        for (size_t i = 0; i < points.size(); ++i) {
            Point p = (points[i].rotate(rotmatrix) + trans).rotate(orirotmatrix);
            points[i] = Point(p.getX(), p.getY(), p.getZ(), points[i].getClusterID());
        }
    }

    return {bbmin, rotmin};
}

std::pair<BBox, Vector> Geometry::minimumBBox(const std::vector<Point> &points) {
    Vector rotmin(0, 0, 0);  // Ángulos de rotación iniciales
    BBox bbmin(points);      // BBox sin rotacion
    arma::mat33 rotmatrix;
    Point trans;

#pragma omp parallel
    {
        // Rotaciones amplias en las tres dimensiones
#pragma omp for collapse(3) schedule(OMP_SCHEDULE_TYPE, OMP_CHUNK_SIZE)
        for (int i = 0; i < 90; i += 6) {
            for (int j = 0; j < 90; j += 6) {
                for (int k = 0; k < 90; k += 6) {
                    // El primer elemento es el mínimo por defecto
                    if (i == 0 && j == 0 && k == 0) {
                        continue;
                    } else {
                        BBox bb(points, rotationMatrix(i, j, k));
#pragma omp critical
                        if (bb < bbmin) {
                            bbmin = bb;
                            rotmin = Vector(i, j, k);
                        }
                    }
                }
            }
        }
        // Rotaciones pequeñas dentro del radio de la mejor rotación grande
#pragma omp for collapse(3) schedule(OMP_SCHEDULE_TYPE, OMP_CHUNK_SIZE)
        for (int i = (int)rotmin.getX() - 5; i < (int)rotmin.getX() + 6; ++i) {
            for (int j = (int)rotmin.getY() - 5; j < (int)rotmin.getY() + 6; ++j) {
                for (int k = (int)rotmin.getZ() - 5; k < (int)rotmin.getZ() + 6; ++k) {
                    // El primer elemento es el mínimo por defecto
                    if (i == (int)rotmin.getX() && j == (int)rotmin.getY() && k == (int)rotmin.getZ()) {
                        continue;
                    } else {
                        BBox bb(points, rotationMatrix(i, j, k));
#pragma omp critical
                        if (bb < bbmin) {
                            bbmin = bb;
                            rotmin = Vector(i, j, k);
                        }
                    }
                }
            }
        }
        // Bounding box de mejor orientación dentro de las 6 posibles
#pragma omp single
        {
            auto temp = bestOrientation(bbmin);
            bbmin = temp.first;
            rotmin = rotmin + temp.second;
        }
    }
    return {BBox(bbmin.getDelta()), rotmin};
}

std::vector<std::pair<BBox, Vector>> Geometry::minimumBBoxes(const std::vector<std::vector<Point *>> &points) {
    std::vector<std::pair<BBox, Vector>> bboxes = {points.size(), {{}, Vector(0, 0, 0)}};

#pragma omp parallel
    {
        for (size_t v = 0; v < points.size(); ++v) {
#pragma omp single
            {
                bboxes[v].first = BBox(points[v]);
            }
            // Rotaciones amplias en las tres dimensiones
#pragma omp for collapse(3) schedule(OMP_SCHEDULE_TYPE, OMP_CHUNK_SIZE)
            for (int i = 0; i < 90; i += 6) {
                for (int j = 0; j < 90; j += 6) {
                    for (int k = 0; k < 90; k += 6) {
                        // El primer elemento es el mínimo por defecto
                        if (i == 0 && j == 0 && k == 0) {
                            continue;
                        } else {
                            BBox bb(points[v], rotationMatrix(i, j, k));
#pragma omp critical
                            if (bb <= bboxes[v].first) {
                                bboxes[v].first = bb;
                                bboxes[v].second = Vector(i, j, k);
                            }
                        }
                    }
                }
            }
            // Rotaciones pequeñas dentro del radio de la mejor rotación grande
#pragma omp for collapse(3) schedule(OMP_SCHEDULE_TYPE, OMP_CHUNK_SIZE)
            for (int i = (int)bboxes[v].second.getX() - 5; i < (int)bboxes[v].second.getX() + 6; ++i) {
                for (int j = (int)bboxes[v].second.getY() - 5; j < (int)bboxes[v].second.getY() + 6; ++j) {
                    for (int k = (int)bboxes[v].second.getZ() - 5; k < (int)bboxes[v].second.getZ() + 6; ++k) {
                        if (i == (int)bboxes[v].second.getX() && j == (int)bboxes[v].second.getY() && k == (int)bboxes[v].second.getZ()) {
                            continue;
                        } else {
                            BBox bb(points[v], rotationMatrix(i, j, k));
#pragma omp critical
                            if (bb <= bboxes[v].first) {
                                bboxes[v].first = bb;
                                bboxes[v].second = Vector(i, j, k);
                            }
                        }
                    }
                }
            }
            // Bounding box de mejor orientación dentro de las 6 posibles
#pragma omp single
            {
                auto temp = bestOrientation(bboxes[v].first);
                bboxes[v].first = temp.first;
                bboxes[v].second = bboxes[v].second + temp.second;
            }
        }
    }
    return bboxes;
}

Point Geometry::mean(const std::vector<Point> &points) {
    Point m(0, 0, 0);
    for (auto &p : points) {
        m = m + p;
    }
    return m / points.size();
}

Point Geometry::mean(const std::vector<Point *> &points) {
    Point m(0, 0, 0);
    for (auto &p : points) {
        m = m + *p;
    }
    return m / points.size();
}

std::pair<BBox, Vector> Geometry::bestOrientation(const BBox &bbox) {
    const Vector &bd = bbox.getDelta();
    Vector deltas[6] = {bd,                                  // [x,y,z]
                        {bd.getX(), bd.getZ(), bd.getY()},   // [x,z,y]
                        {bd.getY(), bd.getX(), bd.getZ()},   // [y,x,z]
                        {bd.getY(), bd.getZ(), bd.getX()},   // [y,z,x]
                        {bd.getZ(), bd.getX(), bd.getY()},   // [z,x,y]
                        {bd.getZ(), bd.getY(), bd.getX()}};  // [z,y,x]

    static const Vector rotations[6] = {{0, 0, 0},    // [x,y,z]
                                        {90, 0, 0},   // [x,z,y]
                                        {0, 0, 90},   // [y,x,z]
                                        {90, 0, 90},  // [y,z,x]
                                        {0, 90, 90},  // [z,x,y]
                                        {0, 90, 0}};  // [z,y,x]

    int bestDim = 0;
    for (int i = 1; i < 6; ++i) {
        if (betterDimensions(deltas[i], deltas[bestDim])) {
            bestDim = i;
        }
    }
    return {BBox(deltas[bestDim]), rotations[bestDim]};
}

bool Geometry::betterDimensions(const Vector &newDim, const Vector &oldDim) {
    // Ajustamos a milimetros
    int x = (int)(oldDim.getX() - newDim.getX());
    int y = (int)(oldDim.getY() - newDim.getY());
    int z = (int)(oldDim.getZ() - newDim.getZ());
    // Comprobamos orientación
    return x > 0 || (x == 0 && (y > 0 || (y == 0 && z > 0)));
}