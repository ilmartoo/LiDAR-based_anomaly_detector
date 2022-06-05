/**
 * @file Geometry.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 12/05/2022
 *
 * @brief Definición de la clase Geometry
 *
 */

#ifndef GEOMETRY_CLASS_H
#define GEOMETRY_CLASS_H

#include <vector>
#include <utility>

#include "armadillo"

#include "models/Point.hh"
#include "models/Octree.hh"
#include "models/BBox.hh"

/**
 * @brief Clase utilizada como almacén de métodos geométricos y espaciales
 */
class Geometry {
   public:
    /**
     * Calcula el centroide de los puntos
     * @param points Puntos de los que se calculará el centroide
     * @return Centroide
     */
    static Point computeCentroid(const std::vector<Point> &points);
    /**
     * Calcula el centroide de los puntos
     * @param points Referencias a los puntos de los que se calculará el centroide
     * @return Centroide
     */
    static Point computeCentroid(const std::vector<Point *> &points);

    /**
     * Obtiene la normal de un plano
     * @param points Puntos del plano sobre los que se calculará la normal
     * @return Vector normal
     */
    static Vector computeNormal(const std::vector<Point> &points);
    /**
     * Obtiene la normal de un plano
     * @param points Referencias a los puntos del plano sobre los que se calculará la normal
     * @return Vector normal
     */
    static Vector computeNormal(const std::vector<Point *> &points);

    /**
     * Calculo de normales de un grupo de puntos
     * @param points Puntos de los que se calcularán las normales
     * @param map Octree con los puntos del vector de puntos
     * @param distance Máxima distancia a la que pueden estar los puntos para considerarse vecinos
     * @return vector de normales, siendo 0 aquellas de los puntos que no se les pudo calcular la normal
     */
    static std::vector<Vector> computeNormals(std::vector<Point> &points, const Octree &map, double distance);

    /**
     * Obtiene el plano con el vector normal especificado y que pasa sobre el centroide
     * @param vnormal Vector normal del plano
     * @param centroid Centroide sobre el que pasa el plano
     * @return arma::vec
     */
    static arma::vec4 computePlane(const Vector &vnormal, const Point &centroid);
    /**
     * Calcula el plano de un conjunto de puntos
     * @param points Puntos pertenecientes al plano
     * @return Plano
     */
    static arma::vec4 computePlane(const std::vector<Point> &points);
    /**
     * Calcula el plano de un conjunto de puntos
     * @param points Referencias a los puntos pertenecientes al plano
     * @return Plano
     */
    static arma::vec4 computePlane(const std::vector<Point *> &points);

    /**
     * Calcula la media de un vector de puntos
     * @param points Puntos sobre los que calcular la media
     * @return Punto medio del vector
     */
    static Point mean(const std::vector<Point> &points);
    /**
     * Calcula la media de un vector de puntos
     * @param points Referencias a los puntos sobre los que calcular la media
     * @return Punto medio del vector
     */
    static Point mean(const std::vector<Point *> &points);

    /**
     * Matriz de rotación según unos ángulos dados en grados
     * @param xdeg Rotación en X en grados
     * @param ydeg Rotación en Y en grados
     * @param zdeg Rotación en Z en grados
     * @return Matriz de rotación
     */
    static arma::mat33 rotationMatrix(double xdeg, double ydeg, double zdeg);
    /**
     * Matriz de rotación según unos ángulos dados
     * @param deg Vector con los angulos de rotación en grados en cada coordenada
     * @return Matriz de rotación
     */
    static arma::mat33 rotationMatrix(const Vector &deg);

    /**
     * Rota los puntos para buscar la bounding box de mínimo volumen que los englobe:
     * todos los puntos se rotarán según los angulos que den como resultado la bounding box de mínimo volumen
     * y posteriormente serán transladados a (0,0,0)
     * @param points Vector de referencias a los puntos
     * @return Bounding box de mínimo volumen y vector de los ángulos de rotación utilizados en grados
     */
    static std::pair<BBox, Vector> minimumBBoxRotTrans(std::vector<Point> &points);

    /**
     * Obtiene la bounding box de mínimo volumen que engloba los puntos
     * @param points Vector de puntos
     * @return Bounding box de mínimo volumen y vector de los ángulos de rotación utilizados en grados
     */
    static std::pair<BBox, Vector> minimumBBox(const std::vector<Point> &points);

    /**
     * Obtiene las bounding box de mínimo volumen que engloban a cada vector de referencias a puntos (caras de un objeto)
     * @param points Vector de vectores de referencias a puntos
     * @return Vector de bounding boxes de mínimo volumen y vectores de los ángulos de rotación utilizados en grados
     */
    static std::vector<std::pair<BBox, Vector>> minimumBBoxes(const std::vector<std::vector<Point *>> &points);

   private:
    static void computeSVD(const std::vector<Point> &points, arma::mat &U, arma::vec &s, arma::mat &V);
    static void computeSVD(const std::vector<Point *> &points, arma::mat &U, arma::vec &s, arma::mat &V);
    // Obtención de la rotación necesaria adicional para obtener la bbox cúbica de menor largo, ancho y alto, en ese orden
    static std::pair<BBox, Vector> bestOrientation(const BBox &bbox);
    // Comparación de bounding boxes para comprobar cual tiene una mejor orientación
    static bool betterDimensions(const Vector &newDim, const Vector &oldDim);
};

#endif  // GEOMETRY_CLASS_H