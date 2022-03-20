#ifndef __OBJECTCARACTERIZATOR_INTERFACE_H
#define __OBJECTCARACTERIZATOR_INTERFACE_H

#include "models/Point.hpp"
#include "models/CharacteristicsVector.hpp"

class IObjectCharacterizator {
   public:
    /**
     * Callback a donde se recebirán los puntos escaneados
     * @param p Punto escaneado
     */
    virtual void newPoint(Point p) = 0;

    /**
     * Descarta el objeto escaneado guardado en el buffer
     */
    virtual void newObject() = 0;

    /**
     * Callback a donde se recebirán los puntos escaneados
     * @return Vector con las características del objeto escaneado
     */
    virtual CharacteristicsVector extractCharacteristics() = 0;
};

#endif  //__OBJECTCARACTERIZATOR_INTERFACE_H