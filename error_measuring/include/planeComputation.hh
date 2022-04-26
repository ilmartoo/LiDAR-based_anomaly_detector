#include "armadillo"

#include "DoublePoint.hh"

/* Calcula el centroide de los puntos */
DoublePoint computeCentroid(const std::vector<DoublePoint> &points) {
    double x{}, y{}, z{};
    size_t numPoints = points.size();

    for (const auto &p : points) {
        x += p.getX();
        y += p.getY();
        z += p.getZ();
    }

    x /= numPoints;
    y /= numPoints;
    z /= numPoints;

    return DoublePoint(x, y, z);
}

/* ?? */
void computeSVD(const std::vector<DoublePoint> &points, arma::mat &U, arma::vec &s, arma::mat &V) {
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

    // SVD
    arma::svd_econ(U, s, V, P);
}

/* Computa la normal del plano */
arma::vec computeNormal(const std::vector<DoublePoint> &points) {
    arma::vec vnormal(3);

    // SVD
    arma::mat U, V;
    arma::vec s;
    computeSVD(points, U, s, V);

    vnormal = U.col(2);

    return vnormal;
}

/* Computa el plano */
arma::vec computePlane(arma::vec &vnormal, arma::vec &centroid) {
    arma::vec plane(4);

    plane[0] = vnormal[0];
    plane[1] = vnormal[1];
    plane[2] = vnormal[2];
    plane[3] = (-plane[0] * centroid[0] + plane[1] * centroid[1] + plane[2] * centroid[2]);

    return plane;
}

/* Computa el plano segun un centroide DoublePoint */
arma::vec computePlane(arma::vec &vnormal, DoublePoint &centroid) {
    arma::vec plane(4);

    plane[0] = vnormal[0];
    plane[1] = vnormal[1];
    plane[2] = vnormal[2];
    plane[3] = (-plane[0] * centroid[0] + plane[1] * centroid[1] + plane[2] * centroid[2]);

    return plane;
}