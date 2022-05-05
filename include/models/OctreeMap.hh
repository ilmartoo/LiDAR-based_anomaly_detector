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

#include "models/octree.hh"
#include "models/Point.hh"

class OctreeMap {
   public:
    /**
     * Constructor
     */
    OctreeMap() : startTime(false, nullptr) {}

    /**
     * Destructor
     */
    ~OctreeMap() { delete map; }

    /**
     * Inserta un punto nuevo en el vector de puntos. Si el punto ya se encuentra en el vector, se descarta
     * @param p Punto a añadir
     */
    void insert(const Point &p) {

        auto isNew = keys.emplace(std::to_string(p.getX()) + std::to_string(p.getY()) + std::to_string(p.getZ()));
        if (isNew.second) {
            mem.push_back(p);
        }
        if (!startTime.first) {
            startTime.first = true;
            startTime.second = const_cast<Timestamp *>(&mem.back().getTimestamp());
        }
    }

    /**
     * Crea el octree con los puntos del vector
     */
    void buildOctree() {
        DEBUG_STDOUT("Creamos el Octree de puntos.");
        map = new Octree(mem);
    }

    /**
     * Devuelve un tipo de dato pair con un bool estableciendo si el puntero al primer timestamp es valido o no (false si el puntero es nulo).
     * @return pair de bool y referencia siendo false, nullptr si no hay puntos o true y la referencia si existe el punto.
     */
    const std::pair<bool, Timestamp *> &getStartTime() { return startTime; }

    /**
     * Devuelve el octree de puntos
     * @return Referencia al mapa de puntos si se ha construido anteriormente, sino nulo
     */
    const Octree *getMap() const { return map; }

   private:
    std::pair<bool, Timestamp *> startTime;  ///< Timestamp del primer punto
    Octree *map;                             ///< Mapa de puntos
    std::set<std::string> keys;              ///< Claves de unicidad de las coordenadas
    std::vector<Point> mem;                  ///< Buffer de almacenaje de puntos
};

#endif  // OCTREEMAP_CLASS_H