/**
 * @file ModelManager.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 20/03/2022
 *
 * Definición del objeto ModelManager
 *
 */

#ifndef __MODELMANAGER_CLASS_H
#define __MODELMANAGER_CLASS_H

#include <string>
#include <list>

#include "models/Model.hh"
#include "models/CharacteristicsVector.hh"

/**
 * Manager de modelos de objetos
 */
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
    static const std::list<Model> &getModels() { return modelos; }

   private:
    static std::list<Model> modelos;
};

#endif  //__MODELMANAGER_CLASS_H