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

#include "models/Point.hh"
#include "models/BBox.hh"
#include "object_characterization/PlaneUtils.hh"

/**
 * Caracteristicas de un objecto caracterizado
 */
class CharacterizedObject {
   private:
    static const uint32_t clusterPointProximity = 20;   ///< Proximidad máxima (mm) de un punto hacia uno origen para pertenecer al mismo cluster
    static const uint32_t minClusterPoints = 15;        ///< Número mínimo de puntos que debe tener un cluster inicial para ser considerado
    static const uint32_t facePointProximity = 30;      ///< Proximidad máxima (mm) de un punto hacia uno origen para pertenecer a la misma cara
    static const uint32_t minFacePoints = 10;           ///< Número mínimo de puntos que debe tener una cara inicial para ser considerada
    static constexpr double normalVariance = 0.087266;  ///< Radianes máximos de separación angular entre normales para pertenecer a la misma cara

    BBox bbox;                              ///< Bounding box que mejor se adapta al objeto
    std::vector<std::vector<Point>> faces;  ///< Caras del objeto

   public:
    /**
     * Constructor
     */
    CharacterizedObject() {}
    /**
     * Destructor
     */
    ~CharacterizedObject() {}

    /**
     * Caracteriza un objecto segun un conjunto de puntos buscando clusteres de puntos y distinción de caras
     * @param points Conjunto de puntos del objeto
     * @param chrono Indica si se desea recibir mensajes de la duración del proceso
     * @return true si se ha caracterizado el objeto correctamente junto con un objecto
     * CharacterizedObject o false y un objeto vacio si no se ha podido caracterizar
     */
    static std::pair<bool, CharacterizedObject> parse(std::vector<Point>& points, bool chrono);

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
     * Guarda el objeto a un archivo en formato CSV preparado para Livox Viewer
     * @param filename Nombre del archivo
     * @return true si se ha guardado correctamente
     */
    bool writeLivoxCSV(const std::string& filename);

    /**
     * Carga un objeto de un archivo
     * @param filename Nombre del archivo
     * @return El primer elemento es true si se ha cargado correctamente y
     * false en caso contrario, siendo el segundo elemento el objeto cargado o un objeto vacío
     */
    static std::pair<bool, CharacterizedObject> load(const std::string& filename);

    ////// Getters
    /**
     * Devuelve los puntos de las caras
     * @return Vector de puntos de las caras
     */
    const std::vector<std::vector<Point>>& getFaces() const { return faces; }
    /**
     * Devuelve la bounding box del objeto
     * @return Bounding box del objeto
     */
    const BBox& getBBox() const { return bbox; }

   private:
    /**
     * Constructor
     * @param bbox Bounding box
     * @param faces Vector de caras
     */
    CharacterizedObject(const BBox& bbox, const std::vector<std::vector<Point>>& faces) : bbox(bbox), faces(faces) {}
};

typedef CharacterizedObject Model;  ///< Definición de los modelos

#endif  // CHARACTERIZEDOBJECT_CLASS_H