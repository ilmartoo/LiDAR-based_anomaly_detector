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

#include "FloatPoint.hh"

#include "scanner/ScannerLVX.hh"
#include "logging/string_format.h"

/* Macro de impresión de errores de entrada */
#define INPUT_ERROR(msg)                    \
    do {                                    \
        std::cerr << red(msg) << std::endl; \
    } while (0)

/* Struct de puntos */
struct FloatPointData {
    FloatPointData() : v(){};

    void callback(const Point& p) { v->push_back(FloatPoint{p}); }

    std::vector<FloatPoint>* v;
}

/* Main */
int main(int argc, char** argv) {
    if (argc != 3) {
        INPUT_ERROR("Número inválido de argumentos!");
        return EXIT_FAILURE;
    }

    FloatPoitData data;  // Buffer de puntos

    /* Obtención de puntos del archivo */
    ScannerLVX s = {string(argv[1])};                             // Creación del escaner
    s.init();                                                     // Inicialización
    s.setCallback([data](const Point& p) { data.callback(p); });  // Callback
    s.start();                                                    // Inicio

    s.wait();  // Non-busy wait hasta que finalize el archivo

    /* Cálculo del plano */
}