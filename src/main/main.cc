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

#include "livox_sdk/livox_def.h"

#include "app/App.hh"
#include "app/InputParser.hh"
#include "debug_lbad.hh"

/* Defaults */
#define DEFAULT_TIMER_MODE          kUntimed  // Default timer mode
#define DEFAULT_FRAME_TIME          100       // Default frame time (ms)
#define DEFAULT_BACKGROUND_TIME     500       // Default background time (ms)
#define DEFAULT_MIN_RELECTIVITY     0.0f      // Default point reflectivity
#define DEFAULT_BACKGROUND_DISTANCE 0.2f      // Default backgound distance (m)

/* ParsedInput struct */
struct ParsedInput {
    ParsedInput()
        : is_ok(false),
          time_mode(DEFAULT_TIMER_MODE),
          frame_time(DEFAULT_FRAME_TIME),
          background_time(DEFAULT_BACKGROUND_TIME),
          min_reflectivity(DEFAULT_MIN_RELECTIVITY),
          background_distance(DEFAULT_BACKGROUND_DISTANCE){};

    bool is_ok;     ///< Variable para comprobar si se ha introducido el input necesario
    int exit_code;  ///< Exit code to return when is_ok is false

    bool is_lidar;              ///< Tipo de escanner a usar: true si es mediante lidar
    std::string filename;       ///< Nombre del archivo de datos
    char *broadcast_code;       ///< Codigo de broadcast del sensor lidar
    TimerMode time_mode;        ///< Tipo de métricas a tomar
    uint32_t frame_time;        ///< Tiempo que duraran los puntos en el frame
    uint32_t background_time;   ///< Tiempo en el cual los puntos formarán parte del background
    float min_reflectivity;     ///< Reflectividad mínima que necesitan los puntos para no ser descartados
    float background_distance;  ///< Distancia mínima a la que tiene que estar un punto para no pertenecer al background
};

/* Constants */
std::string exec_name;

/* Declarations */
void help();                            // Command line help
void usage();                           // Command line usage
ParsedInput &missusage(ParsedInput &);  // Treats ParsedInput when command line options are used wrong
ParsedInput parseInput(int, char **);   // Command line input parser

#include "scanner/ScannerCSV.hh"
#include "models/Point.hh"
// Main function
int main(int argc, char *argv[]) {
    
    std::cout << argv[1] << std::endl;
    ScannerLVX s(argv[1]);
    s.init();
    s.setCallback([](const Point &p){std::cout << p.csv_string() << std::endl;});
    s.start();
    while(true);
    return 0;
    
    exec_name.assign(argv[0]);  // Nombre del ejecutable

    ParsedInput pi = parseInput(argc, argv);  // Parse input

    if (pi.is_ok) {
        if (pi.is_lidar) {
            App app(pi.broadcast_code, pi.time_mode, pi.frame_time, pi.background_time, pi.min_reflectivity,
                    pi.background_distance);
        } else {
            App app(pi.filename, pi.time_mode, pi.frame_time, pi.background_time, pi.min_reflectivity,
                    pi.background_distance);
        }
    }

    return EXIT_SUCCESS;  // Exit
}

// Command line parser & App creator
ParsedInput parseInput(int argc, char *argv[]) {
    // Parser de argumentos
    InputParser parser(argc, argv);

    /* App options */
    struct ParsedInput pi;  // Valores por defecto
    pi.frame_time = DEFAULT_FRAME_TIME;
    pi.time_mode = kUntimed;

    /* Impresión de la ayuda */
    if (parser.optionExists("-h") || parser.optionExists("--help")) {
        printDebug("Opción -h | --help");  // debug

        help();  // Imprimimos help
        pi.exit_code = EXIT_SUCCESS;
        return pi;
    }

    /* Input por lidar */
    else if (parser.optionExists("-b")) {
        printDebug("Opción -b");  // debug

        const std::string &option = parser.getOption("-b");
        // No se ha proporcionado valor
        if (option.empty() || option.length() > kBroadcastCodeSize) {
            return missusage(pi);  // Salimos
        }

        pi.is_ok = true;                                         // Input correcto
        pi.is_lidar = true;                                      // Sensor lidar
        pi.broadcast_code = const_cast<char *>(option.c_str());  // Código de broadcast
    }

    /* Input por archivo */
    else if (parser.optionExists("-f")) {
        printDebug("Opción -f");  // debug

        const std::string &option = parser.getOption("-f");
        // No se ha proporcionado valor
        if (option.empty()) {
            return missusage(pi);  // Salimos
        }

        pi.is_ok = true;       // Input correcto
        pi.is_lidar = false;   // Escaner de archivo
        pi.filename = option;  // Filename
    }

    /* No se ha especificado una de las opciones obligatorias */
    else {
        printDebug("No se ha especificado una opción obligatoria");  // debug

        usage();  // Imprimimos help
        pi.exit_code = EXIT_SUCCESS;
        return pi;
    }

    /* Duración del frame */
    if (parser.optionExists("-p")) {
        printDebug("Opción -p");  // debug

        const std::string &option = parser.getOption("-p");
        // No se ha proporcionado valor
        if (option.empty()) {
            return missusage(pi);  // Salimos
        }
        // Obtención del valor
        else {
            // Valor válido
            try {
                pi.frame_time = static_cast<uint32_t>(std::stoul(option));
            }
            // Valor inválido
            catch (std::exception &e) {
                return missusage(pi);  // Salimos
            }
        }
    }

    /* Tipo de cronómetro */
    if (parser.optionExists("-t")) {
        printDebug("Opción -t");  // debug

        const std::string &option = parser.getOption("-t");
        // No se ha proporcionado valor
        if (option.empty()) {
            return missusage(pi);  // Salimos
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
                return missusage(pi);  // Salimos
            }
        }
    }

    /* Duración del background */
    if (parser.optionExists("-g")) {
        printDebug("Opción -g");  // debug

        const std::string &option = parser.getOption("-g");
        // No se ha proporcionado valor
        if (option.empty()) {
            return missusage(pi);  // Salimos
        }
        // Obtención del valor
        else {
            // Valor válido
            try {
                pi.background_time = static_cast<uint32_t>(std::stoul(option));
            }
            // Valor inválido
            catch (std::exception &e) {
                return missusage(pi);  // Salimos
            }
        }
    }

    /* Reflectividad mínima */
    if (parser.optionExists("-r")) {
        printDebug("Opción -r");  // debug

        const std::string &option = parser.getOption("-r");
        // No se ha proporcionado valor
        if (option.empty()) {
            return missusage(pi);  // Salimos
        }
        // Obtención del valor
        else {
            // Valor válido
            try {
                pi.min_reflectivity = std::stof(option);
            }
            // Valor inválido
            catch (std::exception &e) {
                return missusage(pi);  // Salimos
            }
        }
    }

    /* Distancia al background mínima */
    if (parser.optionExists("-d")) {
        printDebug("Opción -d");  // debug

        const std::string &option = parser.getOption("-d");
        // No se ha proporcionado valor
        if (option.empty()) {
            return missusage(pi);  // Salimos
        }
        // Obtención del valor
        else {
            // Valor válido
            try {
                pi.background_distance = std::stof(option);
            }
            // Valor inválido
            catch (std::exception &e) {
                return missusage(pi);  // Salimos
            }
        }
    }

    return pi;
}

// Command line usage
void usage() {
    std::cout << std::endl
              << "Usage:" << std::endl
              << exec_name << " <-b broadcast_code> [-p duration] [-t mode] [-g time] [-r reflectivity] [-d distance]"
              << std::endl
              << exec_name << " <-f filename> [-p duration] [-t mode] [-g time] [-r reflectivity] [-d distance]"
              << std::endl
              << exec_name << " <-h | --help>" << std::endl
              << std::endl;
}

// Command line help
void help() {
    usage();  // Imprimimos usage
    std::cout
        << "\t -b                Broadcast code of the lidar sensor composed of " << kBroadcastCodeSize
        << " digits maximum" << std::endl
        << "\t -f                File with the 3D points to get the data from" << std::endl
        << "\t -p                Amount of miliseconds to use as frame duration time. Default: " << DEFAULT_FRAME_TIME
        << std::endl
        << "\t -t                Type of chronometer to set up and measure time from. Default: notime" << std::endl
        << "\t                       notime - No chrono set" << std::endl
        << "\t                       char   - Characterizator chrono set" << std::endl
        << "\t                       anom   - Anomaly detector chrono set" << std::endl
        << "\t                       all    - All chronos set" << std::endl
        << "\t -g                Miliseconds during which scanned points will be part of the background. Default: "
        << DEFAULT_BACKGROUND_TIME << std::endl
        << "\t -r                Minimum reflectivity value points may have not to be discarded. Default: "
        << DEFAULT_MIN_RELECTIVITY << std::endl
        << "\t -d                Minimum distance from the background in meters a point must have not to be discarded. "
           "Default: "
        << DEFAULT_BACKGROUND_DISTANCE << std::endl
        << "\t -h,--help         Print the program help text" << std::endl
        << std::endl;
}

// Exit when command line options are used wrong
ParsedInput &missusage(ParsedInput &pi) {
    printDebug("Uso incorrecto de la linea de comandos");  // debug

    usage();

    pi.is_ok = false;             // No seguir ejecutando
    pi.exit_code = EXIT_FAILURE;  // Mal input

    return pi;
}
