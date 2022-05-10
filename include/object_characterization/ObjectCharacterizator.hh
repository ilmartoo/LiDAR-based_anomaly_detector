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
#include <utility>

#include "scanner/IScanner.hh"
#include "models/Point.hh"
#include "models/OctreeMap.hh"
#include "object_characterization/CharacterizedObject.hh"

/**
 * Estados en los que se puede encontrar el caracterizador de objetos
 */
enum CharacterizatorState {
    defBackground,  ///< Definiendo el fondo
    defObject,      ///< Definiendo objetos
    defDiscard,     ///< Descarte de puntos intencionado
    defStopped      ///< Definición parada
};

/**
 * Cararterizador de objetos que implementa la interfaz IObjectCharacterizator
 */
class ObjectCharacterizator {
   private:
    IScanner *scanner;  ///< Escaner de puntos

    bool chrono;            ///< Activador de la medicion de tiempos
    uint64_t objFrame;      ///< Duración del frame de puntos en nanosegundos
    uint64_t backFrame;     ///< Tiempo en el cual los puntos formarán parte del fondo
    float minReflectivity;  ///< Reflectividad mínima que necesitan los puntos para no ser descartados
    float backDistance;     ///< Distancia mínima a la que tiene que estar un punto para no pertenecer al fondo

    enum CharacterizatorState state;  ///< Estado en el que se encuentra el caracterizador de objetos
    OctreeMap *background;            ///< Mapa de puntos que forman el fondo
    OctreeMap *object;                ///< Mapa de puntos que forman el objeto

    uint64_t discardTime;                         ///< Tiempo durante el cual se descartarán puntos
    std::pair<bool, Timestamp> discardStartTime;  ///< Timestamp de inicio del descarte de puntos

   public:
    /**
     * Constructor del objeto ObjectCharacterizator
     * @param scanner Escaner de puntos a utilizar por el caracterizador
     * @param objFrame Tiempo en ms que durará un frame
     * @param backFrame Tiempo en ms en los que se escaneará en fondo
     * @param minReflectivity Reflectividad mínima de los puntos
     * @param backDistance Distancia al fondo en metros
     * @param chrono Activador del cronometraje de tiempos
     */
    ObjectCharacterizator(IScanner *scanner, uint32_t objFrame, uint32_t backFrame, float minReflectivity, float backDistance, bool chrono)
        : scanner(scanner),
          chrono(chrono),
          objFrame(objFrame * 1000000),
          backFrame(backFrame * 1000000),
          minReflectivity(minReflectivity),
          backDistance(backDistance),
          state(defStopped),
          discardTime(0),
          discardStartTime(false, Timestamp(0, 0)) {
        background = new OctreeMap();
        object = new OctreeMap();
    }
    /**
     * Destructor
     */
    ~ObjectCharacterizator() {
        delete background;
        delete object;
    }

    /**
     * Callback a donde se recebirán los puntos escaneados
     * @param p Punto escaneado
     */
    void newPoint(const Point &p);

    /**
     * Inicializa el caracterizador
     */
    void init();

    /**
     * Define el fondo de la escena
     */
    void defineBackground();

    /**
     * Define un objeto en la escena
     * @return Objecto caracterizado
     */
    CharacterizedObject defineObject();

    /**
     * Descarta puntos durante la duracion especificada
     * @param miliseconds Milisegundos a esperar
     */
    void wait(uint32_t miliseconds);

    /**
     * Para la caracterización de objetos
     */
    void stop();

    ////// Setters
    /**
     * Setter del cronometraje
     * @param chrono Booleano para establecer el nuevo cronometraje
     */
    void setChrono(bool chrono) { this->chrono = chrono; }
    /**
     * Setter de la duración del frame
     * @param objFrame Nueva duración del frame en ms
     */
    void setObjFrame(uint32_t objFrame) { this->objFrame = objFrame * 1000000; }
    /**
     * Setter del tiempo de escaneo del fondo
     * @param backFrame Nuevo tiempo de escaneo del fondo en ms
     */
    void setBackFrame(uint32_t backFrame) { this->backFrame = backFrame * 1000000; }
    /**
     * Setter de la reflectividad minima
     * @param minReflectivity Nueva reflectividad minima
     */
    void setMinReflectivity(float minReflectivity) { this->minReflectivity = minReflectivity; }
    /**
     * Setter de la distancia al fondo
     * @param backDistance Nueva distancia al fondo
     */
    void setBackDistance(float backDistance) { this->backDistance = backDistance; }

    ////// Getters
    /**
     * Devuelve si se está cronometrando la caracterización
     * @return Booleano conforme está activado el cronometraje
     */
    bool isChrono() const { return this->chrono; }
    /**
     * Getter de la duración del frame
     * @return Duración del frame
     */
    uint64_t getObjFrame() const { return this->objFrame; }
    /**
     * Getter del tiempo de escaneo del fondo
     * @return Tiempo de escaneo del fondo
     */
    uint64_t getBackFrame() const { return this->backFrame; }
    /**
     * Getter de la reflectividad minima
     * @return Reflectividad minima
     */
    float getMinReflectivity() const { return this->minReflectivity; }
    /**
     * Getter de la distancia al fondo
     * @return Distancia al fondo
     */
    float getBackDistance() const { return this->backDistance; }

   private:
    /**
     * Guarda en fondo y elimina los puntos del objeto fuera del frame
     */
    void managePoints();
    /**
     * Comprueba si un punto pertenece al fondo
     * @param p Punto a comprobar
     * @return true si el punto pertenece al fondo o false en caso contrario
     */
    bool isBackground(const Point &p) const;
};

#endif  // OBJECTCARACTERIZATOR_CLASS_H