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
    Vector diags;  ///< Diagonales de la bounding box

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
            diags = max - min;
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
            diags = max - min;
        }
    }
    /**
     * Constructor
     * @param center Centro de la bounding box
     * @param radii Radio de las dimensiones
     */
    BBox(const Vector &diags) : diags(diags) {}
    /**
     * Constructor
     * @param center Centro de la bounding box
     * @param max Punto con las coordenadas máximas
     * @param min Punto con las coordenadas minimas
     */
    BBox(const Point &max, const Point &min) : diags(max - min) {}
    /**
     * Constructor
     * @param center Centro de la bounding box
     * @param xradius Radio en x
     * @param yradius Radio en y
     * @param zradius Radio en z
     */
    BBox(double xradius, double yradius, double zradius) : diags(Vector(xradius, yradius, zradius)) {}

    ////// Getters
    /**
     * Obtiene las diagonales de la bounding box
     * @return Diagonales de la bounding box
     */
    const Vector &getDiags() const { return diags; }
    /**
     * Obtiene el delta en X
     * @return Delta en X
     */
    double getDeltaX() const { return diags.getX(); }
    /**
     * Obtiene el delta en Y
     * @return Delta en Y
     */
    double getDeltaY() const { return diags.getY(); }
    /**
     * Obtiene el delta en Z
     * @return Delta en Z
     */
    double getDeltaZ() const { return diags.getZ(); }

    ////// Operators
    
    /**
     * Comprueba si se es menor que una bounding box
     * @param bb Bounding box a comparar
     * @return true si el volumen del cuadrado es menor
     */
    bool operator<(const BBox &bb) const { return (diags.getX() * diags.getY() * diags.getZ()) < (bb.diags.getX() * bb.diags.getY() * bb.diags.getZ()); }
};

#endif  // BBOX_CLASS_H