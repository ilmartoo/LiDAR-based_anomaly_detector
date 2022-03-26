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

#include "scanner/ScannerFile.hh"
#include "models/Point.hh"
#include "debug_lbad.hh"

// Destructor
ScannerFile::~ScannerFile() {
    if (this->infile.is_open()) {
        this->infile.close();
    }
};

// Inicialización del escaner
bool ScannerFile::initScanner() {
    printDebug("Inicializando el escaner de archivos.");  // debug

    // Abrimos stream del archivo
    this->infile.open(this->filename, std::ios::in);
    if (this->infile.fail()) {
        std::cerr << "Fallo al inicializar el escaner de archivos." << std::endl;
        return false;
    }

    printDebug("Escaner de archivos inicializado correctamente.");  // debug

    return true;
}

// Comienza la obtención de puntos
bool ScannerFile::startScanner() {
    printDebug("Inicio del escaneo de puntos.");  // debug

    if (this->infile.is_open()) {
        std::string line;     // String de la linea
        std::string data[5];  // Strings de las celdas

        // Proceso de lectura de puntos
        std::getline(infile, line);  // Linea de cabecera

        for (int commas, i; std::getline(infile, line);) {
            // Fallo en la lectura
            if (infile.fail()) {
                return false;
            }

            commas = 0;  // Contador de comas
            i = 0;       // Indice del string

            // Datos no necesarios
            for (; commas < 7; ++i) {
                if (line[i] == ',') {
                    ++commas;
                }
            }

            // Timestamp
            data[0] = line.substr(i, line.substr(i).find_first_of(','));

            // Datos no necesarios
            for (commas = 0; commas < 3; ++i) {
                if (line[i] == ',') {
                    ++commas;
                }
            }

            // Reflectividad
            data[1] = line.substr(i, line.substr(i).find_first_of(','));

            // Datos no necesarios
            for (commas = 0; commas < 1; ++i) {
                if (line[i] == ',') {
                    ++commas;
                }
            }

            // X, Y, Z
            data[2] = line.substr(i, line.substr(i).find_first_of(','));
            i += data[2].length();
            data[3] = line.substr(i, line.substr(i).find_first_of(','));
            i += data[3].length();
            data[4] = line.substr(i, line.substr(i).find_first_of(','));

            // Creación del punto
            Point p(static_cast<uint64_t>(std::stoull(data[0])), static_cast<uint8_t>(std::stoull(data[1])),
                    static_cast<uint32_t>(std::stoul(data[2])), static_cast<uint32_t>(std::stoul(data[3])),
                    static_cast<uint32_t>(std::stoul(data[4])));

            // Llamada al callback
            if (this->callback) {
                this->callback(p);
            }
        }
    }
    // Fallo de apertura
    else {
        std::cerr << "Fallo de apertura del archivo de puntos." << std::endl;
        return false;
    }

    this->infile.close();
    return true;
};

// Establece la función especificada como función de callback a la que se llamará cada vez que
// se escanee un nuevo punto
bool ScannerFile::setCallback(const std::function<void(Point)> func) {
    printDebug("Estableciendo el callback.");  // debug

    this->callback = func;
    return ((bool)callback);
}

// Finaliza el escaner
inline void ScannerFile::closeScanner() {
    printDebug("Finalizando el escaneo de puntos.");

    this->infile.close();

    printDebug("Finalizado el escaneo de puntos.");
}