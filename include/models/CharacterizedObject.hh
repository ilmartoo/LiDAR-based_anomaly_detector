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
    std::string name;

   public:
    /**
     * Constructor del objeto CharacterizedObject
     * @param name Nombre del objeto
     * @param map Mapa de puntos
     */
    CharacterizedObject(const std::string& name, const Octree& map) : name(name) {
    }

    /**
     * Recupera el nombre del modelo
     * @return Nombre del modelo
     */
    std::string getName() { return this->name; };
};

typedef CharacterizedObject Model;

#endif  // CHARACTERIZEDOBJECT_CLASS_H