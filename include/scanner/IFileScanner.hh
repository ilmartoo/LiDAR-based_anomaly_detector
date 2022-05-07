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
#include <mutex>
#include <condition_variable>

#include "scanner/IScanner.hh"
#include "models/Point.hh"

#include "logging/logging.hh"

/**
 * Interfaz de un escaner de puntos
 */
class IFileScanner : public IScanner {
   protected:
    std::string filename;  ///< Nombre del archivo de datos

   public:
    /**
     * Destructor virtual
     */
    virtual ~IFileScanner() {}

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
        LOG_ERROR("Method not implemented!");
        return (IFileScanner *)instance;
    }

    /**
     * Espera pasiva a la finalización del escaneo del archivo de puntos
     */
    virtual void wait() = 0;

   protected:
    /**
     * Constructor
     */
    IFileScanner(const std::string &filename) : filename(filename) {}

    /**
     * Lee los puntos del archivo de input
     */
    virtual void readData(std::condition_variable &cv, std::mutex &mutex) = 0;
};

#endif  // FILE_SCANNER_INTERFACE_H