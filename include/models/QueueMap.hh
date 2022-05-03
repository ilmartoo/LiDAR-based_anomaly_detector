/**
 * @file QueueMap.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 29/03/2022
 *
 * Definición e implementación del objeto QueueMap
 *
 */

#ifndef QUEUEMAP_CLASS_H
#define QUEUEMAP_CLASS_H

#include <queue>
#include <mutex>

#include "models/Point.hh"
#include "models/Timestamp.hh"

#include "logging/debug.hh"

/**
 * Clase para realizar el mapping de puntos mediante una cola FIFO concurrente
 */
class QueueMap {
   public:
    /**
     * Constructor del objero QueueMap
     */
    QueueMap() : lastTimestamp(false, nullptr) {}

    /**
     * Introduce un punto al final de la cola
     * @param p Nuevo punto a introducir en la cola
     */
    void push(const Point &p) {
        mutexMaps.lock();  // Bloqueamos el mutex

        map.push(p);                                                                  // Guardamos punto
        lastTimestamp = {true, const_cast<Timestamp *>(&map.back().getTimestamp())};  // Guardamos último timestamp

        mutexMaps.unlock();  // Desbloqueamos el mutex
    }

    /**
     * Quita el primer punto de la cola
     */
    void pop() {
        mutexMaps.lock();  // Bloqueamos el mutex

        map.pop();  // Eliminamos punto

        mutexMaps.unlock();  // Desbloqueamos el mutex
    }

    /**
     * Comprobación si la cola está vacía
     * @return true La cola está vacía
     * @return false La cola no está vacía
     */
    bool empty() { return this->map.empty(); }

    /**
     * Obtiene el primer punto de la cola
     * @return Primer punto de la cola
     */
    inline Point &front() { return map.front(); }

    /**
     * Devuelve el último timestamp introducido
     * @return Devuelve un elemento std::pair con true si existe ultimo timestamp y un puntero al último Timestamp.
     * El puntero es nulo si el primer elemento es false.
     */
    const std::pair<bool, Timestamp *> &getLastTimestamp() const { return lastTimestamp; }

    /**
     * Devuelve la cola correspondiente al mapa de puntos
     * @return queue<Point> Mapa de puntos
     */
    const std::queue<Point> &getMap() const { return this->map; }

   private:
    std::mutex mutexMaps;                        ///< Mutex de acceso a los mapas de puntos
    std::queue<Point> map;                       ///< Mapa de puntos
    std::pair<bool, Timestamp *> lastTimestamp;  ///< Ultimo timestamp obtenido
};

#endif  // QUEUEMAP_CLASS_H