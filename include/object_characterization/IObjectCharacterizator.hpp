/**
 * @file IObjectCharacterizator.hpp
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 25/03/2022
 *
 * Interfaz IObjectCharacterizator
 *
 */

#ifndef __OBJECTCARACTERIZATOR_INTERFACE_H
#define __OBJECTCARACTERIZATOR_INTERFACE_H

#include "models/Point.hpp"
#include "models/CharacteristicsVector.hpp"

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
    virtual void newPoint(Point p) = 0;

    /**
     * Descarta el background guardado y empieza a guardar uno nuevo
     * @param seconds Segundos durante los que se definirá el background
     */
    virtual void defineBackground(uint16_t seconds) = 0;

    /**
     * Comienza la definición de puntos
     */
    virtual void defineObjects() = 0;

    /**
     * Callback a donde se recebirán los puntos escaneados
     * @return Vector con las características del objeto escaneado
     */
    // virtual CharacteristicsVector extractCharacteristics() = 0;

    /**
     * Comienza la definición de objetos, definiendo el background durante los segundos
     * especificados para despues definir los objetos
     * @param backgroundDefinitionTime Segundos durante los que todos los puntos recogidos formarán
     * parte del background
     */
    void start(uint16_t backgroundDefinitionTime) {
        defineBackground(backgroundDefinitionTime);
        defineObjects();
    };

    /**
     * Para la caracterización de objetos
     */
    virtual void stop() = 0;
};

#endif  //__OBJECTCARACTERIZATOR_INTERFACE_H