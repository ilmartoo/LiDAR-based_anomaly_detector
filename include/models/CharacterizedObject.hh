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

#include "models/Octree.hh"

/**
 * Caracteristicas de un objecto caracterizado
 */
class CharacterizedObject {
   private:
    // DATA

   public:
    /**
     * Constructor del objeto CharacterizedObject
     * @param map Mapa de puntos
     */
    CharacterizedObject(const Octree& map) {
    }
};

typedef CharacterizedObject Model;

#endif  // CHARACTERIZEDOBJECT_CLASS_H