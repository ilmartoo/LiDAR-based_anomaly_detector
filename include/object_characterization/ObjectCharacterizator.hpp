#ifndef __OBJECTCARACTERIZATOR_CLASS_H
#define __OBJECTCARACTERIZATOR_CLASS_H

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
class ObjectCharacterizator {
   public:
    /**
     * Callback a donde se recebirán los puntos escaneados
     * @param p Punto escaneado
     */
    void newPoint(Point p);

    /**
     * Descarta el background guardado y empieza a guardar uno nuevo
     * @param seconds Segundos durante los que se definirá el background
     */
    void defineBackground(uint16_t seconds);

    /**
     * Comienza la definición de puntos
     */
    void defineObjects();

    /**
     * Para la caracterización de objetos
     */
    void stop();

   private:
    Point *background;
    Point *object;
};

#endif  //__OBJECTCARACTERIZATOR_CLASS_H