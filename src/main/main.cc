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
#include <vector>
#include <stdlib.h>

#include "scanner/IScanner.hh"
#include "scanner/ScannerFile.hh"
#include "models/Point.hh"
#include "app/App.hh"
#include "app/InputParser.hh"

// Main function
// int main(int argc, char* argv[]) {
//     // Vector de argumentos
//     //...

//     return 0;
// }

// Funcionality test
void callback(Point p);
int main(int argc, char* argv[]) {
    IScanner* scanner = new ScannerFile("data/puntos_test.csv");

    scanner->initScanner();

    scanner->setCallback(callback);

    scanner->startScanner();

    scanner->closeScanner();

    delete scanner;

    return EXIT_SUCCESS;
}

void callback(Point p) {
    std::cout << p << std::endl;
}
