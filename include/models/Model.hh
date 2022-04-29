/**
 * @file Model.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 20/03/2022
 *
 * Definición del objeto Model
 *
 */

#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <string>

#include "models/CharacteristicsVector.hh"

#include "logging/debug.hh"

/**
 * Modelo de un objeto
 */
class Model {
   public:
    /**
     * Constructor del objeto Model
     * @param name Nombre del modelo
     * @param cv Vector de características
     */
    Model(std::string name, CharacteristicsVector cv);

    /**
     * Recupera el vector de características del modelo
     * @return Vector de características del modelo
     */
    CharacteristicsVector getCharacteristicsVector() { return this->cv; };

    /**
     * Recupera el nombre del modelo
     * @return Nombre del modelo
     */
    std::string getName() { return this->name; };

   private:
    int id;
    std::string name;
    CharacteristicsVector cv;
};

#endif  //MODEL_CLASS_H