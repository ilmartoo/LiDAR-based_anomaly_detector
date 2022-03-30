/**
 * @file IObjectCharacterizator.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 25/03/2022
 *
 * Interfaz IObjectCharacterizator
 *
 */

#ifndef __OBJECTCARACTERIZATOR_INTERFACE_H
#define __OBJECTCARACTERIZATOR_INTERFACE_H

#include <thread>

#include "models/Point.hh"
#include "models/CharacteristicsVector.hh"

/**
 * Estados en los que se puede encontrar un caracterizador de objetos
 */
enum CharacterizatorState {
    defBackground,  ///< Definiendo el background
    defObject,      ///< Definiendo objetos
    defStopped,     ///< Definición parada
};

/**
 * Interfaz del cararterizador de objetos
 */
class IObjectCharacterizator {
   public:
    /**
     * Destructor virtual
     */
    virtual ~IObjectCharacterizator() {}

    /**
     * Callback a donde se recebirán los puntos escaneados
     * @param p Punto escaneado
     */
    virtual void newPoint(Point &p) = 0;

    /**
     * Callback a donde se recebirán los puntos escaneados
     * @return Vector con las características del objeto escaneado
     */
    // virtual CharacteristicsVector extractCharacteristics() = 0;

    /**
     * Comienza la definición de objetos, definiendo el background durante los milisegundos
     * especificados para despues definir los objetos
     * @param backgroundTime Milisegundos durante los que todos los puntos recogidos formarán
     * parte del background
     */
    virtual void start(uint16_t backgroundTime) = 0;

    /**
     * Para la caracterización de objetos
     */
    virtual void stop() = 0;

   protected:
    std::thread *executionThread;  ///< Hilo de ejecución del caracterizador
};

#endif  //__OBJECTCARACTERIZATOR_INTERFACE_H