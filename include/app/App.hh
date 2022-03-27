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
    kTimedAnomalyDetection = 0b10,   ///< Ejecución con medida de tiempo en la detección de anomalías
    kTimed = 0b11,                  ///< Ejecución con medida de tiempo en todo el programa
};

class App {
   public:
    /**
     * Constructor de la app para input de archivo
     * @param filename Nombre de la ruta completa o relativa al archivo de datos
     * @param timerMode Tipo de mediciones de tiempo a tomar
     * @param frameDuration Milisegundos que debe durar un frame en el caracterizador de objetos
     */
    App(const std::string &filename, TimerMode timerMode, uint32_t frameDuration) : timerMode(timerMode) {
        scanner = new ScannerFile(filename);
        oc = new ObjectCharacterizator(frameDuration);
    }

    /**
     * Constructor de la app para input de sensor lidar
     * @param filename Nombre de la ruta completa o relativa al archivo de datos
     * @param timerMode Tipo de mediciones de tiempo a tomar
     */
    App(const char *broadcast_code, TimerMode timerMode, uint32_t frameDuration) : timerMode(timerMode) {
        scanner = new ScannerLidar(broadcast_code);
        oc = new ObjectCharacterizator(frameDuration);
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
    ~App() {}

    /**
     * Inicia la aplicación
     */
    void start();

    /**
     * Inicia la aplicación
     * @param seconds Tiempo a ejecutar la aplicación
     */
    void start(uint32_t seconds);

    /**
     * Finaliza la aplicación
     */
    void stop();

   private:
    enum InputType inputType;  ///< Tipo de input de datos
    enum TimerMode timerMode;  ///< Tipo de mediciones de tiempo a tomar

    uint32_t endTime;  ///< Tiempo total de ejecución de la aplicación

    IScanner *scanner;           ///< Escaner de puntos
    IObjectCharacterizator *oc;  ///< Caracterizador de objetos
    // IAnomalyDetector &ad;        ///< Detector de anomalías
};

#endif  //__APP_CLASS_H