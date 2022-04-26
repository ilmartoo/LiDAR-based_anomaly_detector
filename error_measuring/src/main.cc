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
#include "planeComputation.hh"

#include "scanner/ScannerLVX.hh"

/* Struct de puntos */
struct DoublePointData {
    DoublePointData(int xmin, int xmax, int ymin, int ymax) : x{xmin, xmax}, y{ymin, ymax} { v = new std::vector<DoublePoint>(); };
    ~DoublePointData() { delete v; };

    /* Miembros */
    void callback(const Point& p) {
        if (p.getX() >= x[0] && p.getY() >= y[0] && p.getX() <= x[1] && p.getY() <= y[1]) {
            v->push_back(DoublePoint{p});
        }
    };

    /* Atributos*/
    int x[2], y[2];
    std::vector<DoublePoint>* v;
};

/* Main */
int main(int argc, char** argv) {
    if (argc != 7) {
        std::cerr << "Número inválido de argumentos!\n\terror_measuring <filename> <wall_distance> <x_min> <x_max> <y_min> <y_max>"
                  << std::endl;
        return EXIT_FAILURE;
    }

    struct DoublePointData data(atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));  // Buffer de puntos

    /* Obtención de puntos del archivo */
    ScannerLVX s = {std::string(argv[1])};                         // Creación del escaner
    s.init();                                                      // Inicialización
    s.setCallback([&data](const Point& p) { data.callback(p); });  // Callback
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