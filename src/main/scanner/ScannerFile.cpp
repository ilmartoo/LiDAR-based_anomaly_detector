
#include <iostream>
#include <string>
#include <fstream>
#include <functional>

#include "scanner/ScannerFile.hpp"
#include "Point.hpp"

// Constructor
ScannerFile::ScannerFile(std::string filename) {
    this->filename = filename;
}

// Destructor
ScannerFile::~ScannerFile() {
    infile.close();
};

// Inicialización del escaner
bool ScannerFile::initScanner() {
    infile.open(filename, std::ios::in);
    return !infile.fail();
}

// Comienza la obtención de puntos
bool ScannerFile::startScanner() {
    if (infile.is_open()) {
        std::string line;  // Buffer de linea

        for (std::string line; std::getline(infile, line);) {
            // Proceso de lectura de puntos
            std::cout << line << std::endl;

            // Fallo en la lectura
            if (infile.fail()) {
                return false;
            }
        }
    }
    // Fallo de apertura
    else {
        return false;
    }

    infile.close();
    return true;
};

// Establece la función especificada como función de callback a la que se llamará cada vez que
// se escanee un nuevo punto
bool ScannerFile::setCallback(std::function<void(Point)> func) {
    callback = func;
}

// Finaliza el escaner
inline void ScannerFile::closeScanner() {
    infile.close();
}