/**
 * @file ObjectManager.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 06/05/2022
 * 
 * Definición e implementación del objeto ObjectManager
 * 
 */

#ifndef OBJECTMANAGER_CLASS_H
#define OBJECTMANAGER_CLASS_H

#include <string>
#include <list>

#include "models/CharacterizedObject.hh"
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

#endif  // OBJECTMANAGER_CLASS_H