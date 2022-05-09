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

#include "object_characterization/CharacterizedObject.hh"
#include "object_characterization/Model.hh"

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
     * Constructor
     * @param filenames Vector de nombre de archivos de modelos
     */
    ObjectManager(const std::vector<std::string> &filenames) : objID(0) {
        models = new std::unordered_map<std::string, Model>();
        objects = new std::unordered_map<std::string, CharacterizedObject>();

        for (const auto &f : filenames) {
            loadModel(f);
        }
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
     * @return pair con boolean a false si no se ha guardado o true y el nombre del objeto si se ha guardado correctamente
     */
    const std::pair<bool, std::string> newObject(const std::string &objname, const CharacterizedObject &obj) { return {objects->insert({objname, obj}).second, objname}; };

    /**
     * Guarda un nuevo objecto bajo un nombre generico
     * @param obj Objeto a guardar
     * @return pair con boolean a false si no se ha guardado o true y el nombre del objeto si se ha guardado correctamente
     */
    const std::pair<bool, std::string> newObject(const CharacterizedObject &obj) {
        std::string name;
        do {
            name = "object-" + std::to_string(objID++);
        } while (!objects->insert({name, obj}).second);

        return {true, name};
    }

    /**
     * Crea un nuevo modelo a partir de un objeto base
     * @param objname Nombre del objeto existente
     * @param modelname Nombre del nuevo modelo
     * @return pair con boolean a false si no se ha guardado o true y el nombre del modelo si se ha guardado correctamente
     */
    const std::pair<bool, std::string> newModel(const std::string &objname, const std::string &modelname, const ModelFace &face) {
        auto oitr = objects->find(objname);
        if (oitr != objects->end()) {
            auto mitr = models->find(modelname);
            // Existe el modelo
            if (mitr != models->end()) {
                mitr->second.setFace(face, oitr->second);
                return {true, modelname};
            }
            // Nuevo modelo
            else {
                auto p = models->insert({modelname, Model(modelname, face, oitr->second)});
                return {p.second, modelname};
            }
        }
        return {false, ""};
    }

    /**
     * Carga un modelo de un archivo
     * @param filename Archivo del modelo
     * @return pair con boolean a false si no se ha cargado o true y el modelo si se ha cargado el modelo correctamente
     */
    const std::pair<bool, std::string> loadModel(const std::string &filename) {
        auto p = Model::load(filename);
        if (p.first) {
            auto ep = models->insert({p.second.getName(), p.second});
            return {true, ep.first->first};
        }
        return {false, ""};
    }

    /**
     * Escribe el modelo al archivo especificado
     * @param filename Nombre del archivo a escribir
     * @param model Nombre del modelo a guardar
     * @return true si se ha escrito correctamente
     */
    bool writeModel(const std::string &filename, const std::string &model) {
        auto itr = models->find(model);
        if (itr != models->end()) {
            return itr->second.write(filename);
        } else {
            return false;
        }
    }

    ////// Getters
    /**
     * Obtiene la lista de modelos actualmente disponibles
     * @return Lista de modelos
     */
    const std::unordered_map<std::string, Model> &getModels() { return *models; }
    /**
     * Obtiene la lista de objetos actualmente disponibles
     * @return Lista de objetos
     */
    const std::unordered_map<std::string, CharacterizedObject> &getObjects() { return *objects; }
    /**
     * Comprueba la existencia de un modelo
     * @param model Nombre del modelo
     * @return true si existe el modelo
     */
    bool existsModel(const std::string &model) { return models->find(model) != models->end(); }
    /**
     * Comprueba la existencia de un objeto
     * @param model Nombre del objeto
     * @return true si existe el objeto
     */
    bool existsObject(const std::string &obj) { return objects->find(obj) != objects->end(); }

   private:
};

#endif  // OBJECTMANAGER_CLASS_H