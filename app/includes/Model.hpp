#ifndef __MODEL_CLASS_H
#define __MODEL_CLASS_H

#include <string>

#include "CharacteristicsVector.hpp"

class Model {
   public:
    Model(std::string name, CharacteristicsVector cv);

    /**
     * Recupera el vector de características del modelo
     * @return Vector de características del modelo
     */
    CharacteristicsVector getCharacteristicsVector() {
        return this->cv;
    };

    /**
     * Recupera el nombre del modelo
     * @return Nombre del modelo
     */
    std::string getName() {
        return this->name;
    };

   private:
    int id;
    std::string name;
    CharacteristicsVector cv;
};

#endif  //__MODEL_CLASS_H