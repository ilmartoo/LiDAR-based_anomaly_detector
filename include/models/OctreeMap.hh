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
    OctreeMap() {}

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
    }

    /**
     * Crea el octree con los puntos del vector
     */
    void buildOctree() { map = new Octree(mem); }

    /**
     * Devuelve el octree de puntos
     * @return Referencia al mapa de puntos si se ha construido anteriormente, sino nulo
     */
    const Octree *getMap() const { return map; }

   private:
    Octree *map;
    std::set<std::string> keys;
    std::vector<Point> mem;
};

#endif  // OCTREEMAP_CLASS_H