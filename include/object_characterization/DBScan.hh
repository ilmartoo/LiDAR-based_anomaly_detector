/**
 * @file DBScan.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 18/05/2022
 *
 * Definición del algoritmo DBScan
 *
 */

#ifndef DBSCAN_CLASS_H
#define DBSCAN_CLASS_H

#include <vector>
#include <cmath>
#include <utility>

#include "models/Point.hh"

class DBScan {
   public:
   /**
    * Ejecuta el algoritmo de DBScan estableciendo los clusterIDs correspondientes cada punto del vector de puntos
    * @param maxDistance Maxima distancia de separación entre dos puntos para pertenecer al mismo cluster
    * @param minPoints Número mínimo de puntos para una agrupación ser considerada cluster
    * @param points Vector de puntos sobre los cuales se realizará la distinción de clusteres
    * @return Vector de clusteres representados en forma de vectores de indices de los puntos pertenecientes al cluster
    */
    static std::vector<std::vector<size_t>> clusters(double maxDistance, unsigned int minPoints, std::vector<Point> &points);

    /**
    * Ejecuta el algoritmo de DBScan estableciendo los clusterIDs correspondientes cada punto del vector de puntos según sus normales
    * @param maxDistance Maxima distancia de separación entre dos puntos para pertenecer a la misma cara
    * @param minPoints Número mínimo de puntos para una agrupación ser considerada una cara
    * @param points Vector de puntos sobre los cuales se realizará la distinción de caras
    * @param normalDispersion Dispersión máxima entre normales para considerarse la misma cara
    * @return Vector de caras representadas en forma de vectores de indices de los puntos pertenecientes a la cara
    */
    static std::vector<std::vector<size_t>> normals(double maxDistance, unsigned int minPoints, std::vector<Point> &points, double normalDispersion);

   private:
	// Expande un cluster a partir de un centroide y el ID especificado junto con las variables limitantes especificadas
    static std::pair<bool, std::vector<size_t>> expandCluster(Point &centroid, int clusterID, double maxDistance, unsigned int minPoints, std::vector<Point> &points);
    // Calcula el indice de los puntos pertenecientes al cluster según un centroide dado
    static std::vector<size_t> calculateCluster(const Point &centroid, double maxDistance, const std::vector<Point> &points);
	
    // Expande una cara a partir de un centroide y el ID especificado junto con las variables limitantes especificadas
    static std::pair<bool, std::vector<size_t>> expandNormal(const Vector &cnormal, Point &centroid, int clusterID, double maxDistance, unsigned int minPoints, const Octree &pointsMap, std::vector<Point> &points, double normalDispersion);
    // Calcula el indice de los puntos pertenecientes a la cara según una normal dada
    static std::vector<size_t> calculateNormal(const Vector &cnormal, const Point &centroid, double maxDistance, const Octree &map, const std::vector<Point> &points, double normalDispersion);
};

#endif  // DBSCAN_CLASS_H