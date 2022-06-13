/**
 * @file ScannerCSV.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 19/03/2022
 *
 * @brief Implementación del objeto ScannerCSV
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

#include "logging/debug.hh"

bool ScannerCSV::init() {
    DEBUG_STDOUT("Initializing csv file scanner");

    // Abrimos stream del archivo
    infile.open(filename, std::ifstream::in);
    if (infile.fail()) {
        DEBUG_STDERR("Error while initializing csv file scanner");
        return false;
    }

    DEBUG_STDOUT("Initialized csv file scanner");

    return true;
}

ScanCode ScannerCSV::scan() {
    DEBUG_STDOUT("Starting point scanning");

    if (!scanning) {
        if (!infile.is_open()) {
            infile.open(filename, std::ios::in);
        }
        // Vuelve al principio del archivo
        else if (infile.eof()) {
            infile.seekg(std::ios::beg);
        }

        if (!infile.fail()) {
            scanning = true;
            return readData();
        }
        // Fallo de apertura
        else {
            DEBUG_STDERR("Error while opening csv file");
            return ScanCode::kScanError;
        }

    } else {
        DEBUG_STDERR("Scanner already in use");
        return ScanCode::kScanError;
    }
}

void ScannerCSV::pause() {
    scanning = false;
}

bool ScannerCSV::setCallback(const std::function<void(const LidarPoint &p)> func) {
    DEBUG_STDOUT("Setting up callback");

    callback = func;
    return ((bool)callback);
}

void ScannerCSV::stop() {
    DEBUG_STDOUT("Closing scanner");

    if (infile.is_open()) {
        infile.close();
    }

    DEBUG_STDOUT("Scanner closed");
}

ScanCode ScannerCSV::readData() {
    std::string line;     // String de la linea
    std::string data[5];  // Strings de las celdas

    // Proceso de lectura de puntos
    std::getline(this->infile, line);  // Linea de cabecera

    for (int commas, i; scanning && std::getline(infile, line);) {
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
                this->callback({Timestamp(data[0]), static_cast<uint32_t>(std::stol(data[1])), std::stod(data[2]), std::stod(data[3]), std::stod(data[4])});

            } catch (std::exception &e) {
                return ScanCode::kScanError;
            }
        }
    }

    if (infile.eof()) {
        scanning = false;
        return ScanCode::kScanEof;
    }

    return ScanCode::kScanOk;
}