#ifndef __POINT_CLASS_H
#define __POINT_CLASS_H

#include <stdint.h>

class Point {
   public:
    Point(int32_t x, int32_t y, int32_t z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    /**
     * Devuelve la localización en el eje x del punto
     * @return Posición en x del punto
     */
    inline int32_t getX() {
        return this->x;
    };

    /**
     * Devuelve la localización en el eje y del punto
     * @return Posición en y del punto
     */
    inline int32_t getY() {
        return this->y;
    };

    /**
     * Devuelve la localización en el eje z del punto
     * @return Posición en z del punto
     */
    inline int32_t getZ() {
        return this->z;
    };

   private:
    int32_t x;
    int32_t y;
    int32_t z;
};

#endif  //__POINT_CLASS_H