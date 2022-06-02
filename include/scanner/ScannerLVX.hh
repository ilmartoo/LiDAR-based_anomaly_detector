/**
 * @file ScannerLVX.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 03/04/2022
 *
 * @brief Definicion del objeto ScannerLVX
 *
 */

#ifndef SCANNERLVX_CLASS_H
#define SCANNERLVX_CLASS_H

#include <iostream>
#include <string>
#include <functional>
#include <thread>

#include "lvx_file.h"

#include "scanner/IFileScanner.hh"
#include "models/LidarPoint.hh"

#include "logging/debug.hh"

/**
 * @brief Escáner de un archivo de puntos en formato lvx
 */
class ScannerLVX : public IFileScanner {
   private:
    livox_ros::LvxFileHandle lvx_file;            ///< Handler del archivo de puntos
    livox_ros::OutPacketBuffer packets_of_frame;  ///< Buffer de guardado de datos del archivo lvx
    uint32_t fileOffset;                          ///< Offset del archivo de datos
    uint32_t frameOffset;                         ///< Offset del frame de datos
    size_t packetOffset;                          ///< Offset del paquete de datos

   public:
    /**
     * Devuelve la instancia única creada del escaner
     * @return Instancia única del escaner
     */
    static ScannerLVX *getInstance() { return (ScannerLVX *)instance; }

    /**
     * Crea una instancia unica del escaner si no existe
     * @param filename Nombre del archivo de datos
     * @return Instancia única del escaner
     */
    static IFileScanner *create(std::string filename) {
        static ScannerLVX scanner = {filename};
        instance = (IScanner *)&scanner;
        return (IFileScanner *)instance;
    }

    /**
     * Inicialización del escaner
     * @return Se devolverá true si se ha establecido el escaner correctamente
     */
    bool init();

    /**
     * Comienza a escanear puntos.
     * Si no se quiere escanear hasta el final del archivo será responsabilidad del programador
     * hacer una llamada a la función pause() cuando se requiera parar el escaneo.
     * @return Se devolverá un ScanCode respecto a como ha finalizado el escaneo
     */
    ScanCode scan();

    /**
     * Pausa el escaneo de puntos
     */
    void pause();

    /**
     * Establece la función especificada como función de callback a la que se llamará cada vez que
     * se escanee un nuevo punto
     * @param func Función de callback a ser llamada por el sensor
     * @return Se devolverá true si se ha establecido el callback correctamente
     */
    bool setCallback(const std::function<void(const LidarPoint &p)> func);

    /**
     * Espera pasiva a la finalización del escaneo del archivo de puntos
     */
    void wait();

    /**
     * Finaliza el escaner
     */
    void stop();

   private:
    /**
     * Constructor del objeto ScannerLVX
     * @param filename Archivo contenedor de datos
     */
    ScannerLVX(const std::string &filename) : IFileScanner(filename) {}
    /**
     * Destructor del scanner
     */
    ~ScannerLVX() {
        if (lvx_file.GetFileState() == livox_ros::kLvxFileOk) {
            lvx_file.CloseLvxFile();
        }
    }

    /**
     * Lee los puntos del archivo de input
     * @return Devuelve un ScanCode según la finalización de la lectura del archivo
     */
    ScanCode readData();
};

#endif  // SCANNERLVX_CLASS_H