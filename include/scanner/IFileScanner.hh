/**
 * @file IFileScanner.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 01/05/2022
 *
 * Interfaz IFileScanner
 *
 */

#ifndef FILE_SCANNER_INTERFACE_H
#define FILE_SCANNER_INTERFACE_H

#include <string>
#include <functional>

#include "scanner/IScanner.hh"
#include "models/Point.hh"

/**
 * Interfaz de un escaner de puntos
 */
class IFileScanner : public IScanner {
   public:
    /**
     * Destructor virtual
     */
    virtual ~IFileScanner() {}

    /**
     * Espera pasiva a la finalización del escaneo del archivo de puntos
     */
    virtual void wait() = 0;

   protected:
    /**
     * Constructor
     */
    IFileScanner(const std::string &filename) : filename(filename) {}

    std::string filename;          ///< Nombre del archivo de datos
    std::thread *executionThread;  ///< Hilo de ejecución del escaner

    /**
     * Lee los puntos del archivo de input
     */
    virtual void readData() = 0;
};

#endif  // FILE_SCANNER_INTERFACE_H