/**
 * @file main.cpp
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 19/03/2022
 *
 * Función main del programa
 *
 */

#include <iostream>
#include <string>
#include <stdlib.h>

#include "app/App.hh"
#include "app/InputParser.hh"

void help();  // Command line help

// Main function
int main(int argc, char* argv[]) {
    // Parser de argumentos
    InputParser parser(argc, argv);

    // Impresión de la ayuda
    if (parser.optionExists("-h") || parser.optionExists("--help")) {
        usage();              // Imprimimos usage
        help();               // Imprimimos help
        return EXIT_SUCCESS;  // Salimos
    }

    // Input por lidar
    if (parser.optionExists("-b")) {
    }

    // Input por archivo
    else if (parser.optionExists("-f")) {
    }

    // No se ha especificado una de las opciones obligatorias
    else {
        usage();              // Imprimimos usage
        return EXIT_SUCCESS;  // Salimos
    }

    return EXIT_SUCCESS;
}

// Command line help
void help() {
    std::cout << " -b                Broadcast code of the lidar sensor" << std::endl
              << " -f                File with the 3D points to get the data from" << std::endl
              << " -t                Tipo de mediciones de tiempo a tomar:" << std::endl
              << "                       notime - No se toman mediciones" << std::endl
              << "                       char   - Mediciones de la caracterización" << std::endl
              << "                       anom   - Mediciones en la deteccion de anomalías" << std::endl
              << "                       all    - Mediciones en todos los apartados" << std::endl
              << " -h, --help        Print this help" << std::endl
              << std::endl;
}

// Command line usage
void usage() {
    std::cout << std::endl
              << "Usage:" << std::endl
              << " lbad <-b broadcast_code> [-t time_mode]" << std::endl
              << " lbad <-f filename> [-t time_mode]" << std::endl
              << " lbad <-h | --help>" << std::endl
              << std::endl;
}

// Funcionality test
// #include "scanner/IScanner.hh"
// #include "scanner/ScannerFile.hh"
// #include "models/Point.hh"
// void callback(Point p);
// int main(int argc, char* argv[]) {
//     IScanner* scanner = new ScannerFile("data/puntos_test.csv");

//     scanner->initScanner();

//     scanner->setCallback(callback);

//     scanner->startScanner();

//     scanner->closeScanner();

//     delete scanner;

//     return EXIT_SUCCESS;
// }

// void callback(Point p) {
//     std::cout << p << std::endl;
// }
