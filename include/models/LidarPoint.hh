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
#include <sstream>
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
    LidarPoint(const Timestamp &timestamp, uint32_t reflectivity, double x, double y, double z) : Point(x, y, z), timestamp(timestamp), reflectivity(reflectivity) {}
    /**
     * Constructor
     * @param timestamp Timestamp del punto
     * @param reflectivity Reflectividad del punto
     * @param x Posición en x del punto
     * @param y Posición en y del punto
     * @param z Posición en z del punto
     */
    LidarPoint(const Timestamp &timestamp, uint32_t reflectivity, int x, int y, int z) : Point((double)x, (double)y, (double)z), timestamp(timestamp), reflectivity(reflectivity) {}
    /**
     * Constructor
     * @param timestamp Timestamp del punto
     * @param reflectivity Reflectividad del punto
     * @param p Punto a compiar
     */
    LidarPoint(const Timestamp &timestamp, uint32_t reflectivity, const Point &p) : Point(p), timestamp(timestamp), reflectivity(reflectivity) {}
    /**
     * Constructor
     * @param timestamp Timestamp del punto
     * @param reflectivity Reflectividad del punto
     */
    LidarPoint(const Timestamp &timestamp, uint32_t reflectivity) : Point(), timestamp(timestamp), reflectivity(reflectivity) {}
    /**
     * Constructor
     * @param p Punto a copiar
     */
    LidarPoint(const Point &p) : Point(p), timestamp(0, 0), reflectivity(0) {}
    /**
     * Constructor del objeto solo con las coordenadas
     * @param x Posición en x del punto
     * @param y Posición en y del punto
     * @param z Posición en z del punto
     */
    LidarPoint(double x, double y, double z) : Point(x, y, z), timestamp(0, 0), reflectivity(0) {}
    /**
     * Constructor vacío del objeto
     */
    LidarPoint() : Point(), timestamp(0, 0), reflectivity(0) {}

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
    uint32_t getReflectivity() const { return reflectivity; }

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
    std::string string() const {
        std::stringstream line;
        line << "[Time: " << timestamp.string() << "] [Ref: " << reflectivity << "] " << ((Point *)this)->string();
        return line.str();
    }
    /**
     * Obtiene un string con los datos del punto en formato CSV
     * @return String con los datos del punto en formato CSV < X,Y,Z,Timestamp,Reflectividad >
     */
    std::string CSV() const {
        std::stringstream line;
        line << getX() << "," << getY() << "," << getZ() << "," << timestamp.getSeconds() << timestamp.getNanoseconds() << "," << reflectivity;
        return line.str();
    }
    /**
     * Obtiene un string con los datos del punto en formato CSV preparado para su lectura por Livox Viewer
     * @return String con los datos del punto en formato CSV de Livox Viewer
     */
    std::string LivoxCSV() const {
        std::stringstream line;
        line << "5,1,1,0,0x00000000,0,2," << timestamp.getSeconds() << timestamp.getNanoseconds()
             << "," << getX() << "," << getY() << "," << getZ() << "," << reflectivity
             << ",0," << (int)getX() << "," << (int)getY() << "," << (int)getZ() << ",0,0,0";
        return line.str();
    }
    /**
     * Obtiene un string con los datos de la cabecera CSV de Livox Viewer
     * @return String de cabecera de un archivo CSV de Livox Viewer
     */
    static const std::string &LivoxCSVHeader() {
        static std::string livox_header("Version,Slot ID,LiDAR Index,Rsvd,Error Code,Timestamp Type,"
                                        "Data Type,Timestamp,X,Y,Z,Reflectivity,Tag,"
                                        "Ori_x,Ori_y,Ori_z,Ori_radius,Ori_theta,Ori_phi");
        return livox_header;
    }
    // Imprime la información del punto p
    friend std::ostream &operator<<(std::ostream &strm, const LidarPoint &p) { return strm << p.string(); }
};

#endif  // LIDARPOINT_CLASS_H