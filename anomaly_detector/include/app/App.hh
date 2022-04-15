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
#include "object_characterization/IObjectCharacterizator.hh"
#include "object_characterization/ObjectCharacterizator.hh"
#include "anomaly_detection/IAnomalyDetector.hh"

/** Tipos de input de datos */
enum InputType {
    kInputFile = 0,   ///< Lectura de puntos de un archivo especificado
    kInputLidar = 1,  ///< Lectura de puntos del sensor lidar especificado
};

/** Tipos de mediciones de tiempo a tomar */
enum TimerMode {
    kUntimed = 0,                   ///< Ejecución sin medida de tiempo
    kTimedCharacterization = 0b01,  ///< Ejecución con medida de tiempo en la caracterización de objetos
    kTimedAnomalyDetection = 0b10,  ///< Ejecución con medida de tiempo en la detección de anomalías
    kTimed = 0b11,                  ///< Ejecución con medida de tiempo en todo el programa
};

class App {
   public:
    /**
     * Constructor de la app para input de archivo
     * @param filename Nombre de la ruta completa o relativa al archivo de datos
     * @param timerMode Tipo de mediciones de tiempo a tomar
     * @param frameDuration Milisegundos que debe durar un frame en el caracterizador de objetos
     * @param backgroundTime Milisegundos en los que los puntos tomados formarán parte del background
     * @param minReflectivity Reflectividad mínima que necesitan los puntos para no ser descartados
     * @param backgoundDistance Distancia mínima a la que tiene que estar un punto para no pertenecer al background
     */
    App(const std::string &filename, TimerMode timerMode, uint32_t frameDuration, uint32_t backgroundTime, float minReflectivity,
        float backgroundDistance) {
        this->timerMode = timerMode;              // timerMode

        // Obtenemos extensión del archivo
        size_t loc = filename.find_last_of('.');
        std::string ext = filename.substr(++loc);

        // Lectura de un archivo LVX
        if (ext.compare("lvx") == 0) {
            scanner = new ScannerLVX(filename);  // Creamos escaner
        }
        // Lectura de un archivo CSV por defecto
        else {
            scanner = new ScannerCSV(filename);  // Creamos escaner
        }

        oc = new ObjectCharacterizator(frameDuration, backgroundTime, minReflectivity, backgroundDistance);  // Creamos caracterizador
        // ad =

        this->start();  // Iniciamos app
        this->wait();   // Esperamos a recibir una señal de salida
        this->stop();   // Finalizamos app
    }

    /**
     * Constructor de la app para input de sensor lidar
     * @param broadcastCode Codigo de broadcast del sensor lidar
     * @param filename Nombre de la ruta completa o relativa al archivo de datos
     * @param timerMode Tipo de mediciones de tiempo a tomar
     * @param backgroundTime Milisegundos en los que los puntos tomados formarán parte del background
     * @param minReflectivity Reflectividad mínima que necesitan los puntos para no ser descartados
     * @param backgoundDistance Distancia mínima a la que tiene que estar un punto para no pertenecer al background
     */
    App(const char *broadcastCode, TimerMode timerMode, uint32_t frameDuration, uint32_t backgroundTime, float minReflectivity,
        float backgroundDistance) {
        this->timerMode = timerMode;              // timerMode

        scanner = new ScannerLidar(broadcastCode);                                                           // Creamos escaner
        oc = new ObjectCharacterizator(frameDuration, backgroundTime, minReflectivity, backgroundDistance);  // Creamos caracterizador
        // ad =

        this->start();  // Iniciamos app
        this->wait();   // Esperamos a recibir una señal de salida
        this->stop();   // Finalizamos app
    }

    /**
     * Destructor de la app
     */
    ~App() {
        delete scanner;
        delete oc;
        // delete ad;
    }

   private:
    /**
     * Inicia la aplicación
     */
    void start();

    /**
     * Espera la llegada de una señal SIGINT
     */
    void wait();

    /**
     * Finaliza la aplicación
     */
    void stop();

    enum InputType inputType;  ///< Tipo de input de datos
    enum TimerMode timerMode;  ///< Tipo de mediciones de tiempo a tomar

    IScanner *scanner;           ///< Escaner de puntos
    IObjectCharacterizator *oc;  ///< Caracterizador de objetos
    IAnomalyDetector *ad;        ///< Detector de anomalías
};

#endif  // APP_CLASS_H