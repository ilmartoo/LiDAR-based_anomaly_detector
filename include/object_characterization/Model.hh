/**
 * @file Model.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 08/05/2022
 *
 * Definición e implementación del objeto Model
 *
 */

#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <vector>
#include <fstream>

#include "object_characterization/CharacterizedObject.hh"

enum ModelFace {
    mFrontal = 0,  ///< Cara frontal
    mBack = 1,     ///< Cara trasera
    mLSide = 2,    ///< Cara lateral izquierda
    mRSide = 3,    ///< Cara lateral derecha
    mCeil = 4,     ///< Cara de arriba
    mBottom = 5    ///< Cara de abajo
};

class Model {
   private:
    std::string name;
    std::vector<bool> hasFace;
    std::vector<CharacterizedObject> faces;

   public:
    /**
     * Constructor
     */
    Model() {
        for (int i = 0; i < 6; ++i) {
            hasFace.push_back(false);
        }
        faces.reserve(6);
    }
    /**
     * Constructor
     * @param name Nombre del modelo
     */
    Model(const std::string &name) : name(name) {
        for (int i = 0; i < 6; ++i) {
            hasFace.push_back(false);
        }
        faces.reserve(6);
    }
    /**
     * Constructor
     * @param name Nombre del modelo
     * @param face Cara del modelo
     * @param obj Caracterización de la cara
     */
    Model(const std::string &name, const ModelFace &face, const CharacterizedObject &obj) : name(name) {
        for (int i = 0; i < 6; ++i) {
            hasFace.push_back(false);
        }
        faces.reserve(6);

        faces[face] = obj;
        hasFace[face] = true;
    }
    /**
     * Constructor
     * @param name Nombre del modelo
     * @param faces Vector con las caras del modelo
     */
    Model(const std::string &name, const std::vector<CharacterizedObject> &modelFaces) : name(name) {
        for (int i = 0; i < 6; ++i) {
            hasFace.push_back(false);
        }
        faces.reserve(6);

        int items = faces.size() <= 6 ? faces.size() : 6;
        for (int i = 0; i < items; ++i) {
            faces[i] = modelFaces[i];
            hasFace[i] = true;
        }
    }
    /**
     * Destructor
     */
    ~Model() {}

    /**
     * Carga un modelo de un archivo
     * @param filename Nombre del archivo
     * @return boolean a false si no se ha podido cargar el modelo o true y el modelo si se ha cargado correctamente
     */
    static std::pair<bool, Model> load(const std::string &filename) {
        std::ifstream infile(filename);
        if (infile.is_open()) {
            Model model;

            size_t slen;
            infile.read((char *)&slen, sizeof(size_t));  // Longitud del nombre del modelo

            char nameBuff[slen + 1];
            nameBuff[slen] = '\0';
            infile.read(nameBuff, slen);  // Nombre del modelo

            model.setName(std::string(nameBuff));

            int nfaces;
            ModelFace face;
            char obj[sizeof(CharacterizedObject)];

            infile.read((char *)&nfaces, sizeof(int));  // Total de caras
            for (int i = 0; i < nfaces; ++i) {
                infile.read((char *)&face, sizeof(ModelFace));  // Tipo de la cara
                infile.read(obj, sizeof(CharacterizedObject));  // BBox de la cara

                model.setFace(face, *((CharacterizedObject *)&obj));
            }

            infile.close();

            return {true, model};

        } else {
            return {false, {}};
        }
    }

    /**
     * Carga un grupo de modelos de un grupo de archivos
     * @param filenames Nombres de los archivos
     * @return Vector de modelos cargados
     */
    static std::vector<Model> load(const std::vector<std::string> &filenames) {
        std::vector<Model> models;
        for (auto &f : filenames) {
            auto p = Model::load(f);
            if (p.first) {
                models.push_back(p.second);
            }
        }
        return models;
    }

    /**
     * Guarda un modelo en un archivo
     * @param filename Nombre del archivo
     * @return true si se ha guardado correctamente
     */
    bool write(const std::string &filename) {
        std::ofstream outfile(filename);
        if (outfile.is_open()) {
            // Cuenta de número de caras
            int nfaces = 0;
            for (const auto &f : hasFace) {
                if (f) {
                    ++nfaces;
                }
            }

            size_t slen = name.size();
            outfile.write((char *)&slen, sizeof(size_t));  // Logitud del nombre del modelo
            outfile.write(name.c_str(), slen);             // Nombre del modelo

            outfile.write((char *)&nfaces, sizeof(int));  // Numero de caras del modelo

            // Guardado de caras
            for (int i = 0; nfaces > 0 && i < 6; ++i) {
                if (hasFace[i]) {
                    outfile.write((char *)&i, sizeof(ModelFace));                   // Tipo de la cara
                    outfile.write((char *)&faces[i], sizeof(CharacterizedObject));  // BBox de la cara

                    --nfaces;
                }
            }

            outfile.close();

            return true;

        } else {
            return false;
        }
    }

    /**
     * Recupera una cara del modelo
     * @param face Cara a recuperar
     * @return Cara caracterizada
     */
    const CharacterizedObject &operator[](const ModelFace &face) const { return faces[face]; }

    ////// Getters

    /**
     * Obtiene el nombre del modelo
     * @return Nombre del modelo
     */
    const std::string &getName() const { return name; }

    /**
     * Obtiene las caras del modelo
     * @return Vector del caras del modelo
     */
    const std::vector<CharacterizedObject> &getFaces() const { return faces; }
    /**
     * Obtiene la cara especificada del modelo
     * @param face Cara a obtener
     * @return const CharacterizedObject&
     */
    const CharacterizedObject &getFace(const ModelFace &face) const { return faces[face]; }
    /**
     * Obtiene la cara frontal
     * @return Caracterización de la cara
     */
    const CharacterizedObject &getFrontal() const { return faces[mFrontal]; }
    /**
     * Obtiene la cara trasera
     * @return Caracterización de la cara
     */
    const CharacterizedObject &getBack() const { return faces[mBack]; }
    /**
     * Obtiene la cara lateral izquierda
     * @return Caracterización de la cara
     */
    const CharacterizedObject &getLSide() const { return faces[mLSide]; }
    /**
     * Obtiene la cara lateral derecha
     * @return Caracterización de la cara
     */
    const CharacterizedObject &getRSide() const { return faces[mRSide]; }
    /**
     * Obtiene la cara de arriba
     * @return Caracterización de la cara
     */
    const CharacterizedObject &getCeil() const { return faces[mCeil]; }
    /**
     * Obtiene la cara de abajo
     * @return Caracterización de la cara
     */
    const CharacterizedObject &getBottom() const { return faces[mBottom]; }

    /**
     * Devuelve un vector de booleanos para especificar que caras tiene el modelo
     * @return Vector de booleanos de caras
     */
    const std::vector<bool> &isFaces() const { return hasFace; }
    /**
     * Obtiene un booleano sobre si el modelo contiene la cara especificada
     * @param face Cara de modelo a comprobar
     * @return true si contiene la cara
     */
    const bool isFace(const ModelFace &face) const { return hasFace[face]; }
    /**
     * Obtiene si contiene cara frontal
     * @return true si contiene la cara
     */
    const bool isFrontal() const { return hasFace[mFrontal]; }
    /**
     * Obtiene si contiene cara trasera
     * @return true si contiene la cara
     */
    const bool isBack() const { return hasFace[mBack]; }
    /**
     * Obtiene si contiene cara lateral izquierda
     * @return true si contiene la cara
     */
    const bool isLSide() const { return hasFace[mLSide]; }
    /**
     * Obtiene si contiene cara lateral derecha
     * @return true si contiene la cara
     */
    const bool isRSide() const { return hasFace[mRSide]; }
    /**
     * Obtiene si contiene cara de arriba
     * @return true si contiene la cara
     */
    const bool isCeil() const { return hasFace[mCeil]; }
    /**
     * Obtiene si contiene cara de abajo
     * @return true si contiene la cara
     */
    const bool isBottom() const { return hasFace[mBottom]; }

    ////// Setters
    /**
     * Establecemos el nombre del modelo
     * @param name Nuevo nombre del modelo
     */
    void setName(const std::string &name) { this->name = name; }

    /**
     * Añade las caras especificadas al modelo, empezando por la cara frontal segun el orden en ModelFace hasta quedarse si caras o llegar al máximo de caras
     * @param modelFaces Vector de nuevas caras del modelo
     */
    void setFaces(const std::vector<CharacterizedObject> &modelFaces) {
        int items = faces.size() <= 6 ? faces.size() : 6;
        for (int i = 0; i < items; ++i) {
            faces[i] = modelFaces[i];
            hasFace[i] = true;
        }
    }
    /**
     * Añade una cara al objeto
     * @param face Cara a añadir
     * @param obj Caracterización de la cara
     */
    void setFace(const ModelFace &face, const CharacterizedObject &obj) {
        hasFace[face] = true;
        faces[face] = obj;
    }
    /**
     * Añade la cara frontal
     * @param obj Caracterización de la cara
     */
    void setFrontal(const CharacterizedObject &obj) { setFace(mFrontal, obj); }
    /**
     * Añade la cara trasera
     * @param obj Caracterización de la cara
     */
    void setBack(const CharacterizedObject &obj) { setFace(mBack, obj); }
    /**
     * Añade la cara lateral izquierda
     * @param obj Caracterización de la cara
     */
    void setLSide(const CharacterizedObject &obj) { setFace(mLSide, obj); }
    /**
     * Añade la cara lateral derecha
     * @param obj Caracterización de la cara
     */
    void setRSide(const CharacterizedObject &obj) { setFace(mRSide, obj); }
    /**
     * Añade la cara de arriba
     * @param obj Caracterización de la cara
     */
    void setCeil(const CharacterizedObject &obj) { setFace(mCeil, obj); }
    /**
     * Añade la cara de abajo
     * @param obj Caracterización de la cara
     */
    void setBottom(const CharacterizedObject &obj) { setFace(mBottom, obj); }
};

#endif  // MODEL_CLASS_H