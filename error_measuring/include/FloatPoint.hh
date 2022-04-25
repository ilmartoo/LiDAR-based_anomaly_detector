/**
 * @file FloatPoint.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 25/04/2022
 *
 * Clase que engloba la clase Point para la computación de planos
 *
 */

#ifndef FLOATPOINT_CLASS_H
#define FLOATPOINT_CLASS_H

#include "models/Point.hh"

class FloatPoint {
   public:
    /**
     * Constructor a partir de un objeto Point
     * @param p Punto de una nube de puntos a usar como base
     */
    FloatPoint(const Point &p) : x(p.getX()), y(p.getY()), z(p.getZ()){};

    /**
     * Constructor a partir de unas coordenadas de un punto en un espacio tridimensional
     * @param x Coordenada x
     * @param y Coordenada y
     * @param z Coordenada z
     */
    FloatPoint(float x, float y, float z) : x(x), y(y), z(z){};

    /**
     * Devuelve la localización en el eje X del punto
     * @return Coordenada x
     */
    const float getX() const { return x; }
    /**
     * Devuelve la localización en el eje Y del punto
     * @return Coordenada y
     */
    const float getY() const { return y; }
    /**
     * Devuelve la localización en el eje Z del punto
     * @return Coordenada z
     */
    const float getZ() const { return z; }

   private:
    float x, y, z;  ///< Coordenadas cartesianas en metros
};

#endif  // FLOATPOINT_CLASS_H