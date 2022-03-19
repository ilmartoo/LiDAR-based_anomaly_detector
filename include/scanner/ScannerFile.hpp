/**
 * @file ScannerFile.hpp
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @brief Definición del objeto ScannerFile, implementador de la interfaz IScanner
 * @date 19/03/2022
 *
 *
 */

#ifndef __SCANNERFILE_H
#define __SCANNERFILE_H

#include <iostream>
#include <string>
#include <fstream>
#include <functional>

#include "scanner/IScanner.hpp"
#include "Point.hpp"

// Implementación de la interfaz IScanner para la lectura de puntos de archivos CSV obtenidos del
// programa propietario Livox Viewer
class ScannerFile : public IScanner {
   public:
    // Constructor
    ScannerFile(std::string filename);

    // Destructor
    ~ScannerFile();

    /**
     * Inicialización del escaner
     * @return Se devolverá true si se ha establecido el escaner correctamente
     */
    bool initScanner();

    /**
     * Comienza la obtención de puntos
     * @return Se devolverá true al finalizar de leer correctamente el archivo o false si ocurre un
     * error en el proceso
     */
    bool startScanner();

    /**
     * Establece la función especificada como función de callback a la que se llamará cada vez que
     * se escanee un nuevo punto
     * @param func Función de callback a ser llamada por el sensor
     * @return Se devolverá true si se ha establecido el callback correctamente
     */
    bool setCallback(std::function<void(Point)> func);

    /**
     * Finaliza el escaner
     */
    void closeScanner();

   private:
    std::string filename;  // Nombre del archivo de datos
    std::ifstream infile;  // Stream del archivo de datos

    std::function<void(Point)> callback;  // Función de callback
};

#endif  //__SCANNERFILE_H
