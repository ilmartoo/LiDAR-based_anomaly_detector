/**
 * @file main.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 25/04/2022
 *
 * Función main del proyecto de medida del error
 *
 */

#include <iostream>
#include <string>
#include <vector>

#include "armadillo"

#include "DoublePoint.hh"

#include "scanner/ScannerLVX.hh"

/* Declaraciones */
DoublePoint computeCentroid(const std::vector<DoublePoint> &points);
void computeSVD(const std::vector<DoublePoint> &points, arma::mat &U, arma::vec &s, arma::mat &V);
arma::vec computeNormal(const std::vector<DoublePoint> &points);
arma::vec computePlane(arma::vec &vnormal, arma::vec &centroid);
arma::vec computePlane(arma::vec &vnormal, DoublePoint &centroid);

/* Struct de puntos */
struct DoublePointData {
    DoublePointData(int xmin, int xmax, int ymin, int ymax) : x{xmin, xmax}, y{ymin, ymax} { v = new std::vector<DoublePoint>(); };
    ~DoublePointData() { delete v; };

    /* Miembros */
    void callback(const Point &p) {
        if (p.getX() >= x[0] && p.getY() >= y[0] && p.getX() <= x[1] && p.getY() <= y[1]) {
            v->push_back(DoublePoint{p});
        }
    };

    /* Atributos*/
    int x[2], y[2];
    std::vector<DoublePoint> *v;
};

/* Main */
int main(int argc, char **argv) {
    if (argc != 7) {
        std::cerr << "Número inválido de argumentos!\n\terror_measuring <filename> <wall_distance> <x_min> <x_max> <y_min> <y_max>"
                  << std::endl;
        return EXIT_FAILURE;
    }

    struct DoublePointData data(atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));  // Buffer de puntos

    /* Obtención de puntos del archivo */
    ScannerLVX s = {std::string(argv[1])};                         // Creación del escaner
    s.init();                                                      // Inicialización
    s.setCallback([&data](const Point &p) { data.callback(p); });  // Callback
    s.start();                                                     // Inicio

    s.wait();  // Non-busy wait hasta que finalize el archivo

    /* Cálculo del plano */
    DoublePoint centroide = computeCentroid(*data.v);
    arma::vec vnormal = computeNormal(*data.v);
    arma::vec plano = computePlane(vnormal, centroide);

    /* Salida de datos */
    std::cout << "[" << argv[2] << "] " << plano[0] << " " << plano[1] << " " << plano[2] << " " << plano[3] << std::endl;

    return EXIT_SUCCESS;
}

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