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

#include <ostream>
#include <cmath>

/**
 * Representación de un punto tridimensional
 */
class Point {
   private:
    double x;  ///< Localización en el eje x del punto
    double y;  ///< Localización en el eje y del punto
    double z;  ///< Localización en el eje z del punto

   public:
    /**
     * Constructor
     */
    Point() {}
    /**
     * Constructor
     * @param x Posición en x del punto
     * @param y Posición en y del punto
     * @param z Posición en z del punto
     */
    Point(double x, double y, double z) : x(x), y(y), z(z) {}
    /**
     * Constructor
     * @param x Posición en x del punto
     * @param y Posición en y del punto
     * @param z Posición en z del punto
     */
    Point(int x, int y, int z) : x((double)x), y((double)y), z((double)z) {}

    /**
     * Calcula la distancia euclidea entre dos puntos
     * @param p Punto contra el que calcular la distancia
     * @return double distancia de separación entre los dos puntos
     */
    double distance3D(const Point &p) const {
        return std::sqrt((this->x - p.x) * (this->x - p.x) + (this->y - p.y) * (this->y - p.y) + (this->z - p.z) * (this->z - p.z));
    }

    ////// Getters
    /**
     * Devuelve la localización en el eje x del punto
     * @return Posición en x del punto
     */
    double getX() const { return x; }
    /**
     * Devuelve la localización en el eje y del punto
     * @return Posición en y del punto
     */
    double getY() const { return y; }
    /**
     * Devuelve la localización en el eje z del punto
     * @return Posición en z del punto
     */
    double getZ() const { return z; }

    ////// Setters
    /**
     * Setter de la coordenada x
     * @param x Valor de x a establecer
     */
    void setX(double x) { this->x = x; }
    /**
     * Setter de la coordenada y
     * @param y Valor de y a establecer
     */
    void setY(double y) { this->y = y; }
    /**
     * Setter de la coordenada z
     * @param z Valor de z a establecer
     */
    void setZ(double z) { this->z = z; }

    ////// Strings e impresión
    /**
     * Obtiene un string con los datos del punto
     * @return String con los datos del punto
     */
    const std::string string() const {
        return std::to_string(this->x) + ", " + std::to_string(this->y) + ", " + std::to_string(this->z);
    }
    // Imprime la información del punto p
    friend std::ostream &operator<<(std::ostream &strm, const Point &p) { return strm << p.string(); }

    ////// Operadores
    /**
     * Operador de acceso en array
     * @param i Indice de la corrdenada
     * @return Coordenada recuperada o 0 si es < 0 o > 2
     */
    double operator[](int i) const { return (i < 0 || i > 2 ? 0 : (i == 0 ? this->x : (i == 1 ? this->y : this->z))); }
    /**
     * Operador de resta de puntos
     * @param p Punto a restar
     * @return Punto resultado de la operacion con timestamp y reflectividad del punto original
     */
    Point operator-(const Point &p) const { return Point(this->x - p.x, this->y - p.y, this->z - p.z); }
    /**
     * Operador de resta de puntos
     * @param d double a restar
     * @return Punto resultado de la operacion con timestamp y reflectividad del punto original
     */
    Point operator-(double d) const { return Point(this->x - d, this->y - d, this->z - d); }
    /**
     * Operador de suma de puntos
     * @param p Punto a sumar
     * @return Punto resultado de la operacion con timestamp y reflectividad del punto original
     */
    Point operator+(const Point &p) const { return Point(this->x + p.x, this->y + p.y, this->z + p.z); }
    /**
     * Operador de suma de puntos
     * @param d double a sumar
     * @return Punto resultado de la operacion con timestamp y reflectividad del punto original
     */
    Point operator+(double d) const { return Point(this->x + d, this->y + d, this->z + d); }
    /**
     * Operador de división de puntos
     * @param p Punto a dividir
     * @return Punto resultado de la operacion con timestamp y reflectividad del punto original
     */
    Point operator/(const Point &p) const { return Point(this->x / p.x, this->y / p.y, this->z / p.z); }
    /**
     * Operador de división de puntos
     * @param d double a dividir
     * @return Punto resultado de la operacion con timestamp y reflectividad del punto original
     */
    Point operator/(double d) const { return Point(this->x / d, this->y / d, this->z / d); }
    /**
     * Operador de multiplicación de puntos
     * @param p Punto a multiplicar
     * @return Punto resultado de la operacion con timestamp y reflectividad del punto original
     */
    Point operator*(const Point &p) const { return Point(this->x * p.x, this->y * p.y, this->z * p.z); }
    /**
     * Operador de multiplicación de puntos
     * @param d double a multiplicar
     * @return Punto resultado de la operacion con timestamp y reflectividad del punto original
     */
    Point operator*(double d) const { return Point(this->x * d, this->y * d, this->z * d); }
};

typedef Point Vector;  ///< Definición de Vector como un Point

#endif  // POINT_CLASS_H