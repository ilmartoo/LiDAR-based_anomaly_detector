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

#include "logging/logging.hh"

/**
 * Enumeración de los codigos de finalización que puede devolver un escaneo
 */
enum ScanCode {
    kScanOk,    ///< Escaneo correcto
    kScanEof,   ///< Escaneo hasta el final del archivo
    kScanError  ///< Error de escaneo
};

/**
 * Interfaz de un escaner de puntos
 */
class IScanner {
   protected:
    std::function<void(const Point &p)> callback;  ///< Función de callback
    bool scanning;                                 ///< Variable para la finalización del escaneo de puntos

    inline static IScanner *instance = nullptr;  ///< Puntero a la instancia única del escaner

   public:
    /**
     * Devuelve la instancia única creada del escaner
     * @return Instancia única del escaner
     */
    static IScanner *getInstance() { return instance; }

    /**
     * Crea una instancia unica del escaner si no existe
     * @param ... Parametros necesarios para crear el escaner
     * @return Instancia única del escaner
     */
    template <class... Args>
    static IScanner *create(Args &&...args) {
        LOG_ERROR("Method not implemented!");
        return instance;
    }

    /**
     * Inicialización del escaner
     * @return Se devolverá true si se ha establecido el escaner correctamente
     */
    virtual bool init() = 0;

    /**
     * Comienza a escanear puntos.
     * Si no se quiere escanear hasta el final del archivo será responsabilidad del programador
     * hacer una llamada a la función pause() cuando se requiera parar el escaneo.
     * @return Se devolverá un ScanCode respecto a como ha finalizado el escaneo
     */
    virtual ScanCode scan() = 0;

    /**
     * Pausa el escaneo de puntos
     */
    virtual void pause() = 0;

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
     * Devuelve si el escaner está escaneando
     * @return true si el sensor está escaneando
     */
    bool isScanning() { return scanning; }

   protected:
    /**
     * Constructor
     */
    IScanner() : scanning(false) {}
    /**
     * Destructor virtual
     */
    virtual ~IScanner() {}
};

#endif  // SCANNER_INTERFACE_H