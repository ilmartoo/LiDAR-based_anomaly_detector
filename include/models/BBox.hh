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

#include <vector>

#include "armadillo"

#include "models/Point.hh"

class BBox {
   private:
    Vector delta;  ///< Delta de las dimensiones de la bounding box

   public:
    /**
     * Constructor
     */
    BBox() {}
    /**
     * Constructor
     * @param points Vector de puntos sobre los que construir la bounding box
     * @param rot Matriz de rotación a aplicar a los puntos
     */
    BBox(const std::vector<Point> &points, const arma::mat33 &rot) {
        if (points.size() > 0) {
            Point min = points[0].rotate(rot);
            Point max = min;
            for (size_t i = 1; i < points.size(); ++i) {
                Point p = points[i].rotate(rot);
                if (min.getX() > p.getX()) {
                    min.setX(p.getX());
                } else if (max.getX() < p.getX()) {
                    max.setX(p.getX());
                }
                if (min.getY() > p.getY()) {
                    min.setY(p.getY());
                } else if (max.getY() < p.getY()) {
                    max.setY(p.getY());
                }
                if (min.getZ() > p.getZ()) {
                    min.setZ(p.getZ());
                } else if (max.getZ() < p.getZ()) {
                    max.setZ(p.getZ());
                }
            }
            delta = max - min;
        }
    }
    /**
     * Constructor
     * @param points Vector de referencias a los puntos sobre los que construirá la bounding box
     * @param rot Matriz de rotación a aplicar a los puntos
     */
    BBox(const std::vector<Point *> &points, const arma::mat33 &rot) {
        if (points.size() > 0) {
            Point min = points[0]->rotate(rot);
            Point max = min;
            for (size_t i = 1; i < points.size(); ++i) {
                Point p = points[i]->rotate(rot);
                if (min.getX() > p.getX()) {
                    min.setX(p.getX());
                } else if (max.getX() < p.getX()) {
                    max.setX(p.getX());
                }
                if (min.getY() > p.getY()) {
                    min.setY(p.getY());
                } else if (max.getY() < p.getY()) {
                    max.setY(p.getY());
                }
                if (min.getZ() > p.getZ()) {
                    min.setZ(p.getZ());
                } else if (max.getZ() < p.getZ()) {
                    max.setZ(p.getZ());
                }
            }
            delta = max - min;
        }
    }
    /**
     * Constructor
     * @param delta Deltas de las dimensiones
     */
    BBox(const Vector &delta) : delta(delta) {}
    /**
     * Constructor
     * @param max Punto con las coordenadas máximas
     * @param min Punto con las coordenadas minimas
     */
    BBox(const Point &max, const Point &min) : delta(max - min) {}
    /**
     * Constructor
     * @param xradius Radio en x
     * @param yradius Radio en y
     * @param zradius Radio en z
     */
    BBox(double xradius, double yradius, double zradius) : delta(Vector(xradius, yradius, zradius)) {}

    ////// Getters
    /**
     * Obtiene los deltas de la bounding box
     * @return Deltas de la bounding box
     */
    const Vector &getDelta() const { return delta; }
    /**
     * Obtiene el delta en X
     * @return Delta en X
     */
    double getDeltaX() const { return delta.getX(); }
    /**
     * Obtiene el delta en Y
     * @return Delta en Y
     */
    double getDeltaY() const { return delta.getY(); }
    /**
     * Obtiene el delta en Z
     * @return Delta en Z
     */
    double getDeltaZ() const { return delta.getZ(); }

    ////// Operators
    /**
     * Comprueba si se es menor que una bounding box
     * @param bb Bounding box a comparar
     * @return true si el volumen de la bounding box es menor
     */
    bool operator<(const BBox &bb) const { return (delta.getX() * delta.getY() * delta.getZ()) < (bb.delta.getX() * bb.delta.getY() * bb.delta.getZ()); }
};

#endif  // BBOX_CLASS_H