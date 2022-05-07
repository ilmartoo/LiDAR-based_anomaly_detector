/**
 * @file main.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 19/03/2022
 *
 * Función main del programa
 *
 */

#include <iostream>
#include <string>
#include <stdlib.h>

#include "livox_def.h"

#include "app/App.hh"
#include "app/InputParser.hh"

#include "logging/debug.hh"

/* Defaults */
#define DEFAULT_BROADCAST_CODE      "3WEDH7600101621"  // Broadcast code of the Livox Horizon scanner
#define DEFAULT_TIMER_MODE          kNoChrono          // Default timer mode
#define DEFAULT_OBJECT_FRAME_T      100                // Default frame time (ms)
#define DEFAULT_BACKGROUND_FRAME_T  1000               // Default background time (ms)
#define DEFAULT_MIN_RELECTIVITY     0.0f               // Default point reflectivity
#define DEFAULT_BACKGROUND_DISTANCE 0.2f               // Default backgound distance (m)
#define DEFAULT_ITERATIVE_MODE      false              // Default iterative mode indicator

/* InputParams struct */
struct InputParams {
    InputParams(char *argv0)
        : exec_name(argv0),
          is_ok(false),
          exit_code(EXIT_SUCCESS),
          chrono_mode(DEFAULT_TIMER_MODE),
          obj_frame_t(DEFAULT_OBJECT_FRAME_T),
          back_frame_t(DEFAULT_BACKGROUND_FRAME_T),
          min_reflectivity(DEFAULT_MIN_RELECTIVITY),
          back_distance(DEFAULT_BACKGROUND_DISTANCE){};

    void help() const;         // Command line help
    void usage() const;        // Command line usage
    void missusage();          // Treats InputParams when command line options are used wrong
    void parse(int, char **);  // Command line input parser

    std::string exec_name;  // Nombre del ejecutable

    bool is_ok;     // Variable para comprobar si se ha introducido el input necesario
    int exit_code;  // Exit code to return when is_ok is false

    bool is_lidar;           // Tipo de escanner a usar: true si es mediante lidar
    std::string filename;    // Nombre del archivo de datos
    std::string lidar_code;  // Codigo de broadcast del sensor lidar
    ChronoMode chrono_mode;  // Tipo de métricas a tomar
    uint32_t obj_frame_t;    // Tiempo que duraran los puntos en el frame
    uint32_t back_frame_t;   // Tiempo en el cual los puntos formarán parte del background
    float min_reflectivity;  // Reflectividad mínima que necesitan los puntos para no ser descartados
    float back_distance;     // Distancia mínima a la que tiene que estar un punto para no pertenecer al background
};

/* Utilities */
// Checks if string is alphanumeric
inline bool is_alphanumeric(std::string s) {
    for (const char &c : s) {
        if (!isalnum(c)) {
            return false;
        }
    }
    return true;
}

// Main function
int main(int argc, char *argv[]) {
    struct InputParams pi = {argv[0]};  // Generate struct
    pi.parse(argc, argv);               // Parse input

    if (pi.is_ok) {
        if (pi.is_lidar) {
            App app(pi.lidar_code.c_str(), pi.chrono_mode, pi.obj_frame_t, pi.back_frame_t, pi.min_reflectivity, pi.back_distance);
        } else {
            App app(pi.filename, pi.chrono_mode, pi.obj_frame_t, pi.back_frame_t, pi.min_reflectivity, pi.back_distance);
        }
    }

    return pi.exit_code;  // Exit
}

// Command line parser & App creator
void InputParams::parse(int argc, char **argv) {
    // Parser de argumentos
    InputParser parser(argc, argv);

    /* Impresión de la ayuda */
    if (parser.optionExists("-h") || parser.optionExists("--help")) {
        DEBUG_STDOUT("Opción [-h | --help ]: Impresión del mensaje de ayuda.");

        help();  // Imprimimos help
        exit_code = EXIT_SUCCESS;
        return;  // Salimos
    }

    /* Input por lidar */
    else if (parser.optionExists("-b")) {
        DEBUG_STDOUT("Opción [-b]: Sensor LiDAR.");

        std::string &option = const_cast<std::string &>(parser.getOption("-b"));
        // No se ha proporcionado valor
        if (option.empty()) {
            missusage();
            return;  // Salimos
        }
        // Valor por defecto
        else if (option.compare("default") == 0) {
            option.clear();
            option.append(DEFAULT_BROADCAST_CODE);
        }
        // Valor inválido
        else if (option.length() > kBroadcastCodeSize || !is_alphanumeric(option)) {
            missusage();
            return;  // Salimos
        }

        is_ok = true;         // Input correcto
        is_lidar = true;      // Sensor lidar
        lidar_code = option;  // Código de broadcast

        DEBUG_STDOUT("[-b] " << option);
    }

    /* Input por archivo */
    else if (parser.optionExists("-f")) {
        DEBUG_STDOUT("Opción [-f]: Archivo de datos.");

        const std::string &option = parser.getOption("-f");
        // No se ha proporcionado valor
        if (option.empty()) {
            missusage();
            return;  // Salimos
        }

        is_ok = true;       // Input correcto
        is_lidar = false;   // Escaner de archivo
        filename = option;  // Filename

        DEBUG_STDOUT("[-f] " << option);
    }

    /* No se ha especificado una de las opciones obligatorias */
    else {
        DEBUG_STDOUT("No se ha especificado una opción obligatoria.");

        usage();  // Imprimimos help
        exit_code = EXIT_SUCCESS;
        return;  // Salimos
    }

    /* Duración del frame */
    if (parser.optionExists("-t")) {
        DEBUG_STDOUT("Opción [-t]: Duración del frame.");

        const std::string &option = parser.getOption("-t");
        // No se ha proporcionado valor
        if (option.empty()) {
            missusage();
            return;  // Salimos
        }
        // Obtención del valor
        else {
            // Valor válido
            try {
                obj_frame_t = static_cast<uint32_t>(std::stoul(option));

                DEBUG_STDOUT("[-t] " << option);
            }
            // Valor inválido
            catch (std::exception &e) {
                missusage();
                return;  // Salimos
            }
        }
    }

    /* Tipo de cronómetraje */
    if (parser.optionExists("-c")) {
        DEBUG_STDOUT("Opción [-c]: Tipo de cronometraje.");

        const std::string &option = parser.getOption("-c");
        // No se ha proporcionado valor
        if (option.empty()) {
            missusage();
            return;  // Salimos
        }
        // Obtención del valor
        else {
            // notimer
            if (option.compare("notime") == 0) {
                // Opción por defecto
            }
            // char
            else if (option.compare("char") == 0) {
                chrono_mode = kChronoCharacterization;
            }
            // anom
            else if (option.compare("anom") == 0) {
                chrono_mode = kChronoAnomalyDetection;
            }
            // all
            else if (option.compare("all") == 0) {
                chrono_mode = kChronoAll;
            }
            // Valor inválido
            else {
                missusage();
                return;  // Salimos
            }

            DEBUG_STDOUT("[-c] " << option);
        }
    }

    /* Duración del background */
    if (parser.optionExists("-g")) {
        DEBUG_STDOUT("Opción [-g]: Duración del escaneo del fondo.");

        const std::string &option = parser.getOption("-g");
        // No se ha proporcionado valor
        if (option.empty()) {
            missusage();
            return;  // Salimos
        }
        // Obtención del valor
        else {
            // Valor válido
            try {
                back_frame_t = static_cast<uint32_t>(std::stoul(option));

                DEBUG_STDOUT("[-g] " << option);
            }
            // Valor inválido
            catch (std::exception &e) {
                missusage();
                return;  // Salimos
            }
        }
    }

    /* Reflectividad mínima */
    if (parser.optionExists("-r")) {
        DEBUG_STDOUT("Opción [-r]: Reflectividad mínima.");

        const std::string &option = parser.getOption("-r");
        // No se ha proporcionado valor
        if (option.empty()) {
            missusage();
            return;  // Salimos
        }
        // Obtención del valor
        else {
            // Valor válido
            try {
                min_reflectivity = std::stof(option);

                DEBUG_STDOUT("[-r] " << option);
            }
            // Valor inválido
            catch (std::exception &e) {
                missusage();
                return;  // Salimos
            }
        }
    }

    /* Distancia al background mínima */
    if (parser.optionExists("-d")) {
        DEBUG_STDOUT("Opción [-d]: Distancia mínima del fondo.");

        const std::string &option = parser.getOption("-d");
        // No se ha proporcionado valor
        if (option.empty()) {
            missusage();
            return;  // Salimos
        }
        // Obtención del valor
        else {
            // Valor válido
            try {
                back_distance = std::stof(option);

                DEBUG_STDOUT("[-d] " << option);
            }
            // Valor inválido
            catch (std::exception &e) {
                missusage();
                return;  // Salimos
            }
        }
    }
}

// Command line usage
void InputParams::usage() const {
    std::cout << std::endl
              << "Usage:" << std::endl
              << exec_name << " <-b lidar_code> [-t obj_frame_t] [-c chrono_mode] [-g back_frame_t] [-r reflectivity_threshold] [-d distance_threshold]" << std::endl
              << exec_name << " <-f filename> [-t obj_frame_t] [-c chrono_mode] [-g back_frame_t] [-r reflectivity_threshold] [-d distance_threshold]" << std::endl
              << exec_name << " <-h | --help>" << std::endl
              << std::endl;
}

// Command line help
void InputParams::help() const {
    usage();  // Imprimimos usage
    std::cout << "\t -b                Broadcast code of the lidar sensor composed of " << kBroadcastCodeSize - 1 << " symbols maximum. With 'default' as value it defaults to " << DEFAULT_BROADCAST_CODE << std::endl
              << "\t -f                File with the 3D points to get the data from" << std::endl
              << "\t -t                Miliseconds to use as frame duration time. Defaults to " << DEFAULT_OBJECT_FRAME_T << "ms" << std::endl
              << "\t -c                Type of chronometer to set up and measure time from. Defaults to notime" << std::endl
              << "\t                       notime - No chrono set" << std::endl
              << "\t                       char   - Characterizator chrono set" << std::endl
              << "\t                       anom   - Anomaly detector chrono set" << std::endl
              << "\t                       all    - All chronos set" << std::endl
              << "\t -g                Miliseconds during which scanned points will be part of the background. Defaults to " << DEFAULT_BACKGROUND_FRAME_T << "ms" << std::endl
              << "\t -r                Minimum reflectivity value points may have not to be discarded. Defaults to " << DEFAULT_MIN_RELECTIVITY << std::endl
              << "\t -d                Minimum distance from the background in meters a point must have not to be discarded. Defaults to " << DEFAULT_BACKGROUND_DISTANCE << "m" << std::endl
              << "\t -h,--help         Print the program help text" << std::endl
              << std::endl
              << "Undefined options will be ignored." << std::endl
              << std::endl;
}

// Exit when command line options are used wrong
void InputParams::missusage() {
    DEBUG_STDOUT("Uso incorrecto de la linea de comandos");

    usage();

    is_ok = false;             // No seguir ejecutando
    exit_code = EXIT_FAILURE;  // Mal input
}
