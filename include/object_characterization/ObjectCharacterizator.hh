/**
 * @file ObjectCharacterizator.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 25/03/2022
 *
 * Definición del objeto ObjectCharacterizator
 *
 */

#ifndef __OBJECTCARACTERIZATOR_CLASS_H
#define __OBJECTCARACTERIZATOR_CLASS_H

#include <vector>
#include <thread>

#include "object_characterization/IObjectCharacterizator.hh"
#include "models/Point.hh"
#include "models/CharacteristicsVector.hh"
#include "models/PointMap.hh"

/**
 * Cararterizador de objetos que implementa la interfaz IObjectCharacterizator
 */
class ObjectCharacterizator : public IObjectCharacterizator {
   public:
    /**
     * Constructor
     * @param frameDuration Duración del frame de puntos en milisegundos
     */
    ObjectCharacterizator(uint32_t frameDuration) : frameDuration(frameDuration * 1000000) {
        state = defStopped;

        background = new std::vector<Point>();
        object = new PointMap();
    }

    /**
     * Destructor
     */
    ~ObjectCharacterizator() {
        delete background;
        delete object;
        delete executionThread;
    }

    /**
     * Callback a donde se recebirán los puntos escaneados
     * @param p Punto escaneado
     */
    void newPoint(Point &p);

    /**
     * Comienza la definición de objetos
     * @param backgroundTime Milisegundos durante los que todos los puntos recogidos formarán
     * parte del background
     */
    void start(uint16_t backgroundTime);

    /**
     * Para la caracterización de objetos
     */
    void stop();

   private:
    enum CharacterizatorState state;  ///< Estado en el que se encuentra el caracterizador de objetos

    uint64_t frameDuration;  ///< Duración del frame de puntos en nanosegundos

    std::vector<Point> *background;  ///< Mapa de puntos que forman el background
    PointMap *object;                ///< Mapa de puntos que forman el objeto

    /**
     * Guarda en background y elimina los puntos del objeto fuera del frame
     * @param backgroundTime Milisegundos durante los que todos los puntos recogidos formarán
     * parte del background
     */
    void managePoints(uint16_t backgroundTime);
};

#endif  //__OBJECTCARACTERIZATOR_CLASS_H