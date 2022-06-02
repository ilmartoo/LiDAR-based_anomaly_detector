/**
 * @file IFileScanner.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 01/05/2022
 *
 * @brief Interfaz IFileScanner
 *
 */

#ifndef FILE_SCANNER_INTERFACE_H
#define FILE_SCANNER_INTERFACE_H

#include <string>
#include <functional>

#include "scanner/IScanner.hh"
#include "models/Point.hh"

#include "logging/debug.hh"

/**
 * @brief Interfaz de un escaner de puntos a partir de un archivo
 */
class IFileScanner : public IScanner {
   protected:
    std::string filename;  ///< Nombre del archivo de datos

   public:
    /**
     * Devuelve la instancia única creada del escaner
     * @return Instancia única del escaner
     */
    static IFileScanner *getInstance() { return (IFileScanner *)instance; }

    /**
     * Devuelve una instancia unica del escaner
     * @param filename Nombre del archivo de datos
     * @return Instancia única del escaner
     */
    static IFileScanner *create(std::string filename) {
        DEBUG_STDERR("Method not implemented");
        return (IFileScanner *)instance;
    }

   protected:
    /**
     * Constructor
     */
    IFileScanner(const std::string &filename) : filename(filename) {}
    /**
     * Destructor virtual
     */
    virtual ~IFileScanner() {}

    /**
     * Lee los puntos del archivo de input
     * @return Devuelve un ScanCode según la finalización de la lectura del archivo
     */
    virtual ScanCode readData() = 0;
};

#endif  // FILE_SCANNER_INTERFACE_H