/**
 * @file OctreeMap.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 03/05/2022
 *
 * Definición e implementación del objeto OctreeMap
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
    void insert(const Point &p) {
        auto isNew = keys.insert(std::to_string(p.getX()) + std::to_string(p.getY()) + std::to_string(p.getZ()));
        if (isNew.second) {
            points.push_back(p);
        }
        if (!startTime.first) {
            startTime.first = true;
            startTime.second = p.getTimestamp();
        }
    }

    /**
     * Crea el octree con los puntos del vector
     */
    void buildOctree() {
        map = Octree(points);
    }

    /**
     * Borra el mapa de puntos al completo
     */
    void clear() {
        startTime.first = false;

        map = Octree();
        points.clear();
        keys.clear();
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