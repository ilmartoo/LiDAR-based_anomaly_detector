/**
 * @file main.cpp
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @brief Función main del programa
 * @date 19/03/2022
 *
 */

#include <iostream>

#include "scanner/IScanner.hpp"
#include "scanner/ScannerFile.hpp"

int main(int argc, char* argv[]) {
    IScanner* scanner = new ScannerFile("data/puntos_test.csv");

    scanner->initScanner();

    scanner->startScanner();

    scanner->closeScanner();

    delete scanner;

    return 0;
}