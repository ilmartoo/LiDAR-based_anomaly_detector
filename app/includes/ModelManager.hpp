#ifndef __MODELMANAGER_CLASS_H
#define __MODELMANAGER_CLASS_H

#include <string>
#include <list>

#include "Model.hpp"
#include "CharacteristicsVector.hpp"

class ModelManager {
   public:
    /**
     * Crea un nuevo modelo
     * @param name Nombre del modelo
     * @param cv Vector de características
     * @return Devuelve el nuevo modelo creado
     */
    static Model createModel(std::string name, CharacteristicsVector cv);

    /**
     * Obtiene la lista de modelos actualmente disponibles
     * @return Lista de modelos
     */
    static std::list<Model> listModels();

   private:
    static std::list<Model> modelos = new std::list<Model>();
};

#endif  //__MODELMANAGER_CLASS_H