/**
 * @file PointMap.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 29/03/2022
 *
 * Definición e implementación del objeto PointMap
 *
 */

#ifndef __POINTMAP_CLASS_H
#define __POINTMAP_CLASS_H

#include <queue>
#include <mutex>

#include <iostream>

#include "models/Point.hh"
#include "models/Timestamp.hh"

/**
 * Clase para realizar el mapping de puntos mediante una cola FIFO concurrente
 */
class PointMap {
   public:
    /**
     * Constructor del objero PointMap
     */
    PointMap() { lastTimestamp = nullptr; }

    /**
     * Introduce un punto al final de la cola
     * @param p Nuevo punto a introducir en la cola
     */
    void push(Point &p) {
        mutexMaps.lock();  // Bloqueamos el mutex

        map.push(p);                                           // Guardamos punto
        lastTimestamp = const_cast<Timestamp *>(&map.back().getTimestamp());  // Guardamos último timestamp

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

    const Timestamp getLastTimestamp() const { return *this->lastTimestamp; }

   private:
    std::mutex mutexMaps;      ///< Mutex de acceso a los mapas de puntos
    std::queue<Point> map;     ///< Mapa de puntos
    Timestamp *lastTimestamp;  ///< Ultimo timestamp obtenido
};

#endif  //__POINTMAP_CLASS_H