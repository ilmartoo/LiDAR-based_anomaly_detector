/**
 * @file DoublePoint.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 25/04/2022
 *
 * Clase que engloba la clase Point para la computación de planos
 *
 */

#ifndef DOUBLEPOINT_CLASS_H
#define DOUBLEPOINT_CLASS_H

#include "models/Point.hh"

class DoublePoint {
   public:
    /**
     * Constructor a partir de un objeto Point
     * @param p Punto de una nube de puntos a usar como base
     */
    DoublePoint(const Point &p) : coords{(double)p.getX(), (double)p.getY(), (double)p.getZ()}{};

    /**
     * Constructor a partir de unas coordenadas de un punto en un espacio tridimensional
     * @param x Coordenada x
     * @param y Coordenada y
     * @param z Coordenada z
     */
    DoublePoint(double x, double y, double z) : coords{x, y, z}{};

    /**
     * Devuelve la localización en el eje X del punto
     * @return Coordenada x
     */
    const double getX() const { return coords[0]; }

    /**
     * Devuelve la localización en el eje Y del punto
     * @return Coordenada y
     */
    const double getY() const { return coords[1]; }

    /**
     * Devuelve la localización en el eje Z del punto
     * @return Coordenada z
     */
    const double getZ() const { return coords[2]; }

    /**
     * Obtiene el valor de la dimensión especificada
     * @param i Dimensión del valor a recuperar
     * @return Valor de la dimensión especificada o 0 si sobrepasa la tercera dimensión
     */
    const double operator[](unsigned int i) const { return i > 2 ? 0 : coords[i]; }

   private:
    double coords[3];  ///< Coordenadas cartesianas
};

#endif  // DOUBLEPOINT_CLASS_H