/**                                                          \
 * @file ScannerLVX.hh                                       \
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es) \
 * @date 03/04/2022                                          \
 *                                                           \
 * Definicion del objeto ScannerLVX                          \
 *                                                           \
 */

#ifndef SCANNERLVX_CLASS_H
#define SCANNERLVX_CLASS_H

#include <iostream>
#include <string>
#include <fstream>
#include <functional>
#include <thread>

#include "read_lvx/lvx_file.h"

#include "scanner/IFileScanner.hh"
#include "models/Point.hh"

#include "logging/debug.hh"

/**
 * Implementación de la interfaz IScanner para la lectura de puntos de archivos CSV obtenidos del
 * programa propietario Livox Viewer
 */
class ScannerLVX : public IFileScanner {
   public:
    /**
     * Constructor del objeto ScannerLVX
     * @param filename Archivo contenedor de datos
     */
    ScannerLVX(const std::string &filename) : filename(filename), exit(true) {}

    /**
     * Destructor del scanner
     */
    ~ScannerLVX() {
        if (lvx_file.GetFileState() == livox_ros::kLvxFileOk) {
            lvx_file.CloseLvxFile();
        }
        delete executionThread;
    };

    /**
     * Inicialización del escaner
     * @return Se devolverá true si se ha establecido el escaner correctamente
     */
    bool init();

    /**
     * Comienza la obtención de puntos
     * @return Se devolverá true al finalizar de leer correctamente el archivo o false si ocurre un
     * error en el proceso
     */
    bool start();

    /**
     * Establece la función especificada como función de callback a la que se llamará cada vez que
     * se escanee un nuevo punto
     * @param func Función de callback a ser llamada por el sensor
     * @return Se devolverá true si se ha establecido el callback correctamente
     */
    bool setCallback(const std::function<void(const Point &p)> func);

    /**
     * Finaliza el escaner
     */
    void stop();

    /**
     * Non-busy wait hasta la finalización del escaner
     */
    void wait();

   private:
    const std::string &filename;  ///< Nombre del archivo de datos

    livox_ros::LvxFileHandle lvx_file;            ///< Handler del archivo de puntos
    livox_ros::OutPacketBuffer packets_of_frame;  ///< Buffer de guardado de datos del archivo lvx

    std::function<void(const Point &p)> callback;  ///< Función de callback

    std::thread *executionThread;  ///< Hilo de ejecución del escaner
    bool exit;                     ///< Variable para la finalización del hilo

    /**
     * Lee los puntos del archivo de input
     */
    void readData();
};

#endif  // SCANNERLVX_CLASS_H