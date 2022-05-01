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

#include "logging/debug.hh"

/* Defaults */
#define DEFAULT_BROADCAST_CODE      "3WEDH7600101621"  // Broadcast code of the Livox Horizon scanner
#define DEFAULT_TIMER_MODE          kUntimed           // Default timer mode
#define DEFAULT_FRAME_TIME          100                // Default frame time (ms)
#define DEFAULT_BACKGROUND_TIME     500                // Default background time (ms)
#define DEFAULT_MIN_RELECTIVITY     0.0f               // Default point reflectivity
#define DEFAULT_BACKGROUND_DISTANCE 0.2f               // Default backgound distance (m)
#define DEFAULT_ITERATIVE_MODE      false              // Default iterative mode indicator

/* InputParams struct */
struct InputParams {
    InputParams(char *argv0)
        : exec_name(argv0),
          is_ok(false),
          exit_code(EXIT_SUCCESS),
          iterative_mode(DEFAULT_ITERATIVE_MODE),
          time_mode(DEFAULT_TIMER_MODE),
          frame_time(DEFAULT_FRAME_TIME),
          background_time(DEFAULT_BACKGROUND_TIME),
          min_reflectivity(DEFAULT_MIN_RELECTIVITY),
          background_distance(DEFAULT_BACKGROUND_DISTANCE){};

    void help() const;         // Command line help
    void usage() const;        // Command line usage
    void missusage();          // Treats InputParams when command line options are used wrong
    void parse(int, char **);  // Command line input parser

    std::string exec_name;  // Nombre del ejecutable

    bool is_ok;     // Variable para comprobar si se ha introducido el input necesario
    int exit_code;  // Exit code to return when is_ok is false

    bool is_lidar;               // Tipo de escanner a usar: true si es mediante lidar
    bool iterative_mode;         // Indicador de ejecución en modo iterativo
    std::string filename;        // Nombre del archivo de datos
    std::string broadcast_code;  // Codigo de broadcast del sensor lidar
    TimerMode time_mode;         // Tipo de métricas a tomar
    uint32_t frame_time;         // Tiempo que duraran los puntos en el frame
    uint32_t background_time;    // Tiempo en el cual los puntos formarán parte del background
    float min_reflectivity;      // Reflectividad mínima que necesitan los puntos para no ser descartados
    float background_distance;   // Distancia mínima a la que tiene que estar un punto para no pertenecer al background
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
            App app(pi.broadcast_code.c_str(), pi.time_mode, pi.frame_time, pi.background_time, pi.min_reflectivity, pi.background_distance,
                    pi.iterative_mode);
        } else {
            App app(pi.filename, pi.time_mode, pi.frame_time, pi.background_time, pi.min_reflectivity, pi.background_distance,
                    pi.iterative_mode);
        }
    }

    return pi.exit_code;  // Exit
}

// Command line parser & App creator
void InputParams::parse(int argc, char **argv) {
    // Parser de argumentos
    InputParser parser(argc, argv);

    /* App options */
    this->frame_time = DEFAULT_FRAME_TIME;
    this->time_mode = kUntimed;

    /* Impresión de la ayuda */
    if (parser.optionExists("-h") || parser.optionExists("--help")) {
        DEBUG_STDOUT("Opción [-h | --help ]: Impresión del mensaje de ayuda.");

        this->help();  // Imprimimos help
        this->exit_code = EXIT_SUCCESS;
        return;  // Salimos
    }

    /* Input por lidar */
    else if (parser.optionExists("-b")) {
        DEBUG_STDOUT("Opción [-b]: Sensor LiDAR.");

        std::string &option = const_cast<std::string &>(parser.getOption("-b"));
        // No se ha proporcionado valor
        if (option.empty()) {
            this->missusage();
            return;  // Salimos
        }
        // Valor por defecto
        else if (option.compare("default") == 0) {
            option.clear();
            option.append(DEFAULT_BROADCAST_CODE);
        }
        // Valor inválido
        else if (option.length() > kBroadcastCodeSize || !is_alphanumeric(option)) {
            this->missusage();
            return;  // Salimos
        }

        this->is_ok = true;             // Input correcto
        this->is_lidar = true;          // Sensor lidar
        this->broadcast_code = option;  // Código de broadcast

        DEBUG_STDOUT("[-b] " + option);
    }

    /* Input por archivo */
    else if (parser.optionExists("-f")) {
        DEBUG_STDOUT("Opción [-f]: Archivo de datos.");

        const std::string &option = parser.getOption("-f");
        // No se ha proporcionado valor
        if (option.empty()) {
            this->missusage();
            return;  // Salimos
        }

        this->is_ok = true;       // Input correcto
        this->is_lidar = false;   // Escaner de archivo
        this->filename = option;  // Filename

        DEBUG_STDOUT("[-f] " + option);
    }

    /* No se ha especificado una de las opciones obligatorias */
    else {
        DEBUG_STDOUT("No se ha especificado una opción obligatoria.");

        this->usage();  // Imprimimos help
        this->exit_code = EXIT_SUCCESS;
        return;  // Salimos
    }

    /* Duración del frame */
    if (parser.optionExists("-t")) {
        DEBUG_STDOUT("Opción [-t]: Duración del frame.");

        const std::string &option = parser.getOption("-t");
        // No se ha proporcionado valor
        if (option.empty()) {
            this->missusage();
            return;  // Salimos
        }
        // Obtención del valor
        else {
            // Valor válido
            try {
                this->frame_time = static_cast<uint32_t>(std::stoul(option));

                DEBUG_STDOUT("[-t] " + option);
            }
            // Valor inválido
            catch (std::exception &e) {
                this->missusage();
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
            this->missusage();
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
                this->time_mode = kTimedCharacterization;
            }
            // anom
            else if (option.compare("anom") == 0) {
                this->time_mode = kTimedAnomalyDetection;
            }
            // all
            else if (option.compare("all") == 0) {
                this->time_mode = kTimed;
            }
            // Valor inválido
            else {
                this->missusage();
                return;  // Salimos
            }

            DEBUG_STDOUT("[-c] " + option);
        }
    }

    /* Duración del background */
    if (parser.optionExists("-g")) {
        DEBUG_STDOUT("Opción [-g]: Duración del escaneo del fondo.");

        const std::string &option = parser.getOption("-g");
        // No se ha proporcionado valor
        if (option.empty()) {
            this->missusage();
            return;  // Salimos
        }
        // Obtención del valor
        else {
            // Valor válido
            try {
                this->background_time = static_cast<uint32_t>(std::stoul(option));

                DEBUG_STDOUT("[-g] " + option);
            }
            // Valor inválido
            catch (std::exception &e) {
                this->missusage();
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
            this->missusage();
            return;  // Salimos
        }
        // Obtención del valor
        else {
            // Valor válido
            try {
                this->min_reflectivity = std::stof(option);

                DEBUG_STDOUT("[-r] " + option);
            }
            // Valor inválido
            catch (std::exception &e) {
                this->missusage();
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
            this->missusage();
            return;  // Salimos
        }
        // Obtención del valor
        else {
            // Valor válido
            try {
                this->background_distance = std::stof(option);

                DEBUG_STDOUT("[-d] " + option);
            }
            // Valor inválido
            catch (std::exception &e) {
                this->missusage();
                return;  // Salimos
            }
        }
    }

    /* Modo iterativo */
    if (parser.optionExists("-i")) {
        DEBUG_STDOUT("Opción [-i]: Modo iterativo.");

        this->iterative_mode = true;
    }
}

// Command line usage
void InputParams::usage() const {
    std::cout
        << std::endl
        << "Usage:" << std::endl
        << this->exec_name
        << " <-b broadcast_code> [-t frame_time] [-c chrono_mode] [-g background_time] [-r reflectivity_threshold] [-d distance_threshold] "
           "[-i]"
        << std::endl
        << this->exec_name
        << " <-f filename> [-t frame_time] [-c chrono_mode] [-g background_time] [-r reflectivity_threshold] [-d distance_threshold] [-i]"
        << std::endl
        << this->exec_name << " <-h | --help>" << std::endl
        << std::endl;
}

// Command line help
void InputParams::help() const {
    this->usage();  // Imprimimos usage
    std::cout << "\t -b                Broadcast code of the lidar sensor composed of " << kBroadcastCodeSize - 1
              << " symbols maximum. With 'default' as value it defaults to " << DEFAULT_BROADCAST_CODE << std::endl
              << "\t -f                File with the 3D points to get the data from" << std::endl
              << "\t -t                Miliseconds to use as frame duration time. Defaults to " << DEFAULT_FRAME_TIME << std::endl
              << "\t -c                Type of chronometer to set up and measure time from. Defaults to notime" << std::endl
              << "\t                       notime - No chrono set" << std::endl
              << "\t                       char   - Characterizator chrono set" << std::endl
              << "\t                       anom   - Anomaly detector chrono set" << std::endl
              << "\t                       all    - All chronos set" << std::endl
              << "\t -g                Miliseconds during which scanned points will be part of the background. Defaults to "
              << DEFAULT_BACKGROUND_TIME << std::endl
              << "\t -r                Minimum reflectivity value points may have not to be discarded. Defaults to "
              << DEFAULT_MIN_RELECTIVITY << std::endl
              << "\t -d                Minimum distance from the background in meters a point must have not to be discarded. "
                 "Defaults to "
              << DEFAULT_BACKGROUND_DISTANCE << std::endl
              << "\t -i                Activates the iterative execution mode. It is disabled by default." << std::endl
              << "\t -h,--help         Print the program help text" << std::endl
              << std::endl
              << "Undefined options will be ignored." << std::endl
              << std::endl;
}

// Exit when command line options are used wrong
void InputParams::missusage() {
    DEBUG_STDOUT("Uso incorrecto de la linea de comandos");

    this->usage();

    this->is_ok = false;             // No seguir ejecutando
    this->exit_code = EXIT_FAILURE;  // Mal input
}
