/**
 * @file ScannerCSV.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 19/03/2022
 *
 * Definicion del objeto ScannerCSV
 *
 */

#ifndef SCANNERCSV_CLASS_H
#define SCANNERCSV_CLASS_H

#include <iostream>
#include <string>
#include <fstream>
#include <functional>
#include <thread>
#include <mutex>

#include "scanner/IFileScanner.hh"
#include "models/Point.hh"

#include "logging/debug.hh"

/**
 * Implementación de la interfaz IScanner para la lectura de puntos de archivos CSV obtenidos del
 * programa propietario Livox Viewer
 */
class ScannerCSV : public IFileScanner {
   private:
    std::ifstream infile;       ///< Stream del archivo de datos

   public:
    /**
     * Destructor del scanner
     */
    ~ScannerCSV() {
        if (this->infile.is_open()) {
            this->infile.close();
        }
    }

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
     * Comienza la obtención de puntos
     * @param mutex mutex para bloquear al caracterizador hasta que termine de escanear
     * @return Se devolverá true si se ha comenzado el escaneo correctamente
     */
    bool scan(std::condition_variable &cv, std::mutex &mutex);

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
    bool setCallback(const std::function<void(const Point &p)> func);

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
     * Constructor del objeto ScannerCSV
     * @param filename Archivo contenedor de datos
     */
    ScannerCSV(const std::string &filename) : IFileScanner(filename) {}

    /**
     * Lee los puntos del archivo de input
     */
    void readData(std::condition_variable &cv, std::mutex &mutex);
};

#endif  // SCANNERCSV_CLASS_H
