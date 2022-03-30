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
#include <thread>

#include "scanner/ScannerFile.hh"
#include "models/Point.hh"
#include "models/Timestamp.hh"
#include "debug_lbad.hh"

// Inicialización del escaner
bool ScannerFile::init() {
    printDebug("Inicializando el escaner de archivos.");  // debug

    // Abrimos stream del archivo
    infile.open(filename, std::ios::in);
    if (infile.fail()) {
        std::cerr << "Fallo al inicializar el escaner de archivos." << std::endl;
        return false;
    }

    printDebug("Escaner de archivos inicializado correctamente.");  // debug

    return true;
}

// Comienza la obtención de puntos
bool ScannerFile::start() {
    std::cout << "Inicio del escaneo de puntos." << std::endl;

    if (infile.is_open()) {
        executionThread = new std::thread(&ScannerFile::readData, this);
    }
    // Fallo de apertura
    else {
        std::cerr << "Fallo de apertura del archivo de puntos." << std::endl;
        return false;
    }

    return true;
};

// Establece la función especificada como función de callback a la que se llamará cada vez que
// se escanee un nuevo punto
bool ScannerFile::setCallback(const std::function<void(Point)> func) {
    printDebug("Estableciendo el callback.");  // debug

    callback = func;
    return ((bool)callback);
}

// Finaliza el escaner
void ScannerFile::stop() {
    printDebug("Finalizando el escaneo de puntos.");

    executionThread->detach();   // Desasociamos el hilo
    executionThread->~thread();  // Finalizamos hilo de lectura

    infile.close();  // Cerramos stream del archivo

    std::cout << "Finalizado el escaneo de puntos." << std::endl;
}

// Lectura de puntos del archivo
void ScannerFile::readData() {
    std::string line;     // String de la linea
    std::string data[5];  // Strings de las celdas

    // Proceso de lectura de puntos
    std::getline(infile, line);  // Linea de cabecera

    for (int commas, i; std::getline(infile, line);) {
        // Fallo en la lectura
        if (infile.fail()) {
            std::cout << "Fallo en la lectura de puntos" << std::endl;
            return;
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
        i += data[2].length() + 1;  // Pasamos al siguiente valor
        data[3] = line.substr(i, line.substr(i).find_first_of(','));
        i += data[3].length() + 1;  // Pasamos al siguiente valor
        data[4] = line.substr(i, line.substr(i).find_first_of(','));

        // Creación del punto
        Point p(Timestamp(data[0]), std::stof(data[1]), static_cast<uint32_t>(std::stoi(data[2])),
                static_cast<uint32_t>(std::stoi(data[3])), static_cast<uint32_t>(std::stoi(data[4])));

        // Llamada al callback
        if (this->callback) {
            this->callback(p);
        }
    }
}