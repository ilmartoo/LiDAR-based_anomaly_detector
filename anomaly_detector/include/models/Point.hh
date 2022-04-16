/**
 * @file Point.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 19/03/2022
 *
 * Definición e implementación del objeto Point
 *
 */

#ifndef POINT_CLASS_H
#define POINT_CLASS_H

#include <stdint.h>
#include <ostream>

#include "models/Timestamp.hh"

#include "debug.hh"

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
    Point(const Timestamp timestamp, const uint8_t reflectivity, const int32_t x, const int32_t y, const int32_t z)
        : timestamp(timestamp), reflectivity(reflectivity), x(x), y(y), z(z) {}

    /**
     * Constructor del objeto Punto
     * @param p Punto a copiar
     */
    Point(const Point &p) : timestamp(p.timestamp), reflectivity(p.reflectivity), x(p.x), y(p.y), z(p.z) {}

    /**
     * Devuelve el timestamp del punto
     * @return Timestamp del punto
     */
    inline const Timestamp &getTimestamp() const { return this->timestamp; };

    /**
     * Devuelve la reflectividad del punto
     * @return Reflectividad del punto
     */
    inline const uint8_t &getReflectivity() const { return this->reflectivity; };

    /**
     * Devuelve la localización en el eje x del punto
     * @return Posición en x del punto
     */
    inline const int32_t &getX() const { return this->x; };

    /**
     * Devuelve la localización en el eje y del punto
     * @return Posición en y del punto
     */
    inline const int32_t &getY() const { return this->y; };

    /**
     * Devuelve la localización en el eje z del punto
     * @return Posición en z del punto
     */
    inline const int32_t &getZ() const { return this->z; };

    /**
     * Obtiene un string con los datos del punto
     * @return String con los datos del punto
     */
    inline const std::string string() const {
        return "[Time: " + this->timestamp.string() + "] [Ref: " + std::to_string(this->reflectivity) + "] " + std::to_string(this->x) +
               ", " + std::to_string(this->y) + ", " + std::to_string(this->z);
    }

    /**
     * Obtiene un string con los datos del punto en formato csv: x,y,z,time,reflec
     * @return String con los datos del punto en formato csv
     */
    inline const std::string csv_string() const {
        return std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "," + std::to_string(timestamp.getSeconds()) +
               std::to_string(timestamp.getNanoseconds()) + "," + std::to_string(reflectivity);
    }

    // Imprime la información del punto p
    friend std::ostream &operator<<(std::ostream &strm, Point &p) { return strm << p.string(); }

   private:
    Timestamp timestamp;   ///< Timestamp del punto
    uint8_t reflectivity;  ///< Reflectividad del punto
    int32_t x;             ///< Localización en el eje x del punto
    int32_t y;             ///< Localización en el eje y del punto
    int32_t z;             ///< Localización en el eje <x> del punto
};

#endif  // POINT_CLASS_H