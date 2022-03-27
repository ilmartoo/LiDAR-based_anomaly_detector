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
#include "debug_lbad.hh"

#define DEFAULT_FRAME_TIME 100  // Default frame time

void help();       // Command line help
void usage();      // Command line usage
void missusage();  // Exit when command line options are used wrong

// Main function
int main(int argc, char *argv[]) {
    // Parser de argumentos
    InputParser parser(argc, argv);

    /* Impresión de la ayuda */
    if (parser.optionExists("-h") || parser.optionExists("--help")) {
        printDebug("Opción -h | --help");  // debug

        help();               // Imprimimos help
        return EXIT_SUCCESS;  // Salimos
    }

    /* App options */
    TimerMode tm = kUntimed;
    uint32_t ft = DEFAULT_FRAME_TIME;

    /* Duración del frame */
    if (parser.optionExists("-d")) {
        printDebug("Opción -d");  // debug

        const std::string &option = parser.getOption("-d");
        // No se ha proporcionado valor
        if (option.empty()) {
            missusage();  // Salimos
        }
        // Obtención del valor
        else {
            ft = static_cast<uint32_t>(std::stoul(option));
        }
    }

    /* Tipo de cronómetro */
    if (parser.optionExists("-t")) {
        printDebug("Opción -t");  // debug

        const std::string &option = parser.getOption("-t");
        std::cout << "-- " << option << " --" << std::endl;
        // No se ha proporcionado valor
        if (option.empty()) {
            missusage();  // Salimos
        }
        // Obtención del valor
        else {
            // notimer
            if (option.compare("notime") == 0) {
                // Opción por defecto
            }
            // char
            else if (option.compare("char") == 0) {
                tm = kTimedCharacterization;
            }
            // anom
            else if (option.compare("anom") == 0) {
                tm = kTimedAnomalyDetection;
            }
            // all
            else if (option.compare("all") == 0) {
                tm = kTimed;
            }
            // Valor inválido
            else {
                missusage();  // Salimos
            }
        }
    }

    /* Input por lidar */
    if (parser.optionExists("-b")) {
        printDebug("Opción -b");  // debug

    }

    /* Input por archivo */
    else if (parser.optionExists("-f")) {
        printDebug("Opción -f");  // debug

    }

    /* No se ha especificado una de las opciones obligatorias */
    else {
        printDebug("No se ha especificado una opción obligatoria");  // debug

        help();  // Imprimimos help
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}

// Command line usage
void usage() {
    std::cout << std::endl
              << "Usage:" << std::endl
              << " lbad <-b broadcast_code> [-d frame_time] [-t time_mode]" << std::endl
              << " lbad <-f filename> [-d frame_time] [-t time_mode]" << std::endl
              << " lbad <-h | --help>" << std::endl
              << std::endl;
}

// Command line help
void help() {
    usage();  // Imprimimos usage
    std::cout << "\t -b                Broadcast code of the lidar sensor" << std::endl
              << "\t -f                File with the 3D points to get the data from" << std::endl
              << "\t -d                Amount of miliseconds to use as frame duration time (default is 100)"
              << std::endl
              << "\t -t                Type of chronometer to set up and measure time from (default is notime)"
              << std::endl
              << "\t                       notime - No chrono set" << std::endl
              << "\t                       char   - Characterizator chrono set" << std::endl
              << "\t                       anom   - Anomaly detector chrono set" << std::endl
              << "\t                       all    - All chronos set" << std::endl
              << "\t -h, --help        Print the program help text" << std::endl
              << std::endl;
}

// Exit when command line options are used wrong
void missusage() {
    printDebug("Uso incorrecto de la linea de comandos");  // debug

    usage();
    exit(EXIT_FAILURE);
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
