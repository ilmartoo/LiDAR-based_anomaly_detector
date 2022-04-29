/**
 * @file ScannerCSV.cpp
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 19/03/2022
 *
 * Implementación del objeto ScannerCSV
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <thread>

#include "scanner/ScannerCSV.hh"
#include "models/Point.hh"
#include "models/Timestamp.hh"

#include "logging/debug.hh"
#include "logging/logging.hh"

// Inicialización del escaner
bool ScannerCSV::init() {
    DEBUG_STDOUT("Inicializando el escaner de archivos csv.");

    // Abrimos stream del archivo
    infile.open(filename, std::ifstream::in);
    if (infile.fail()) {
        LOG_ERROR("Fallo al inicializar el escaner de archivos csv.");
        return false;
    }

    LOG_INFO("Escaner de archivos csv inicializado correctamente.");

    return true;
}

// Comienza la obtención de puntos
bool ScannerCSV::start() {
    LOG_INFO("Inicio del escaneo de puntos.");

    if (infile.is_open()) {
        exit = false;                                                    // Permitimos la ejecución del hilo
        executionThread = new std::thread(&ScannerCSV::readData, this);  // Creamos hilo
    }
    // Fallo de apertura
    else {
        LOG_ERROR("Fallo de apertura del archivo de puntos.");
        return false;
    }

    return true;
};

// Establece la función especificada como función de callback a la que se llamará cada vez que
// se escanee un nuevo punto
bool ScannerCSV::setCallback(const std::function<void(const Point &p)> func) {
    DEBUG_STDOUT("Estableciendo el callback.");

    callback = func;
    return ((bool)callback);
}

// Finaliza el escaner
void ScannerCSV::stop() {
    DEBUG_STDOUT("Finalizando el escaneo de puntos.");

    exit = true;              // Comunicamos al hilo que finalice la ejecución
    executionThread->join();  // Realizamos unión del hilo de lectura

    infile.close();  // Cerramos stream del archivo

    LOG_INFO("Finalizado el escaneo de puntos.");
}

// Lectura de puntos del archivo
void ScannerCSV::readData() {
    std::string line;     // String de la linea
    std::string data[5];  // Strings de las celdas

    // Proceso de lectura de puntos
    std::getline(this->infile, line);  // Linea de cabecera

    for (int commas, i; !exit && std::getline(infile, line);) {
        // Fallo en la lectura
        if (infile.fail()) {
            LOG_INFO("Fallo en la lectura de puntos");
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
        for (commas = 0; commas < 4; ++i) {
            if (line[i] == ',') {
                ++commas;
            }
        }

        // Reflectividad
        data[1] = line.substr(i, line.substr(i).find_first_of(','));

        // Datos no necesarios
        for (commas = 0; commas < 2; ++i) {
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

        // Llamada al callback
        if (this->callback) {
            this->callback(Point(Timestamp(data[0]), static_cast<uint8_t>(std::stoi(data[1])), static_cast<uint32_t>(std::stoi(data[2])),
                                 static_cast<uint32_t>(std::stoi(data[3])), static_cast<uint32_t>(std::stoi(data[4]))));
        }
    }

    DEBUG_STDOUT("Se ha terminado el escaneo del archivo de puntos");
    exit = true;
}