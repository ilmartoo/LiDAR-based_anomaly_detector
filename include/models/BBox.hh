/**
 * @file BBox.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 10/05/2022
 *
 * Definición e implementación de la clase BBox
 *
 */

#ifndef BBOX_CLASS_H
#define BBOX_CLASS_H

#include "models/Octree.hh"
#include "models/Point.hh"

class BBox : public Box {
   private:
    Point center;  ///< Centro de la bounding box
    Vector radii;  ///< Diagonales de la bounding box

   public:
    /**
     * Constructor
     */
    BBox() : Box(Point(), Vector()), center(), radii() {}
    /**
     * Constructor
     * @param center Centro de la bounding box
     * @param radii Radio de las dimensiones
     */
    BBox(const Point &center, const Vector &radii) : Box(center, radii), center(center), radii(radii) {}
    /**
     * Constructor
     * @param center Centro de la bounding box
     * @param max Punto con las coordenadas máximas
     * @param min Punto con las coordenadas minimas
     */
    BBox(const Point &center, const Point &max, const Point &min) : Box(center, max - min), center(center), radii(max - min) {}
    /**
     * Constructor
     * @param center Centro de la bounding box
     * @param xradius Radio en x
     * @param yradius Radio en y
     * @param zradius Radio en z
     */
    BBox(const Point &center, double xradius, double yradius, double zradius) : Box(center, Vector(xradius, yradius, zradius)), center(center), radii(Vector(xradius, yradius, zradius)) {}
    /**
     * Constructor
     * @param box bounding box
     */
    BBox(const Box &box) : Box(box) {
        radii = Vector(box.max() - box.min());
        center = radii / 2;
    }

    ////// Getters
    /**
     * Obtiene el centro de la bounding box
     * @return Centro de la bounding box
     */
    const Point &getCenter() const { return center; }
    /**
     * Obtiene los radios de la bounding box
     * @return Radios de la bounding box
     */
    const Vector &getRadii() const { return radii; }
};

#endif  // BBOX_CLASS_H