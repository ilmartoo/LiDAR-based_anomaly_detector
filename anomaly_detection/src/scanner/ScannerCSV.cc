/**
 * @file ScannerCSV.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 19/03/2022
 *
 * Implementación del objeto ScannerCSV
 *
 */

#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <exception>

#include "scanner/ScannerCSV.hh"
#include "models/LidarPoint.hh"
#include "models/Timestamp.hh"
#include "app/CLICommand.hh"

#include "logging/debug.hh"

bool ScannerCSV::init() {
    DEBUG_STDOUT("Inicializando el escaner de archivos csv.");

    // Abrimos stream del archivo
    infile.open(filename, std::ifstream::in);
    if (infile.fail()) {
        CLI_STDERR("Fallo al inicializar el escaner de archivos csv.");
        return false;
    }

    DEBUG_STDOUT("Escaner de archivos csv inicializado correctamente.");

    return true;
}

ScanCode ScannerCSV::scan() {
    DEBUG_STDOUT("Inicio del escaneo de puntos.");

    if (!infile.is_open()) {
        infile.open(filename, std::ios::in);
    }
    // Vuelve al principio del archivo
    else if (infile.eof()) {
        infile.seekg(std::ios::beg);
    }

    if (!infile.fail()) {
        if (!scanning) {
            scanning = true;
            return readData();

        } else {
            CLI_STDERR("El sensor ya está escaneando.");
            return ScanCode::kScanError;
        }
    }
    // Fallo de apertura
    else {
        CLI_STDERR("Fallo de apertura del archivo CSV de puntos.");
        return ScanCode::kScanError;
    }
}

void ScannerCSV::pause() {
    scanning = false;
}

bool ScannerCSV::setCallback(const std::function<void(const LidarPoint &p)> func) {
    DEBUG_STDOUT("Estableciendo el callback.");

    callback = func;
    return ((bool)callback);
}

void ScannerCSV::wait() {
    DEBUG_STDOUT("Esperando a la finalización del escaneo de puntos.");

    readData();

    DEBUG_STDOUT("Finalizado el escaneo de puntos.");
}

void ScannerCSV::stop() {
    DEBUG_STDOUT("Finalizando el escaneo de puntos.");

    if (infile.is_open()) {
        infile.close();
    }

    DEBUG_STDOUT("Finalizado el escaneo de puntos.");
}

ScanCode ScannerCSV::readData() {
    std::string line;     // String de la linea
    std::string data[5];  // Strings de las celdas

    // Proceso de lectura de puntos
    std::getline(this->infile, line);  // Linea de cabecera

    for (int commas, i; scanning && std::getline(infile, line);) {
        // Fallo en la lectura
        if (infile.fail()) {
            DEBUG_STDOUT("Fallo en la lectura de puntos");
            return ScanCode::kScanError;
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
            try {
                this->callback({Timestamp(data[0]), static_cast<uint8_t>(std::stoi(data[1])), static_cast<double>(std::stod(data[2])),
                                static_cast<double>(std::stod(data[3])), static_cast<double>(std::stod(data[4]))});
            } catch (std::exception &e) {
                CLI_STDERR("Error de conversión de datos.");
            }
        }
    }

    if (infile.eof()) {
        DEBUG_STDOUT("Se ha llegado al final del archivo CSV de puntos.");

        scanning = false;
        return ScanCode::kScanEof;
    }

    return ScanCode::kScanOk;
}