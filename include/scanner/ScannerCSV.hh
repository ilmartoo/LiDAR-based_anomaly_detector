/**
 * @file ScannerCSV.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 19/03/2022
 *
 * @brief Definicion del objeto ScannerCSV
 *
 */

#ifndef SCANNERCSV_CLASS_H
#define SCANNERCSV_CLASS_H

#include <iostream>
#include <string>
#include <fstream>
#include <functional>
#include <thread>

#include "scanner/IFileScanner.hh"
#include "models/LidarPoint.hh"

#include "logging/debug.hh"

/**
 * @brief Escáner de un archivo de puntos en formato csv
 */
class ScannerCSV : public IFileScanner {
   private:
    std::ifstream infile;  ///< Stream del archivo de datos

   public:
    /**
     * Devuelve la instancia única creada del escaner
     * @return Instancia única del escaner
     */
    static ScannerCSV *getInstance() { return (ScannerCSV *)instance; }

    /**
     * Crea una instancia unica del escaner si no existe
     * @param filename Nombre del archivo de datos
     * @return Instancia única del escaner
     */
    static IFileScanner *create(std::string filename) {
        static ScannerCSV scanner = {filename};
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
     * Finaliza el escaner
     */
    void stop();

   private:
    /**
     * Constructor del objeto ScannerCSV
     * @param filename Archivo contenedor de datos
     */
    ScannerCSV(const std::string &filename) : IFileScanner(filename) {}
    /**
     * Destructor del scanner
     */
    ~ScannerCSV() {
        if (this->infile.is_open()) {
            this->infile.close();
        }
    }

    /**
     * Lee los puntos del archivo de input
     * @return Devuelve un ScanCode según la finalización de la lectura del archivo
     */
    ScanCode readData();
};

#endif  // SCANNERCSV_CLASS_H
