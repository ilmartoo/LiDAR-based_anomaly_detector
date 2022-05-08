/**
 * @file CharacterizedObject.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 20/03/2022
 *
 * Definición del objeto CharacterizedObject
 *
 */

#ifndef CHARACTERIZEDOBJECT_CLASS_H
#define CHARACTERIZEDOBJECT_CLASS_H

#include <string>

#include "models/OctreeMap.hh"
#include "models/Point.hh"

/**
 * Caracteristicas de un objecto caracterizado
 */
class CharacterizedObject {
   private:
    Box bbox;  ///< Bounding box

   public:
    /**
     * Constructor
     * @param om Mapa de puntos del objeto
     */
    CharacterizedObject(const OctreeMap& om) : bbox(om.getMap().getCenter(),
                                                    {om.getMap().getMax().getX() - om.getMap().getMin().getX(),
                                                     om.getMap().getMax().getY() - om.getMap().getMin().getY(),
                                                     om.getMap().getMax().getZ() - om.getMap().getMin().getZ()}) {}
    /**
     * Constructor
     * @param bbox Bounding box
     */
    CharacterizedObject(const Box& bbox) : bbox(bbox) {}

    /**
     * Destructor
     */
    ~CharacterizedObject() {}

    /**
     * Obtiene la bounding box
     * @return Bounding box del objeto
     */
    const Box& getBBox() const { return bbox; }
};

#endif  // CHARACTERIZEDOBJECT_CLASS_H