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
#include <vector>
#include <map>
#include <utility>

#include "models/OctreeMap.hh"
#include "models/Point.hh"
#include "models/BBox.hh"

/**
 * Caracteristicas de un objecto caracterizado
 */
class CharacterizedObject {
   private:
    static const uint32_t proximityThreshold = 80;  ///< Proximidad máxima (mm) de un punto hacia uno origen para participar en el cálculo de su normal
    static constexpr double normalThreshold = 0.2;  ///< Diferencia máxima entre normales para pertenecer a la misma cara
    BBox bbox;                                      ///< Bounding box
    std::map<Vector, std::vector<Point>> faces;     ///< Caras del objeto

   public:
    /**
     * Constructor
     */
    CharacterizedObject() {}
    /**
     * Constructor
     * @param points Puntos del objeto
     * @param chrono Comunica si se debe medir el tiempo que tarda en crearse el objeto
     */
    CharacterizedObject(const std::vector<Point>& points, bool chrono);
    /**
     * Constructor
     * @param bbox Bounding box
     */
    CharacterizedObject(const Box& bbox, const std::map<Vector, std::vector<Point>>& faces) : bbox(bbox), faces(faces) {}
    /**
     * Destructor
     */
    ~CharacterizedObject() {}

    /**
     * Devuelve el número de caras del objeto
     * @return Número de caras del objeto
     */
    int numFaces() const { return faces.size(); }

    /**
     * Guarda el objeto a un archivo
     * @param filename Nombre del archivo
     * @return true si se ha guardado correctamente
     */
    bool write(const std::string& filename);

    /**
     * Carga un objeto de un archivo
     * @param filename Nombre del archivo
     * @return El primer elemento es true si se ha cargado correctamente y
     * false en caso contrario, siendo el segundo elemento el objeto cargado o un objeto vacío
     */
    static std::pair<bool, CharacterizedObject> load(const std::string& filename);

    ////// Getters
    /**
     * Obtiene la bounding box
     * @return Bounding box del objeto
     */
    const BBox& getBBox() const { return bbox; }
    /**
     * Devuelve el mapa de caras y puntos del objeto
     * @return Mapa de caras y puntos
     */
    const std::map<Vector, std::vector<Point>>& getFaces() const { return faces; }
};

typedef CharacterizedObject Model;  ///< Definición de los modelos

#endif  // CHARACTERIZEDOBJECT_CLASS_H