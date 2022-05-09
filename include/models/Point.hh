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
#include <cmath>

#include "models/Timestamp.hh"

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
    Point(const Timestamp timestamp, const uint8_t reflectivity, const double x, const double y, const double z)
        : timestamp(timestamp), reflectivity(reflectivity), x(x), y(y), z(z) {}

    /**
     * Constructor del objeto Punto
     * @param timestamp Timestamp del punto
     * @param reflectivity Reflectividad del punto
     * @param x Posición en x del punto
     * @param y Posición en y del punto
     * @param z Posición en z del punto
     */
    Point(const Timestamp timestamp, const uint8_t reflectivity, const int x, const int y, const int z)
        : timestamp(timestamp), reflectivity(reflectivity), x((double)x), y((double)y), z((double)z) {}

    /**
     * Constructor del objeto Punto
     * @param p Punto a copiar
     */
    Point(const Point &p) : timestamp(p.timestamp), reflectivity(p.reflectivity), x(p.x), y(p.y), z(p.z) {}

    /**
     * Constructor del objeto solo con las coordenadas
     * @param x Posición en x del punto
     * @param y Posición en y del punto
     * @param z Posición en z del punto
     */
    Point(const double x, const double y, const double z) : timestamp(0, 0), reflectivity(0), x(x), y(y), z(z) {}

    /**
     * Constructor vacío del objeto
     */
    Point() : timestamp(0, 0) {}

    /**
     * Devuelve el timestamp del punto
     * @return Timestamp del punto
     */
    inline const Timestamp &getTimestamp() const { return this->timestamp; }

    /**
     * Devuelve la reflectividad del punto
     * @return Reflectividad del punto
     */
    inline const uint8_t &getReflectivity() const { return this->reflectivity; }

    /**
     * Devuelve la localización en el eje x del punto
     * @return Posición en x del punto
     */
    inline const double &getX() const { return this->x; }

    /**
     * Devuelve la localización en el eje y del punto
     * @return Posición en y del punto
     */
    inline const double &getY() const { return this->y; }

    /**
     * Devuelve la localización en el eje z del punto
     * @return Posición en z del punto
     */
    inline const double &getZ() const { return this->z; }

    /**
     * Setter del Timestamp
     * @param t Timestamp a establecer
     */
    inline void setTimestamp(const Timestamp &t) { this->timestamp = t; }

    /**
     * Setter de la reflectividad
     * @param r reflectividad a establecer
     */
    inline void setReflectivity(const uint8_t &r) { this->reflectivity = r; }

    /**
     * Setter de la coordenada x
     * @param x Valor de x a establecer
     */
    inline void setX(const double &x) { this->x = x; }

    /**
     * Setter de la coordenada y
     * @param y Valor de y a establecer
     */
    inline void setY(const double &y) { this->y = y; }

    /**
     * Setter de la coordenada z
     * @param z Valor de z a establecer
     */
    inline void setZ(const double &z) { this->z = z; }

    /**
     * Calcula la distancia euclidea entre dos puntos
     * @param p Punto contra el que calcular la distancia
     * @return double distancia de separación entre los dos puntos
     */
    double distance3D(const Point &p) const {
        return std::sqrt((this->x - p.x) * (this->x - p.x) + (this->y - p.y) * (this->y - p.y) + (this->z - p.z) * (this->z - p.z));
    }

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
    friend std::ostream &operator<<(std::ostream &strm, const Point &p) { return strm << p.string(); }

    /**
     * Operador de resta de puntos
     * @param p Punto a restar
     * @return Punto resultado de la operacion con timestamp y reflectividad del punto original
     */
    Point operator-(const Point &p) { return Point(this->timestamp, this->reflectivity, this->x - p.x, this->y - p.y, this->z - p.z); }

    /**
     * Operador de resta de puntos
     * @param d double a restar
     * @return Punto resultado de la operacion con timestamp y reflectividad del punto original
     */
    Point operator-(const double &d) { return Point(this->timestamp, this->reflectivity, this->x - d, this->y - d, this->z - d); }

    /**
     * Operador de suma de puntos
     * @param p Punto a sumar
     * @return Punto resultado de la operacion con timestamp y reflectividad del punto original
     */
    Point operator+(const Point &p) { return Point(this->timestamp, this->reflectivity, this->x + p.x, this->y + p.y, this->z + p.z); }

    /**
     * Operador de suma de puntos
     * @param d double a sumar
     * @return Punto resultado de la operacion con timestamp y reflectividad del punto original
     */
    Point operator+(const double &d) { return Point(this->timestamp, this->reflectivity, this->x + d, this->y + d, this->z + d); }

   private:
    Timestamp timestamp;   ///< Timestamp del punto
    uint8_t reflectivity;  ///< Reflectividad del punto
    double x;              ///< Localización en el eje x del punto
    double y;              ///< Localización en el eje y del punto
    double z;              ///< Localización en el eje z del punto
};

typedef Point Vector;  ///< Definición de Vector como un Point

#endif  // POINT_CLASS_H