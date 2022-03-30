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

/**
 * Cararterizador de objetos que implementa la interfaz IObjectCharacterizator
 */
class ObjectCharacterizator : public IObjectCharacterizator {
   public:
    /**
     * Constructor
     * @param frameDuration Duración del frame de puntos en milisegundos
     */
    ObjectCharacterizator(uint32_t frameDuration, float backgroundDistance)
        : frameDuration(frameDuration * 1000000),
          backgroundDistance(backgroundDistance),
          minReflectivity(0.f),
          exit(true) {
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
     * @param backgroundTime Milisegundos durante los que todos los puntos recogidos formarán parte del background
     * @param minReflectivity Reflectividad mínima que necesitan los puntos para no ser descartados
     */
    virtual void start(uint32_t backgroundTime, float minReflectivity);

    /**
     * Para la caracterización de objetos
     */
    void stop();

   private:
    enum CharacterizatorState state;  ///< Estado en el que se encuentra el caracterizador de objetos

    uint64_t frameDuration;    ///< Duración del frame de puntos en nanosegundos
    float minReflectivity;     ///< Reflectividad mínima que necesitan los puntos para no ser descartados
    float backgroundDistance;  ///< Distancia mínima a la que tiene que estar un punto para no pertenecer al background

    std::vector<Point> *background;  ///< Mapa de puntos que forman el background
    PointMap *object;                ///< Mapa de puntos que forman el objeto

    std::thread *executionThread;  ///< Hilo de ejecución del caracterizador
    bool exit;                     ///< Variable para la finalización del hilo

    /**
     * Guarda en background y elimina los puntos del objeto fuera del frame
     * @param backgroundTime Milisegundos durante los que todos los puntos recogidos formarán parte del background
     */
    void managePoints(uint32_t backgroundTime);

    /**
     * Comprueba si un punto pertenece al background
     * @param p Punto a comprobar
     * @return true El punto pertenece al background
     * @return false El punto no pertenece al background
     */
    bool isBackground(const Point &p) const;
};

#endif  // OBJECTCARACTERIZATOR_CLASS_H