/**
 * @file LidarPoint.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 10/05/2022
 *
 * Definición e implementación del objeto LidarPoint
 *
 */

#ifndef LIDARPOINT_CLASS_H
#define LIDARPOINT_CLASS_H

#include <stdint.h>
#include <ostream>

#include "models/Point.hh"
#include "models/Timestamp.hh"

/**
 * Representación de un punto de una nube de datos 3D
 */
class LidarPoint : public Point {
   private:
    Timestamp timestamp;    ///< Timestamp del punto
    uint32_t reflectivity;  ///< Reflectividad del punto

   public:
    /**
     * Constructor
     * @param timestamp Timestamp del punto
     * @param reflectivity Reflectividad del punto
     * @param x Posición en x del punto
     * @param y Posición en y del punto
     * @param z Posición en z del punto
     */
    LidarPoint(const Timestamp &timestamp, uint32_t reflectivity, double x, double y, double z) : timestamp(timestamp), reflectivity(reflectivity), Point(x, y, z) {}
    /**
     * Constructor
     * @param timestamp Timestamp del punto
     * @param reflectivity Reflectividad del punto
     * @param x Posición en x del punto
     * @param y Posición en y del punto
     * @param z Posición en z del punto
     */
    LidarPoint(const Timestamp &timestamp, uint32_t reflectivity, int x, int y, int z) : timestamp(timestamp), reflectivity(reflectivity), Point((double)x, (double)y, (double)z) {}
    /**
     * Constructor
     * @param timestamp Timestamp del punto
     * @param reflectivity Reflectividad del punto
     * @param p Punto a compiar
     */
    LidarPoint(const Timestamp &timestamp, uint32_t reflectivity, const Point& p) : timestamp(timestamp), reflectivity(reflectivity), Point(p) {}
    /**
     * Constructor
     * @param timestamp Timestamp del punto
     * @param reflectivity Reflectividad del punto
     */
    LidarPoint(const Timestamp &timestamp, uint32_t reflectivity) : timestamp(timestamp), reflectivity(reflectivity), Point() {}
    /**
     * Constructor
     * @param p Punto a copiar
     */
    LidarPoint(const Point &p) : timestamp(0, 0), reflectivity(0), Point(p) {}
    /**
     * Constructor del objeto solo con las coordenadas
     * @param x Posición en x del punto
     * @param y Posición en y del punto
     * @param z Posición en z del punto
     */
    LidarPoint(double x, double y, double z) : timestamp(0, 0), reflectivity(0), Point(x, y, z) {}
    /**
     * Constructor vacío del objeto
     */
    LidarPoint() : timestamp(0, 0), reflectivity(0), Point() {}

    ////// Getters
    /**
     * Devuelve el timestamp del punto
     * @return Timestamp del punto
     */
    const Timestamp &getTimestamp() const { return timestamp; }
    /**
     * Devuelve la reflectividad del punto
     * @return Reflectividad del punto
     */
    const uint32_t getReflectivity() const { return reflectivity; }

    ////// Setters
    /**
     * Setter del Timestamp
     * @param t Timestamp a establecer
     */
    void setTimestamp(const Timestamp &t) { this->timestamp = t; }
    /**
     * Setter de la reflectividad
     * @param r reflectividad a establecer
     */
    void setReflectivity(uint32_t r) { this->reflectivity = r; }

    ////// Strings e impresión
    /**
     * Obtiene un string con los datos del punto
     * @return String con los datos del punto
     */
    const std::string string() const {
        return "[Time: " + this->timestamp.string() + "] [Ref: " + std::to_string(this->reflectivity) + "] " + ((Point *)this)->string();
    }
    /**
     * Obtiene un string con los datos del punto en formato csv: x,y,z,time,reflec
     * @return String con los datos del punto en formato csv
     */
    const std::string csv_string() const {
        return std::to_string(getX()) + "," + std::to_string(getY()) + "," + std::to_string(getZ()) + "," + std::to_string(timestamp.getSeconds()) + std::to_string(timestamp.getNanoseconds()) + "," + std::to_string(reflectivity);
    }
    // Imprime la información del punto p
    friend std::ostream &operator<<(std::ostream &strm, const Point &p) { return strm << p.string(); }
};

#endif  // LIDARPOINT_CLASS_H