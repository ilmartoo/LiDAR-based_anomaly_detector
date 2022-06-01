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
#include <unordered_map>
#include <stdint.h>
#include <utility>

#include "object_characterization/CharacterizedObject.hh"

/**
 * Manager de modelos de objetos
 */
class ObjectManager {
   private:
    std::unordered_map<std::string, Model> *models;
    std::unordered_map<std::string, CharacterizedObject> *objects;
    uint32_t objID;

   public:
    /**
     * Constructor
     */
    ObjectManager() : objID(0) {
        models = new std::unordered_map<std::string, Model>();
        objects = new std::unordered_map<std::string, CharacterizedObject>();
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
     * @return true si se ha guardado correctamente
     */
    bool newObject(const std::string &objname, const CharacterizedObject &obj) { return objects->insert({objname, obj}).second; };

    /**
     * Guarda un nuevo objecto bajo un nombre generico
     * @param obj Objeto a guardar
     * @return pair con boolean a false si no se ha guardado o true y el nombre del objeto si se ha guardado correctamente
     */
    std::pair<bool, std::string> newObject(const CharacterizedObject &obj) {
        std::string name;
        do {
            name = "object-" + std::to_string(objID++);
        } while (!newObject(name, obj));

        return {true, name};
    }

    /**
     * Carga un objeto de un archivo
     * @param filename Archivo del objeto
     * @return true se se ha cargado correctamente el objeto
     */
    bool loadObject(const std::string &filename, const std::string &object) {
        if (objects->find(object) == objects->end()) {
            auto m = CharacterizedObject::load(filename);
            if (m.first) {
                return objects->insert({object, m.second}).second;
            }
        }
        return false;
    }

    /**
     * Escribe el objeto al archivo especificado
     * @param filename Nombre del archivo a escribir
     * @param object Nombre del objeto a guardar
     * @return true si se ha escrito correctamente
     */
    bool writeObject(const std::string &filename, const std::string &object) const {
        auto itr = objects->find(object);
        if (itr != objects->end()) {
            return itr->second.write(filename);
        } else {
            return false;
        }
    }

    /**
     * Escribe el objeto al archivo csv especificado
     * @param filename Nombre del archivo a escribir
     * @param object Nombre del objeto a guardar
     * @return true si se ha escrito correctamente
     */
    bool writeObjectCSV(const std::string &filename, const std::string &object) const {
        auto itr = objects->find(object);
        if (itr != objects->end()) {
            return itr->second.writeLivoxCSV(filename);
        } else {
            return false;
        }
    }

    /**
     * Crea un nuevo modelo a partir de un objeto base
     * @param objname Nombre del objeto existente
     * @param modelname Nombre del nuevo modelo
     * @return true si se ha guardado correctamente
     */
    bool newModel(const std::string &objname, const std::string &modelname) {
        auto oitr = objects->find(objname);
        if (oitr != objects->end()) {
            return models->insert({modelname, oitr->second}).second;
        }
        return false;
    }

    /**
     * Carga un modelo de un archivo
     * @param filename Archivo del modelo
     * @return true se se ha cargado correctamente el modelo
     */
    bool loadModel(const std::string &filename, const std::string &model) {
        if (models->find(model) == models->end()) {
            auto m = Model::load(filename);
            if (m.first) {
                return models->insert({model, m.second}).second;
            }
        }
        return false;
    }

    /**
     * Escribe el modelo al archivo especificado
     * @param filename Nombre del archivo a escribir
     * @param model Nombre del modelo a guardar
     * @return true si se ha escrito correctamente
     */
    bool writeModel(const std::string &filename, const std::string &model) const {
        auto itr = models->find(model);
        if (itr != models->end()) {
            return itr->second.write(filename);
        } else {
            return false;
        }
    }

    /**
     * Escribe el modelo al archivo csv especificado
     * @param filename Nombre del archivo a escribir
     * @param model Nombre del modelo a guardar
     * @return true si se ha escrito correctamente
     */
    bool writeModelCSV(const std::string &filename, const std::string &model) const {
        auto itr = models->find(model);
        if (itr != models->end()) {
            return itr->second.writeLivoxCSV(filename);
        } else {
            return false;
        }
    }

    ////// Getters
    /**
     * Obtiene la lista de modelos actualmente disponibles
     * @return Lista de modelos
     */
    const std::unordered_map<std::string, Model> &getModels() const { return *models; }
    /**
     * Obtiene la lista de objetos actualmente disponibles
     * @return Lista de objetos
     */
    const std::unordered_map<std::string, CharacterizedObject> &getObjects() const { return *objects; }
    /**
     * Comprueba la existencia de un modelo
     * @param model Nombre del modelo
     * @return true si existe el modelo
     */
    bool existsModel(const std::string &model) const { return models->find(model) != models->end(); }
    /**
     * Comprueba la existencia de un objeto
     * @param model Nombre del objeto
     * @return true si existe el objeto
     */
    bool existsObject(const std::string &obj) const { return objects->find(obj) != objects->end(); }
};

#endif  // OBJECTMANAGER_CLASS_H