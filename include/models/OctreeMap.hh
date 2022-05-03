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

#include <vector>

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
    ~OctreeMap() {}

	/**
	 * Inserta un punto en el vector de puntos
	 * @param p Punto a añadir
	 */
    void insert(const Point& p) { mem.push_back(p); }

	/**
	 * Crea el octree con los puntos del vector
	 */
	void buildOctree() {
		map.buildOctree(mem);
	}

    /**
     * Devuelve el octree de puntos
     * @return Mapa de puntos
     */
    const Octree& getMap() const { return map; }

   private:
    Octree map;
    std::vector<Point> mem{};
};

#endif  // OCTREEMAP_CLASS_H