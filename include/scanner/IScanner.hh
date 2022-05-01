/**
 * @file IScanner.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 19/03/2022
 *
 * Interfaz IScanner
 *
 */

#ifndef SCANNER_INTERFACE_H
#define SCANNER_INTERFACE_H

#include <string>
#include <functional>

#include "models/Point.hh"

/**
 * Interfaz de un escaner de puntos
 */
class IScanner {
   public:
    /**
     * Destructor virtual
     */
    virtual ~IScanner(){};

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

   protected:
    /**
     * Constructor
     */
    IScanner() : exit(false) {}

    std::function<void(const Point &p)> callback;  ///< Función de callback
    bool exit;                                     ///< Variable para la finalización del escaneo de puntos
};

#endif  // SCANNER_INTERFACE_H