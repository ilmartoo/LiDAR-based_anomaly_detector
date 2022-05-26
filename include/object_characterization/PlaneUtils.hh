/**
 * @file PlaneUtils.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 12/05/2022
 *
 * Definición de la clase PlaneUtils
 *
 */

#ifndef PLANEUTILS_CLASS_H
#define PLANEUTILS_CLASS_H

#include <vector>

#include "armadillo"

#include "models/Point.hh"
#include "models/Octree.hh"

#define NORMAL_CALCULATION_THREADS 4
#define DEG2RAD                    0.0174532925199

class PlaneUtils {
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
     * @param distance Máxima distancia a la que pueden estar los puntos para considerarse vecinos
     * @return vector de normales, siendo 0 aquellas de los puntos que no se les pudo calcular la normal
     */
    static std::vector<Vector> computeNormals(std::vector<Point> &points, double distance);

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
     * Matriz de rotación según unos ángulos dados en grados
     * @param xdeg Rotación en X en grados
     * @param ydeg Rotación en Y en grados
     * @param zdeg Rotación en Z en grados
     * @return Matriz de rotación
     */
    static arma::mat33 rotationMatrix(int xdeg, int ydeg, int zdeg);
    /**
     * Matriz de rotación según unos ángulos dados
     * @param deg Vector con los angulos de rotación en grados en cada coordenada
     * @return Matriz de rotación
     */
    static arma::mat33 rotationMatrix(const Vector &deg);

   private:
    static void computeSVD(const std::vector<Point> &points, arma::mat &U, arma::vec &s, arma::mat &V);
    static void computeSVD(const std::vector<Point *> &points, arma::mat &U, arma::vec &s, arma::mat &V);
};

#endif  // PLANEUTILS_CLASS_H