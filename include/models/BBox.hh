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
    Point min;     ///< Punto mínimo de la bounding box
    Point max;     ///< Punto máximo de la bounding box

   public:
    /**
     * Constructor
     */
    BBox() {}
    /**
     * Constructor
     * @param points Vector de puntos sobre los que construir la bounding box
     */
    BBox(const std::vector<Point> &points) {
        if (points.size() > 0) {
            min = points[0];
            max = min;
            for (size_t i = 1; i < points.size(); ++i) {
                const Point &p = points[i];
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
     * @param points Vector de referencias a los puntos sobre los que construir la bounding box
     */
    BBox(const std::vector<Point *> &points) {
        if (points.size() > 0) {
            min = *points[0];
            max = min;
            for (size_t i = 1; i < points.size(); ++i) {
                const Point &p = *points[i];
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
     * @param points Vector de puntos sobre los que construir la bounding box
     * @param rot Matriz de rotación a aplicar a los puntos
     */
    BBox(const std::vector<Point> &points, const arma::mat33 &rot) {
        if (points.size() > 0) {
            min = points[0].rotate(rot);
            max = min;
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
            min = points[0]->rotate(rot);
            max = min;
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
    BBox(const Vector &delta) : delta(delta), min(0, 0, 0), max(delta) {}
    /**
     * Constructor
     * @param max Punto con las coordenadas máximas
     * @param min Punto con las coordenadas minimas
     */
    BBox(const Point &max, const Point &min) : delta(max - min), min(min), max(max) {}
    /**
     * Constructor
     * @param xradius Radio en x
     * @param yradius Radio en y
     * @param zradius Radio en z
     */
    BBox(double xradius, double yradius, double zradius) : delta(xradius, yradius, zradius), min(0, 0, 0), max(xradius, yradius, zradius) {}
    /**
     * Destructor
     */
    ~BBox() {}

    /**
     * Calcula el volumen de la bounding box
     * @return Volumen de la bounding box
     */
    double volume() const { return delta.getX() * delta.getY() * delta.getZ(); }

    /**
     * Comprueba si la bounding box es de menor tamaño en X, Y y Z en ese orden con respecto a la bounding box pasada como parametro.
     * Metodo utilizado en la búsqueda de la mejor bouding box.
     * @param bbox BBox contra la que se comparará
     * @return true si la bounding box es mejor
     */
    bool isBetterThan(const BBox &bbox) const { return delta.getX() < bbox.delta.getX() ||
                                                       (delta.getX() == bbox.delta.getX() && delta.getY() < bbox.delta.getY()) ||
                                                       (delta.getX() == bbox.delta.getX() && delta.getY() == bbox.delta.getY() && delta.getZ() < bbox.delta.getZ()); };

    ////// Getters
    /**
     * Obtiene los deltas de la bounding box
     * @return Deltas de la bounding box
     */
    const Vector &getDelta() const { return delta; }
    /**
     * Devuelve el punto mínimo de la bounding box
     * @return Punto mínimo de la bounding box
     */
    const Point &getMin() const { return min; }
    /**
     * Devuelve el punto máximo de la bounding box
     * @return Punto máximo de la bounding box
     */
    const Point &getMax() const { return max; }
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
    bool operator<(const BBox &bb) const { return this->volume() < bb.volume(); }
};

#endif  // BBOX_CLASS_H