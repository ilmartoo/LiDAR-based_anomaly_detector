/**
 * @file IFileScanner.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 25/04/2022
 *
 * Interfaz IFileScanner
 *
 */

#ifndef FILESCANNER_INTERFACE_H
#define FILESCANNER_INTERFACE_H

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
    virtual ~IFileScanner(){};

    /**
     * Inicialización del escaner
     * @return Se devolverá true si se ha establecido el escaner correctamente
     */
    virtual bool init() = 0;

    /**
     * Comienza la obtención de puntos
     * @return Se devolverá true al finalizar de leer correctamente el archivo o false si ocurre un
     * error en el proceso
     */
    virtual bool start() = 0;

    /**
     * Establece la función especificada como función de callback a la que se llamará cada vez que
     * se escanee un nuevo punto
     * @param func Función de callback a ser llamada por el sensor
     * @return Se devolverá true si se ha establecido el callback correctamente
     */
    virtual bool setCallback(std::function<void(const Point &p)> func) = 0;

    /**
     * Finaliza el escaner
     */
    virtual void stop() = 0;

    /**
     * Non-busy wait hasta la finalización del escaner
     */
    virtual void wait() = 0;
};

#endif  // FILESCANNER_INTERFACE_H