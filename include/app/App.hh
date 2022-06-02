/**
 * @file App.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 20/03/2022
 *
 * Clase de gestión de la aplicación
 *
 */

#ifndef APP_CLASS_H
#define APP_CLASS_H

#include <stdint.h>
#include <string>

#include "scanner/IScanner.hh"
#include "scanner/ScannerCSV.hh"
#include "scanner/ScannerLVX.hh"
#include "scanner/ScannerLidar.hh"
#include "object_characterization/ObjectCharacterizator.hh"
#include "anomaly_detection/AnomalyDetector.hh"
#include "app/ObjectManager.hh"
#include "app/CLICommand.hh"

#include "logging/debug.hh"

/** Tipos de mediciones de tiempo a tomar */
enum ChronoMode {
    kNoChrono = 0,                   ///< Ejecución sin medida de tiempo
    kChronoCharacterization = 0b01,  ///< Ejecución con medida de tiempo en la caracterización de objetos
    kChronoAnomalyDetection = 0b10,  ///< Ejecución con medida de tiempo en la detección de anomalías
    kChronoAll = 0b11,               ///< Ejecución con medida de tiempo en todo el programa
};

class App {
   private:
    ObjectCharacterizator *oc;  ///< Caracterizador de objetos
    ObjectManager *om;          ///< Gestor de objetos y modelos
    AnomalyDetector *ad;        ///< Detector de anomalías

   public:
    /**
     * Constructor de la app para input de archivo
     * @param filename Nombre de la ruta completa o relativa al archivo de datos
     * @param chronoMode Tipo de mediciones de tiempo a tomar
     * @param objFrame Milisegundos que debe durar un frame en el caracterizador de objetos
     * @param backFrame Milisegundos en los que los puntos tomados formarán parte del background
     * @param minReflectivity Reflectividad mínima que necesitan los puntos para no ser descartados
     * @param backDistance Distancia mínima a la que tiene que estar un punto para no pertenecer al background
     */
    App(const std::string &filename, ChronoMode chronoMode, uint32_t objFrame, uint32_t backFrame, float minReflectivity, float backDistance) {
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
            CLI_STDERR("No se dispone de un escaner para leer el tipo de archivo especificado.");
            return;
        }

        oc = new ObjectCharacterizator(scanner, objFrame, backFrame, minReflectivity, backDistance, chronoMode & kChronoCharacterization);
        om = new ObjectManager();
        ad = new AnomalyDetector(chronoMode & kChronoAnomalyDetection);

        execution();
    }
    /**
     * Constructor de la app para input de sensor lidar
     * @param broadcastCode Codigo de broadcast del sensor lidar
     * @param filename Nombre de la ruta completa o relativa al archivo de datos
     * @param chronoMode Tipo de mediciones de tiempo a tomar
     * @param backFrame Milisegundos en los que los puntos tomados formarán parte del background
     * @param minReflectivity Reflectividad mínima que necesitan los puntos para no ser descartados
     * @param backDistance Distancia mínima a la que tiene que estar un punto para no pertenecer al background
     */
    App(const char *broadcastCode, ChronoMode chronoMode, uint32_t objFrame, uint32_t backFrame, float minReflectivity, float backDistance) {
        IScanner *scanner = ScannerLidar::create(broadcastCode);
        oc = new ObjectCharacterizator(scanner, objFrame, backFrame, minReflectivity, backDistance, chronoMode & kChronoCharacterization);
        om = new ObjectManager();
        ad = new AnomalyDetector(chronoMode & kChronoAnomalyDetection);

        execution();
    }
    /**
     * Destructor de la app
     */
    ~App() {
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
     */
    void init();

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
        init();   // Inicializamos componentes
        cli();    // Iniciamos CLI
        close();  // Cerramos componentes
    }
};

#endif  // APP_CLASS_H