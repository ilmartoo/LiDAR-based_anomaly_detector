/**
 * @file DBScan.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 18/05/2022
 *
 * @brief Definición del algoritmo DBScan
 *
 */

#ifndef DBSCAN_CLASS_H
#define DBSCAN_CLASS_H

#include <vector>
#include <cmath>
#include <utility>

#include "models/Point.hh"

/**
 * @brief Implementación del algoritmo DBSCAN para la búsqueda de clusters y caras de un objeto
 */
class DBScan {
   public:
   /**
    * Ejecuta el algoritmo de DBScan estableciendo los clusterIDs correspondientes cada punto del vector de puntos
    * @param points Vector de puntos sobre los cuales se realizará la distinción de clusteres
    * @return Vector de clusteres representados en forma de vectores de indices de los puntos pertenecientes al cluster
    */
    static std::vector<std::vector<size_t>> clusters(std::vector<Point> &points);

    /**
    * Ejecuta el algoritmo de DBScan estableciendo los clusterIDs correspondientes cada punto del vector de puntos según sus normales
    * @param points Vector de puntos sobre los cuales se realizará la distinción de caras
    * @return Vector de caras representadas en forma de vectores de indices de los puntos pertenecientes a la cara
    */
    static std::vector<std::vector<size_t>> normals(std::vector<Point> &points);

   private:
	// Expande un cluster a partir de un centroide y el ID especificado junto con las variables limitantes especificadas
    static std::pair<bool, std::vector<size_t>> expandCluster(Point &centroid, int clusterID, std::vector<Point> &points, const Octree &map);
    // Calcula el indice de los puntos pertenecientes al cluster según un centroide dado
    static std::pair<size_t, std::vector<size_t>> centroidNeighbours(const Point &centroid, const std::vector<Point> &points, const Octree &map);

    // Expande una cara a partir de un centroide y el ID especificado junto con las variables limitantes especificadas
    static std::pair<bool, std::vector<size_t>> expandNormalCluster(size_t centroid, int clusterID, std::vector<Point> &points, const std::vector<Vector> &normals, const Octree &map);
    // Calcula el indice de los puntos pertenecientes a la cara según una normal dada
    static std::pair<size_t, std::vector<size_t>> centroidNormalNeighbours(size_t centroid, const Vector &meanNormal, const std::vector<Point> &points, const std::vector<Vector> &normals, const Octree &map);
};

#endif  // DBSCAN_CLASS_H