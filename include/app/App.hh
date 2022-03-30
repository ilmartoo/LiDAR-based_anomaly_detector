/**
 * @file App.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 20/03/2022
 *
 * Clase de gestión de la aplicación
 *
 */

#ifndef __APP_CLASS_H
#define __APP_CLASS_H

#include <stdint.h>
#include <string>

#include "scanner/IScanner.hh"
#include "scanner/ScannerFile.hh"
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
     */
    App(const std::string &filename, TimerMode timerMode, uint32_t frameDuration, uint32_t backgroundTime,
        float minReflectivity)
        : timerMode(timerMode), backgroundTime(backgroundTime), minReflectivity(minReflectivity) {
        scanner = new ScannerFile(filename);            // Creamos escaner
        oc = new ObjectCharacterizator(frameDuration);  // Creamos caracterizador
        // ad =

        this->start();
        this->wait();
        this->stop();
    }

    /**
     * Constructor de la app para input de sensor lidar
     * @param broadcast_code Codigo de broadcast del sensor lidar
     * @param filename Nombre de la ruta completa o relativa al archivo de datos
     * @param timerMode Tipo de mediciones de tiempo a tomar
     * @param backgroundTime Milisegundos en los que los puntos tomados formarán parte del background
     * @param minReflectivity Reflectividad mínima que necesitan los puntos para no ser descartados
     */
    App(const char *broadcast_code, TimerMode timerMode, uint32_t frameDuration, uint32_t backgroundTime,
        float minReflectivity)
        : timerMode(timerMode), backgroundTime(backgroundTime), minReflectivity(minReflectivity) {
        scanner = new ScannerLidar(broadcast_code);
        oc = new ObjectCharacterizator(frameDuration);
        // ad =

        this->start();
        this->wait();
        this->stop();
    }

    /**
     * Constructor de la app para input de lidar
     * @param timerMode Tipo de mediciones de tiempo a tomar
     * @param id Id del lidar
     */
    // App(TimerMode timerMode, const std::string &id);

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

    uint32_t backgroundTime;  ///< Tiempo en milisegundos en el cual los puntos tomados formarán parte del background
    float minReflectivity;    ///< Reflectividad mínima que necesitan los puntos para no ser descartados

    IScanner *scanner;           ///< Escaner de puntos
    IObjectCharacterizator *oc;  ///< Caracterizador de objetos
    IAnomalyDetector *ad;        ///< Detector de anomalías
};

#endif  //__APP_CLASS_H