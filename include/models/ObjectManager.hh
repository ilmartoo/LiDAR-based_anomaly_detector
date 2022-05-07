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
#include <map>
#include <stdint.h>

#include "models/CharacterizedObject.hh"

/**
 * Manager de modelos de objetos
 */
class ObjectManager {
   private:
    std::map<std::string, Model> *models;
    std::map<std::string, CharacterizedObject> *objects;
    static inline uint32_t objID = 0;

   public:
    /**
     * Constructor
     */
    ObjectManager() {
        models = new std::map<std::string, Model>();
        objects = new std::map<std::string, CharacterizedObject>();
    }
    /**
     * Constructor
     * @param modelfile Path del archivo de modelos
     */
    ObjectManager(const std::string &modelfile) {
        models = new std::map<std::string, Model>();
        objects = new std::map<std::string, CharacterizedObject>();

        // RETRIEVE MODELS LOGIC
    }
    /**
     * Destructor
     */
    ~ObjectManager() {
        delete models;
        delete objects;
    }

    /**
     * Guarda un nuevo objecto
     * @param objname Nombre del objeto
     * @param obj Objeto a guardar
     * @return true si no existe un objeto bajo el mismo nombre y se ha insertado correctamente
     */
    bool createObject(const std::string &objname, const CharacterizedObject &obj) { return objects->emplace(objname, obj).second; };

    /**
     *
     * @param objname
     * @return true si existe el objecto con el nombre especificado y no exite un modelo con el nombre deseado
     */
    bool newModel(const std::string &objname, const std::string &modelname) {
        auto itr = objects->find(objname);
        if (itr != objects->end()) {
            return models->emplace(modelname, itr->second).second;
        }
        return false;
    }

    ////// Getters
    /**
     * Obtiene la lista de modelos actualmente disponibles
     * @return Lista de modelos
     */
    const std::map<std::string, Model> &getModels() { return *models; }
    /**
     * Obtiene la lista de objetos actualmente disponibles
     * @return Lista de objetos
     */
    const std::map<std::string, CharacterizedObject> &getObjects() { return *objects; }

   private:
};

#endif  // OBJECTMANAGER_CLASS_H