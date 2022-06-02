/**
 * @file Face.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 29/05/2022
 *
 * @brief Definición e implementación del objeto Face
 *
 */

#ifndef FACE_CLASS_H
#define FACE_CLASS_H

#include <vector>

#include "models/BBox.hh"
#include "models/Point.hh"

/**
 * @brief Representación de una cara de un objeto
 */
class Face {
   private:
    std::vector<Point> points;  ///< Puntos de la cara
    Vector normal;              ///< Vector normal de la cara
    BBox minBBox;               ///< Bounding box mínima que engloba a los puntos de la cara
    Vector minBBoxRotAngles;    ///< Ángulos de rotación en grados que dan como resultado la bounding box mínima

   public:
    /**
     * Constructor
     */
    Face() {}
    /**
     * Constructor
     * @param points Puntos de la cara
     * @param normal Normal de la cara
     * @param minBBox Bounding box mínima que engloba a los puntos de la cara
     * @param minBBoxRotAngles Ángulos de rotación en grados que dan como resultado la bounding box mínima
     */
    Face(const std::vector<Point> &points, const Vector &normal, const BBox &minBBox, const Vector &minBBoxRotAngles)
        : points(points),
          normal(normal),
          minBBox(minBBox),
          minBBoxRotAngles(minBBoxRotAngles) {}
    /**
     * Destructor
     */
    ~Face() {}

    ////// Getters
    /**
     * Devuelve los puntos de la cara
     * @return Puntos de la cara
     */
    const std::vector<Point> &getPoints() const { return points; }
    /**
     * Devuelve la normal de la cara
     * @return Normal de la cara
     */
    const Vector &getNormal() const { return normal; }
    /**
     * Devuelve la bounding box mínima que engloba a los puntos de la cara
     * @return Bounding box mínima
     */
    const BBox &getMinBBox() const { return minBBox; }
    /**
     * Devuelve los ángulos de rotación en grados que dan como resultado la bounding box mínima
     * @return Ángulos de rotación
     */
    const Vector &getMinBBoxRotAngles() const { return minBBoxRotAngles; }
};

#endif  // FACE_CLASS_H