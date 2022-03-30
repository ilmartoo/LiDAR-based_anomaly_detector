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

/* Defaults */
#define DEFAULT_FRAME_TIME 100  // Default frame time

/* ParsedInput struct */
struct ParsedInput {
    ParsedInput() : time_mode(kUntimed), frame_time(DEFAULT_FRAME_TIME){};

    bool is_lidar;         ///< Tipo de escanner a usar: true si es mediante lidar
    std::string filename;  ///< Nombre del archivo de datos
    char *broadcast_code;  ///< Codigo de broadcast del sensor lidar
    TimerMode time_mode;   ///< Tipo de métricas a tomar
    uint32_t frame_time;   ///< Tiempo que duraran los puntos en el frame
};

/* Declarations */
void help();                           // Command line help
void usage();                          // Command line usage
void missusage();                      // Exit when command line options are used wrong
ParsedInput parseInput(int, char **);  // Command line input parser

// Main function
int main(int argc, char *argv[]) {
    ParsedInput pi = parseInput(argc, argv);  // Parse input

    App app(pi.is_lidar ? pi.broadcast_code : pi.filename, pi.time_mode, pi.frame_time);

    return EXIT_SUCCESS;  // Exit
}

// Command line parser & App creator
ParsedInput parseInput(int argc, char *argv[]) {
    // Parser de argumentos
    InputParser parser(argc, argv);

    /* Impresión de la ayuda */
    if (parser.optionExists("-h") || parser.optionExists("--help")) {
        printDebug("Opción -h | --help");  // debug

        help();              // Imprimimos help
        exit(EXIT_SUCCESS);  // Salimos
    }

    /* App options */
    struct ParsedInput pi;  // Valores por defecto
    pi.frame_time = DEFAULT_FRAME_TIME;
    pi.time_mode = kUntimed;

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
            // Valor válido
            try {
                pi.frame_time = static_cast<uint32_t>(std::stoul(option));
            }
            // Valor inválido
            catch (std::exception &e) {
                missusage();  // Salimos
            }
        }
    }

    /* Tipo de cronómetro */
    if (parser.optionExists("-t")) {
        printDebug("Opción -t");  // debug

        const std::string &option = parser.getOption("-t");
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
                pi.time_mode = kTimedCharacterization;
            }
            // anom
            else if (option.compare("anom") == 0) {
                pi.time_mode = kTimedAnomalyDetection;
            }
            // all
            else if (option.compare("all") == 0) {
                pi.time_mode = kTimed;
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

        const std::string &option = parser.getOption("-b");
        // No se ha proporcionado valor
        if (option.empty()) {
            missusage();  // Salimos
        }

        pi.is_lidar = true;
        pi.broadcast_code = const_cast<char *>(option.c_str());
    }

    /* Input por archivo */
    else if (parser.optionExists("-f")) {
        printDebug("Opción -f");  // debug

        const std::string &option = parser.getOption("-f");
        // No se ha proporcionado valor
        if (option.empty()) {
            missusage();  // Salimos
        }

        pi.is_lidar = false;
        pi.filename = option;
    }

    /* No se ha especificado una de las opciones obligatorias */
    else {
        printDebug("No se ha especificado una opción obligatoria");  // debug

        help();  // Imprimimos help
        exit(EXIT_SUCCESS);
    }

    return pi;
}

// Command line usage
void usage() {
    std::cout << std::endl
              << "Usage:" << std::endl
              << " anomaly_detection <-b broadcast_code> [-d frame_time] [-t time_mode]" << std::endl
              << " anomaly_detection <-f filename> [-d frame_time] [-t time_mode]" << std::endl
              << " anomaly_detection <-h | --help>" << std::endl
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
              << "\t -h,--help         Print the program help text" << std::endl
              << std::endl;
}

// Exit when command line options are used wrong
void missusage() {
    printDebug("Uso incorrecto de la linea de comandos");  // debug

    usage();
    exit(EXIT_FAILURE);
}
