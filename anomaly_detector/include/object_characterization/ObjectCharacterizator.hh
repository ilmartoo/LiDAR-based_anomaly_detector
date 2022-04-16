/**
 * @file ObjectCharacterizator.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 25/03/2022
 *
 * Definición del objeto ObjectCharacterizator
 *
 */

#ifndef OBJECTCARACTERIZATOR_CLASS_H
#define OBJECTCARACTERIZATOR_CLASS_H

#include <vector>
#include <thread>

#include "object_characterization/IObjectCharacterizator.hh"
#include "models/Point.hh"
#include "models/CharacteristicsVector.hh"
#include "models/PointMap.hh"

#include "debug.hh"

/**
 * Estados en los que se puede encontrar el caracterizador de objetos
 */
enum CharacterizatorState {
    defStartTime,   ///< Definiendo el timestamp de inicio
    defBackground,  ///< Definiendo el background
    defObject,      ///< Definiendo objetos
    defStopped,     ///< Definición parada
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
    ObjectCharacterizator(uint32_t frameDuration, uint32_t backgroundTime, float minReflectivity, float backgroundDistance) {
        state = defStopped;

        this->frameDuration = frameDuration * 1000000;
        this->backgroundTime = backgroundTime * 1000000;
        this->minReflectivity = minReflectivity;
        this->backgroundDistance = backgroundDistance;
        this->exit = true;

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
    void newPoint(const Point &p);

    /**
     * Comienza la definición de objetos
     */
    virtual void start();

    /**
     * Para la caracterización de objetos
     */
    void stop();

   private:
    enum CharacterizatorState state;  ///< Estado en el que se encuentra el caracterizador de objetos

    uint64_t frameDuration;    ///< Duración del frame de puntos en nanosegundos
    uint64_t backgroundTime;   ///< Tiempo en el cual los puntos formarán parte del background
    float minReflectivity;     ///< Reflectividad mínima que necesitan los puntos para no ser descartados
    float backgroundDistance;  ///< Distancia mínima a la que tiene que estar un punto para no pertenecer al background

    Timestamp *startTimestamp;       ///< Timestamp del primer punto
    std::vector<Point> *background;  ///< Mapa de puntos que forman el background
    PointMap *object;                ///< Mapa de puntos que forman el objeto

    std::thread *executionThread;  ///< Hilo de ejecución del caracterizador
    bool exit;                     ///< Variable para la finalización del hilo

    /**
     * Guarda en background y elimina los puntos del objeto fuera del frame
     */
    void managePoints();

    /**
     * Comprueba si un punto pertenece al background
     * @param p Punto a comprobar
     * @return true El punto pertenece al background
     * @return false El punto no pertenece al background
     */
    bool isBackground(const Point &p) const;
};

#endif  // OBJECTCARACTERIZATOR_CLASS_H