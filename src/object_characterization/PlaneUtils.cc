/**
 * @file PlaneUtils.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 12/05/2022
 *
 * Implementación del objeto PlaneUtils
 *
 */

#include <vector>
#include <omp.h>

#include "armadillo"

#include "object_characterization/PlaneUtils.hh"
#include "models/Point.hh"

Point PlaneUtils::computeCentroid(const std::vector<Point> &points) {
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

Point PlaneUtils::computeCentroid(const std::vector<Point *> &points) {
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

void PlaneUtils::computeSVD(const std::vector<Point> &points, arma::mat &U, arma::vec &s, arma::mat &V) {
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

void PlaneUtils::computeSVD(const std::vector<Point *> &points, arma::mat &U, arma::vec &s, arma::mat &V) {
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

Vector PlaneUtils::computeNormal(const std::vector<Point> &points) {
    arma::vec vnormal(3);

    arma::mat U, V;
    arma::vec s;
    computeSVD(points, U, s, V);

    vnormal = U.col(2);

    return Vector(vnormal[0], vnormal[1], vnormal[2]);
}

Vector PlaneUtils::computeNormal(const std::vector<Point *> &points) {
    arma::vec vnormal(3);

    arma::mat U, V;
    arma::vec s;
    computeSVD(points, U, s, V);

    vnormal = U.col(2);

    return Vector(vnormal[0], vnormal[1], vnormal[2]);
}

std::vector<Vector> PlaneUtils::computeNormals(std::vector<Point> &points, double distance) {
    std::vector<Vector> normals(points.size(), Vector(0, 0, 0));
    Octree map(points);

#pragma omp parallel for num_threads(NORMAL_CALCULATION_THREADS) shared(map, normals) schedule(guided)
    for (size_t i = 0; i < points.size(); ++i) {
        std::vector<Point *> neighbours = map.searchNeighbors(points[i], distance, Kernel_t::sphere);

        // Para el cálculo de la normal se necesitan un mínimo de 3 puntos vecinos
        // En el caso de no cumplir este requerimiento el punto no tendrá una normal válida asignada
        if (neighbours.size() > 2) {
            normals[i] = PlaneUtils::computeNormal(neighbours);
            if (normals[i].getX() < 0) {
                normals[i] = normals[i] * -1;
            }
        }
    }

    return normals;
}

arma::vec PlaneUtils::computePlane(const Vector &vnormal, const Point &centroid) {
    arma::vec plane(4);

    plane[0] = vnormal.getX();
    plane[1] = vnormal.getY();
    plane[2] = vnormal.getZ();
    plane[3] = (-plane[0] * centroid.getX() + plane[1] * centroid.getY() + plane[2] * centroid.getZ());

    return plane;
}

arma::vec PlaneUtils::computePlane(const std::vector<Point> &points) {
    arma::vec plane(4);
    Vector vnormal = computeNormal(points);
    Point centroid = computeNormal(points);

    plane[0] = vnormal.getX();
    plane[1] = vnormal.getY();
    plane[2] = vnormal.getZ();
    plane[3] = (-plane[0] * centroid.getX() + plane[1] * centroid.getY() + plane[2] * centroid.getZ());

    return plane;
}

arma::vec PlaneUtils::computePlane(const std::vector<Point *> &points) {
    arma::vec plane(4);
    Vector vnormal = computeNormal(points);
    Point centroid = computeNormal(points);

    plane[0] = vnormal.getX();
    plane[1] = vnormal.getY();
    plane[2] = vnormal.getZ();
    plane[3] = (-plane[0] * centroid.getX() + plane[1] * centroid.getY() + plane[2] * centroid.getZ());

    return plane;
}