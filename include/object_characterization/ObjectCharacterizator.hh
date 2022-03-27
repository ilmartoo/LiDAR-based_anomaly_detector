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

#include <queue>
#include <vector>
#include <mutex>
#include <thread>

#include "object_characterization/IObjectCharacterizator.hh"
#include "models/Point.hh"
#include "models/CharacteristicsVector.hh"

/**
 * Estructura para realizar el mapping de puntos
 */
struct PointMap {
    std::queue<Point> *map;  ///< Mapa de puntos
    uint64_t lastTimestamp;  ///< Ultimo timestamp obtenido
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
        state = defStopped;

        background = new std::vector<Point>();
        object.map = new std::queue<Point>();
    }

    /**
     * Destructor
     */
    ~ObjectCharacterizator() {
        delete background;
        delete object.map;
    }

    /**
     * Callback a donde se recebirán los puntos escaneados
     * @param p Punto escaneado
     */
    void newPoint(Point &p);

    /**
     * Comienza la definición de objetos, definiendo el background durante los milisegundos
     * especificados para despues definir los objetos
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

    uint32_t frameDuration;  ///< Duración del frame de puntos en milisegundos

    std::vector<Point> *background;  ///< Mapa de puntos que forman el background
    PointMap object;                 ///< Mapa de puntos que forman el objeto

    std::mutex mutexMaps;  ///< Mutex de acceso a los mapas de puntos

    std::thread timestampChecker;  ///< Hilo encargado de eliminar puntos del objeto con un frametime no válido

    /**
     * Elimina los puntos del objeto cullo tiempo de vida sea mayor que el máximo establecido
     */
    void removeOldObjectPoints();
};

#endif  //__OBJECTCARACTERIZATOR_CLASS_H