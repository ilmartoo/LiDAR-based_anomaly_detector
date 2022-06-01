/**
 * @file Geometry.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 12/05/2022
 *
 * Implementación del objeto Geometry
 *
 */

#include <vector>
#include <utility>
#include <omp.h>

#include "armadillo"

#include "object_characterization/Face.hh"
#include "models/Geometry.hh"
#include "models/Point.hh"
#include "object_characterization/BBox.hh"
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

#pragma omp parallel for num_threads(PARALELIZATION_NUM_THREADS) schedule(guided)
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

arma::mat33 Geometry::rotationMatrix(int xdeg, int ydeg, int zdeg) {
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

std::pair<BBox, Vector> Geometry::minimumBBoxRotTrans(std::vector<Point *> &points) {
    Vector rotmin(0, 0, 0);  // Ángulos de rotación iniciales
    BBox bbmin(points);      // BBox sin rotacion
    arma::mat33 rotmatrix;
    Point trans;

#pragma omp parallel num_threads(PARALELIZATION_NUM_THREADS)
    {
        // Rotaciones amplias en las tres dimensiones
#pragma omp for collapse(3) schedule(guided)
        for (int i = 0; i < 90; i += 10) {
            for (int j = 0; j < 90; j += 10) {
                for (int k = 0; k < 90; k += 10) {
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
#pragma omp for collapse(3) schedule(guided)
        for (int i = (int)rotmin.getX() - 10; i < (int)rotmin.getX() + 10; ++i) {
            for (int j = (int)rotmin.getY(); j < (int)rotmin.getY() + 10; ++j) {
                for (int k = (int)rotmin.getZ(); k < (int)rotmin.getZ() + 10; ++k) {
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
        // Rotamos puntos originales hacia la posición de menor volumen
#pragma omp single
        {
            rotmatrix = Geometry::rotationMatrix(rotmin);
            trans = Point(0, 0, 0) - bbmin.getMin();
        }
        // Implicit barrier
#pragma omp for schedule(guided)
        for (size_t i = 0; i < points.size(); ++i) {
            *points[i] = points[i]->rotate(rotmatrix) + trans;
        }
    }

    return {BBox(bbmin.getDelta()), rotmin};
}

std::vector<std::pair<BBox, Vector>> Geometry::minimumBBox(const std::vector<std::vector<Point>> &points) {
    std::vector<std::pair<BBox, Vector>> bboxes = {points.size(), {{}, Vector(0, 0, 0)}};

#pragma omp parallel num_threads(PARALELIZATION_NUM_THREADS)
    {
        for (size_t v = 0; v < points.size(); ++v) {
#pragma omp single
            {
                bboxes[v].first = BBox(points[v]);
            }
            // Rotaciones amplias en las tres dimensiones
#pragma omp for collapse(3) schedule(guided)
            for (int i = 0; i < 90; i += 10) {
                for (int j = 0; j < 90; j += 10) {
                    for (int k = 0; k < 90; k += 10) {
                        // El primer elemento es el mínimo por defecto
                        if (i == 0 && j == 0 && k == 0) {
                            continue;
                        } else {
                            BBox bb(points[v], rotationMatrix(i, j, k));
#pragma omp critical
                            if (isBetter(bb, bboxes[v].first)) {
                                bboxes[v].first = bb;
                                bboxes[v].second = Vector(i, j, k);
                            }
                        }
                    }
                }
            }
            // Rotaciones pequeñas dentro del radio de la mejor rotación grande
#pragma omp for collapse(3) schedule(guided)
            for (int i = (int)bboxes[v].second.getX() - 10; i < (int)bboxes[v].second.getX() + 10; ++i) {
                for (int j = (int)bboxes[v].second.getY(); j < (int)bboxes[v].second.getY() + 10; ++j) {
                    for (int k = (int)bboxes[v].second.getZ(); k < (int)bboxes[v].second.getZ() + 10; ++k) {
                        if (i == (int)bboxes[v].second.getX() && j == (int)bboxes[v].second.getY() && k == (int)bboxes[v].second.getZ()) {
                            continue;
                        } else {
                            BBox bb(points[v], rotationMatrix(i, j, k));
#pragma omp critical
                            if (isBetter(bb, bboxes[v].first)) {
                                bboxes[v].first = bb;
                                bboxes[v].second = Vector(i, j, k);
                            }
                        }
                    }
                }
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

bool Geometry::isBetter(const BBox &bb1, const BBox &bb2) {
    return bb1.getDelta().getX() < bb2.getDelta().getX() ||
           (bb1.getDelta().getX() == bb2.getDelta().getX() && bb1.getDelta().getY() < bb2.getDelta().getY()) ||
           (bb1.getDelta().getX() == bb2.getDelta().getX() && bb1.getDelta().getY() == bb2.getDelta().getY() && bb1.getDelta().getZ() < bb2.getDelta().getZ());
};