/**
 * @file IObjectCharacterizator.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 25/03/2022
 *
 * Interfaz IObjectCharacterizator
 *
 */

#ifndef OBJECTCARACTERIZATOR_INTERFACE_H
#define OBJECTCARACTERIZATOR_INTERFACE_H

#include <thread>

#include "models/Point.hh"
#include "models/CharacteristicsVector.hh"

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
    virtual void newPoint(const Point &p) = 0;

    /**
     * Callback a donde se recebirán los puntos escaneados
     * @return Vector con las características del objeto escaneado
     */
    // virtual CharacteristicsVector extractCharacteristics() = 0;

    /**
     * Comienza la definición de objetos, definiendo el background durante los milisegundos
     * especificados para despues definir los objetos
     */
    virtual void start() = 0;

    /**
     * Para la caracterización de objetos
     */
    virtual void stop() = 0;
};

#endif  // OBJECTCARACTERIZATOR_INTERFACE_H