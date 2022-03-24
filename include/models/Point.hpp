/**
 * @file Point.hpp
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 19/03/2022
 *
 * Definición e implementación del objeto Point
 *
 */

#ifndef __POINT_CLASS_H
#define __POINT_CLASS_H

#include <stdint.h>
#include <ostream>

/**
 * Representación de un punto de una nube de datos 3D
 */
class Point {
   public:
    /**
     * Constructor del objeto Punto
     * @param timestamp Timestamp del punto
     * @param reflectivity Reflectividad del punto
     * @param x Posición en x del punto
     * @param y Posición en y del punto
     * @param z Posición en z del punto
     */
    Point(uint64_t timestamp, uint8_t reflectivity, int32_t x, int32_t y, int32_t z)
        : timestamp(timestamp), reflectivity(reflectivity), x(x), y(y), z(z) {
        this->timestamp_counter = 0;  // Solo se ejecuta la primera vez
    }

    /**
     * Constructor del objeto Punto
     * @param reflectivity Reflectividad del punto
     * @param x Posición en x del punto
     * @param y Posición en y del punto
     * @param z Posición en z del punto
     */
    Point(uint8_t reflectivity, int32_t x, int32_t y, int32_t z) : reflectivity(reflectivity), x(x), y(y), z(z) {
        this->timestamp_counter = 0;  // Solo se ejecuta la primera vez
        this->timestamp = timestamp_counter++;
    }

    /**
     * Devuelve el timestamp del punto
     * @return Timestamp del punto
     */
    inline uint64_t getTimestamp() { return this->timestamp; };

    /**
     * Devuelve la reflectividad del punto
     * @return Reflectividad del punto
     */
    inline uint8_t getReflectivity() { return this->reflectivity; };

    /**
     * Devuelve la localización en el eje x del punto
     * @return Posición en x del punto
     */
    inline int32_t getX() { return this->x; };

    /**
     * Devuelve la localización en el eje y del punto
     * @return Posición en y del punto
     */
    inline int32_t getY() { return this->y; };

    /**
     * Devuelve la localización en el eje z del punto
     * @return Posición en z del punto
     */
    inline int32_t getZ() { return this->z; };

    // Imprime la información del punto p
    friend std::ostream &operator<<(std::ostream &strm, const Point &p) {
        return strm << "[Time: " << p.timestamp << "] [Ref: " << p.reflectivity << "] " << p.x << ", " << p.y << ", "
                    << p.z;
    }

   private:
    static uint64_t timestamp_counter;  ///< Contador del timestamp del punto
    uint64_t timestamp;                 ///< Timestamp del punto
    uint8_t reflectivity;               ///< Reflectividad del punto
    int32_t x;                          ///< Localización en el eje x del punto
    int32_t y;                          ///< Localización en el eje y del punto
    int32_t z;                          ///< Localización en el eje <x> del punto
};

#endif  //__POINT_CLASS_H