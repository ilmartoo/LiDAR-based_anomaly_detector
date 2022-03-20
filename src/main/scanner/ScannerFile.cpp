/**
 * @file ScannerFile.cpp
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 19/03/2022
 *
 * Implementación del objeto ScannerFile
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>

#include "scanner/ScannerFile.hpp"
#include "models/Point.hpp"

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
        std::string strLine;      // Buffer de linea
        std::string strCells[4];  // Buffer de celdas

        // Proceso de lectura de puntos
        std::getline(infile, strLine);  // Linea de cabecera

        for (std::string line; std::getline(infile, strLine);) {
            std::istringstream streamLine(strLine);  // Stream de la linea
            int i;

            // Datos no necesarios
            for (i = 0; i < 7; ++i) {
                getline(streamLine, strCells[3], ',');
            }
            // Timestamp
            getline(streamLine, strCells[0], ',');
            // Datos no necesarios
            for (i = 0; i < 5; ++i) {
                getline(streamLine, strCells[3], ',');
            }
            // X, Y, Z
            for (i = 1; i < 4; ++i) {
                getline(streamLine, strCells[i], ',');
            }

            // Creación del punto
            Point p(static_cast<uint32_t>(std::stoull(strCells[0])),
                    static_cast<uint32_t>(std::stoul(strCells[1])),
                    static_cast<uint32_t>(std::stoul(strCells[2])),
                    static_cast<uint32_t>(std::stoul(strCells[3])));

            // Llamada al callback
            if (callback) {
                callback(p);
            }

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
    return ((bool)callback);
}

// Finaliza el escaner
inline void ScannerFile::closeScanner() {
    infile.close();
}