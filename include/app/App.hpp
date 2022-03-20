/**
 * @file App.hpp
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

#include "scanner/IScanner.hpp"
#include "scanner/ScannerFile.hpp"
#include "object_characterization/IObjectCharacterizator.hpp"
#include "anomaly_detection/IAnomalyDetector.hpp"

/** Tipos de input de datos */
enum InputType {
    fileInput = 0,   ///< Lectura de puntos de un archivo especificado
    lidarInput = 1,  ///< Lectura de puntos del sensor lidar especificado
};

/** Tipos de mediciones de tiempo a tomar */
enum TimerMode {
    untimed = 0,                   ///< Ejecución sin medida de tiempo
    timedCharacterization = 0b01,  ///< Ejecución con medida de tiempo en la caracterización de objetos
    timedAnomalyDetector = 0b10,   ///< Ejecución con medida de tiempo en la detección de anomalías
    timed = 0b11,                  ///< Ejecución con medida de tiempo en todo el programa
};

class App {
   public:
    /**
     * Constructor de la app para input de archivo
     * @param timerMode Tipo de mediciones de tiempo a tomar
     * @param filename Nombre de la ruta completa o relativa al archivo de datos
     */
    App(TimerMode timerMode, const std::string &filename);

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

    // IScanner &scanner;           ///< Escaner de puntos
    // IObjectCharacterizator &oc;  ///< Caracterizador de objetos
    // IAnomalyDetector &ad;        ///< Detector de anomalías
};

#endif  //__APP_CLASS_H