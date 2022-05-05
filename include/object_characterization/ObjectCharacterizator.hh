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
#include "models/QueueMap.hh"
#include "models/OctreeMap.hh"

#include "logging/debug.hh"

/**
 * Estados en los que se puede encontrar el caracterizador de objetos
 */
enum CharacterizatorState {
    defBackground,     ///< Definiendo el background
    defObject,         ///< Definiendo objetos
    defStopped,        ///< Definición parada
};

/**
 * Cararterizador de objetos que implementa la interfaz IObjectCharacterizator
 */
class ObjectCharacterizator {
   public:
    /**
     * Constructor del objeto ObjectCharacterizator
     * @param frameDuration Tiempo en ms que durará un frame
     * @param backgroundTime Tiempo en ms en los que se escaneará en fondo
     * @param minReflectivity Reflectividad mínima de los puntos
     * @param backgroundDistance Distancia al fondo en metros
     * @param timer Activador del cronometraje de tiempos
     */
    ObjectCharacterizator(uint32_t frameDuration, uint32_t backgroundTime, float minReflectivity, float backgroundDistance, bool timer)
        : timer(timer),
          frameDuration(frameDuration * 1000000),
          backgroundTime(backgroundTime * 1000000),
          minReflectivity(minReflectivity),
          backgroundDistance(backgroundDistance),
          exit(true) {
        state = defStopped;

        background = new OctreeMap();
        object = new QueueMap();
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
     * Define el fondo de la escena
     */
    void defineBackground();

    /**
     * Define un objeto en la escena
     */
    void defineObject();

    /**
     * Para la caracterización de objetos
     */
    void stop();

    /* Setters */
    /**
     * Setter del cronometraje
     * @param timer Booleano para establecer el nuevo cronometraje
     */
    void setTimer(const bool timer) { this->timer = timer; }
    /**
     * Setter de la duración del frame
     * @param frameDuration Nueva duración del frame
     */
    void setFrameDuration(const uint64_t &frameDuration) { this->frameDuration = frameDuration; }
    /**
     * Setter del tiempo de escaneo del background
     * @param backgroundTime Nuevo tiempo de escaneo del background
     */
    void setBackgroundTime(const uint64_t &backgroundTime) { this->backgroundTime = backgroundTime; }
    /**
     * Setter de la reflectividad minima
     * @param minReflectivity Nueva reflectividad minima
     */
    void setMinReflectivity(const float &minReflectivity) { this->minReflectivity = minReflectivity; }
    /**
     * Setter de la distancia al background
     * @param backgroundDistance Nueva distancia al background
     */
    void setBackgroundDistance(const float &backgroundDistance) { this->backgroundDistance = backgroundDistance; }

    /* Getters */
    /**
     * Devuelve si se está cronometrando la caracterización
     * @return Booleano conforme está activado el cronometraje
     */
    const bool isTimer() const { return this->timer; }
    /**
     * Getter de la duración del frame
     * @return Duración del frame
     */
    const uint64_t &getFrameDuration() const { return this->frameDuration; }
    /**
     * Getter del tiempo de escaneo del background
     * @return Tiempo de escaneo del background
     */
    const uint64_t &getBackgroundTime() const { return this->backgroundTime; }
    /**
     * Getter de la reflectividad minima
     * @return Reflectividad minima
     */
    const float &getMinReflectivity() const { return this->minReflectivity; }
    /**
     * Getter de la distancia al background
     * @return Distancia al background
     */
    const float &getBackgroundDistance() const { return this->backgroundDistance; }

   private:
    enum CharacterizatorState state;  ///< Estado en el que se encuentra el caracterizador de objetos

    bool timer;  ///< Activador de la medicion de tiempos

    uint64_t frameDuration;    ///< Duración del frame de puntos en nanosegundos
    uint64_t backgroundTime;   ///< Tiempo en el cual los puntos formarán parte del background
    float minReflectivity;     ///< Reflectividad mínima que necesitan los puntos para no ser descartados
    float backgroundDistance;  ///< Distancia mínima a la que tiene que estar un punto para no pertenecer al background

    Timestamp *startTimestamp;  ///< Timestamp del primer punto
    OctreeMap *background;      ///< Mapa de puntos que forman el background
    QueueMap *object;           ///< Mapa de puntos que forman el objeto

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