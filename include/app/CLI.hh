/**
 * @file CLI.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 20/03/2022
 *
 * @brief Interfaz de línea de comandos del programa
 *
 */

#ifndef CLI_CLASS_H
#define CLI_CLASS_H

#include <stdint.h>
#include <string>

#include "scanner/IScanner.hh"
#include "scanner/ScannerCSV.hh"
#include "scanner/ScannerLVX.hh"
#include "scanner/ScannerLidar.hh"
#include "object_characterization/ObjectCharacterizer.hh"
#include "anomaly_detection/AnomalyDetector.hh"
#include "app/ObjectManager.hh"

#include "logging/debug.hh"
#include "app/config.h"

#define CLI_STDOUT(msg) do { std::cout << cyan("" << msg << "") << std::endl; } while(0)
#define CLI_STDOUT_NO_NL(msg) do { std::cout << cyan("" << msg << ""); } while(0)
#define CLI_STDERR(msg) do { std::cerr << red("" << msg << "") << std::endl; } while(0)
#define CLI_STDERR_NO_NL(msg) do { std::cerr << red("" << msg << ""); } while(0)

/**
 * @brief Director de la ejecución del programa e interfaz de comunicación entre usuario y el resto de módulos
 */
class CLI {
   private:
    ObjectCharacterizer *oc;  ///< Caracterizador de objetos
    ObjectManager *om;          ///< Gestor de objetos y modelos
    AnomalyDetector *ad;        ///< Detector de anomalías

   public:
    /**
     * Constructor de la CLI para input de archivo
     * @param filename Nombre de la ruta completa o relativa al archivo de datos
     * @param chronoMode Tipo de mediciones de tiempo a tomar
     * @param objFrame Milisegundos que debe durar un frame en el caracterizador de objetos
     * @param backFrame Milisegundos en los que los puntos tomados formarán parte del background
     * @param minReflectivity Reflectividad mínima que necesitan los puntos para no ser descartados
     * @param backDistance Distancia mínima a la que tiene que estar un punto para no pertenecer al background
     */
    CLI(const std::string &filename, ChronoMode chronoMode, uint32_t objFrame, uint32_t backFrame, float minReflectivity, float backDistance) {
        IScanner *scanner;

        // Obtenemos extensión del archivo
        size_t loc = filename.find_last_of('.');
        std::string ext = filename.substr(++loc);

        // Lectura de un archivo LVX
        if (ext.compare("lvx") == 0) {
            scanner = ScannerLVX::create(filename);
        }
        // Lectura de un archivo CSV
        else if (ext.compare("csv") == 0) {
            scanner = ScannerCSV::create(filename);
        }
        // Archivo no legible
        else {
            CLI_STDERR("File format could not be read with any of the current scanners");
            return;
        }

        oc = new ObjectCharacterizer(scanner, objFrame, backFrame, minReflectivity, backDistance, chronoMode & kChronoCharacterization);
        om = new ObjectManager();
        ad = new AnomalyDetector(chronoMode & kChronoAnomalyDetection);

        execution();
    }
    /**
     * Constructor de la CLI para input de sensor lidar
     * @param broadcastCode Codigo de broadcast del sensor lidar
     * @param filename Nombre de la ruta completa o relativa al archivo de datos
     * @param chronoMode Tipo de mediciones de tiempo a tomar
     * @param backFrame Milisegundos en los que los puntos tomados formarán parte del background
     * @param minReflectivity Reflectividad mínima que necesitan los puntos para no ser descartados
     * @param backDistance Distancia mínima a la que tiene que estar un punto para no pertenecer al background
     */
    CLI(const char *broadcastCode, ChronoMode chronoMode, uint32_t objFrame, uint32_t backFrame, float minReflectivity, float backDistance) {
        IScanner *scanner = ScannerLidar::create(broadcastCode);
        oc = new ObjectCharacterizer(scanner, objFrame, backFrame, minReflectivity, backDistance, chronoMode & kChronoCharacterization);
        om = new ObjectManager();
        ad = new AnomalyDetector(chronoMode & kChronoAnomalyDetection);

        execution();
    }
    /**
     * Destructor de la CLI
     */
    ~CLI() {
        if (ad != nullptr) {
            delete ad;
        }
        if (om != nullptr) {
            delete om;
        }
        if (ad != nullptr) {
            delete oc;
        }
    }

   private:
    /**
     * Inicializa la aplicación
     * @return true si la inicialización fue correcta
     */
    bool init();

    /**
     * Command line interface
     */
    void cli();

    /**
     * Finaliza la aplicación
     */
    void close();

    /**
     * Ejecución de las fases de la aplicación
     */
    void execution() {
        if (init()) {   // Inicializamos componentes
            cli();    // Iniciamos CLI
            close();  // Cerramos componentes
        } else {
            CLI_STDERR("Failed to initiate the CLI");
        }
    }
};

#endif  // CLI_CLASS_H