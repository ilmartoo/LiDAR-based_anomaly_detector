/**
 * @file ObjectCharacterizator.hpp
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 25/03/2022
 *
 * Definición del objeto ObjectCharacterizator
 *
 */

#ifndef __OBJECTCARACTERIZATOR_CLASS_H
#define __OBJECTCARACTERIZATOR_CLASS_H

#include <vector>

#include "object_characterization/IObjectCharacterizator.hpp"
#include "models/Point.hpp"
#include "models/CharacteristicsVector.hpp"

// Numero de puntos a reservar dinamicamente para el background y el incremento si no hay suficientes
#define POINTS_BACKGROUND_INIT 3000
#define POINTS_BACKGROUND_INC  1000

// Numero de puntos a reservar dinamicamente para el objeto y el incremento si no hay suficientes
#define POINTS_OBJECT_INIT     1000
#define POINTS_OBJECT_INC      500

struct PointMap {
    std::vector<Point> *map;
    uint32_t index;
    uint32_t size;
};

/**
 * Cararterizador de objetos que implementa la interfaz IObjectCharacterizator
 */
class ObjectCharacterizator : public IObjectCharacterizator {
   public:
    /**
     * Constructor
     * @param frameDuration Duración del frame de puntos en milisegundos
     */
    ObjectCharacterizator(uint32_t frameDuration) : frameDuration(frameDuration) {
        background.index = 0;
        background.size = POINTS_BACKGROUND_INIT;
        background.map = new std::vector<Point>(background.size);

        object.index = 0;
        object.size = POINTS_OBJECT_INIT;
        object.map = new std::vector<Point>(object.size);
    }

    /**
     * Destructor
     */
    ~ObjectCharacterizator() {
        delete background.map;
        delete object.map;
    }

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
    enum CharacterizatorState state;  ///< Estado en el que se encuentra el caracterizador de objetos

    struct PointMap background;  ///< Mapa de puntos que forman el background
    struct PointMap object;      ///< Mapa de puntos que forman el objeto

    uint32_t frameDuration;  ///< Duración del frame de puntos en milisegundos
};

#endif  //__OBJECTCARACTERIZATOR_CLASS_H