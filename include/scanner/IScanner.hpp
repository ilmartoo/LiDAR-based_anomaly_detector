/**
 * @file IScanner.hpp
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @brief Interfaz IScanner
 * @date 19/03/2022
 *
 *
 */

#ifndef __SCANNER_INTERFACE_H
#define __SCANNER_INTERFACE_H

#include <string>
#include <functional>

#include "Point.hpp"

/**
 * Interfaz de un escaner de puntos
 */
class IScanner {
   public:
    /**
     * Destructor
     */
    virtual ~IScanner(){};

    /**
     * Inicialización del escaner
     * @return Se devolverá true si se ha establecido el escaner correctamente
     */
    virtual bool initScanner() = 0;

    /**
     * Comienza la obtención de puntos
     * @return Se devolverá true al finalizar de leer correctamente el archivo o false si ocurre un
     * error en el proceso
     */
    virtual bool startScanner() = 0;

    /**
     * Establece la función especificada como función de callback a la que se llamará cada vez que
     * se escanee un nuevo punto
     * @param func Función de callback a ser llamada por el sensor
     * @return Se devolverá true si se ha establecido el callback correctamente
     */
    virtual bool setCallback(std::function<void(Point)> func) = 0;

    /**
     * Finaliza el escaner
     */
    virtual void closeScanner() = 0;
};

#endif  //__SCANNER_INTERFACE_H