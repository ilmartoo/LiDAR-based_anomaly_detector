/**
 * @file OctreeMap.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 03/05/2022
 *
 * @brief Definición e implementación del objeto OctreeMap
 *
 */

#ifndef OCTREEMAP_CLASS_H
#define OCTREEMAP_CLASS_H

#include <set>
#include <vector>
#include <string>
#include <utility>

#include "models/Octree.hh"
#include "models/Point.hh"
#include "models/LidarPoint.hh"
#include "models/Timestamp.hh"

/**
 * @brief Mapa de puntos únicos utilizando un octree como estructura de almacenaje
 */
class OctreeMap {
   private:
    std::pair<bool, Timestamp> startTime;  ///< Timestamp del primer punto
    Octree map;                            ///< Mapa de puntos
    std::set<std::string> keys;            ///< Claves de unicidad de las coordenadas
    std::vector<Point> points;             ///< Buffer de almacenaje de puntos

   public:
    /**
     * Constructor
     */
    OctreeMap() : startTime(false, Timestamp(0, 0)) {}
    /**
     * Destructor
     */
    ~OctreeMap() {}

    /**
     * Inserta un punto nuevo en el vector de puntos. Si el punto ya se encuentra en el vector, se descarta
     * @param p Punto a añadir
     */
    void insert(const LidarPoint &p) {
        if (keys.insert(p.ID()).second) {
            points.push_back(p);
        }
    }

    /**
     * Crea el octree con los puntos del vector
     */
    void buildOctree() {
        map = Octree(points);
    }

    ////// Setters
    /**
     * Establece el timestamp inicial
     * @param t Timestamp inicial
     */
    void setStartTime(const Timestamp &t) {
        startTime = {true, t};
    }

    ////// Getters
    /**
     * Devuelve un tipo de dato pair con un bool estableciendo si el timestamp es valido o no
     * @return pair de bool y Timestamp siendo false si no hay puntos o true si el timestamp es válido
     */
    const std::pair<bool, Timestamp> &getStartTime() const { return startTime; }
    /**
     * Devuelve el octree de puntos
     * @return Mapa de puntos si se ha construido anteriormente
     */
    const Octree &getMap() const { return map; }
    /**
     * Devuelve el vector de puntos
     * @return Vector de puntos del objeto
     */
    const std::vector<Point> &getPoints() const { return points; }
};

#endif  // OCTREEMAP_CLASS_H