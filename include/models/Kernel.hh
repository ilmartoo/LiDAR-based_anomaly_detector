//
// Created by miguelyermo on 28/1/22.
//

#pragma once

#include "models/Point.hh"
#include <memory>

class Octree;  // Forward declaration

enum Kernel_t  // Different types of kernels to be used in the factory function
{
    circle,
    sphere,
    square,
    cube
};

class AbstractKernel
/**
 * @brief Kernel to be used in the neighborhood searchs. This Kernel must be implemented in concrete classes.
 */
{
   public:
    double radius;  // Search Radius
    Point center;   // Center of the kernel
    Point boxMin;   // Min Point of the Bounding Cube around the given kernel
    Point boxMax;   // Max Point of the Bounding Cube around the given kernel

   public:
    AbstractKernel(const Point& center, const double radius) : radius(radius), center(center) { makeBox(boxMin, boxMax); };

    void makeBox(Point& boxMin, Point& boxMax) {
        boxMin = center - radius;
        boxMax = center + radius;
    };

    virtual const bool isInside(const Point& p) const = 0;  // This functions must be implemented in each concreteKernel
    virtual const bool boxOverlap(const Octree& octant) const = 0;
};

class Kernel2D : public AbstractKernel {
   public:
    Kernel2D(const Point& center, const double radius) : AbstractKernel(center, radius){};

    virtual const bool boxOverlap(const Octree& octant) const override;
};

class Kernel3D : public AbstractKernel {
   public:
    Kernel3D(const Point& center, const double radius) : AbstractKernel(center, radius){};

    virtual const bool boxOverlap(const Octree& octant) const override;
};

class CircularKernel : public Kernel2D {
   public:
    CircularKernel(const Point& center, const double radius) : Kernel2D(center, radius){};

    virtual const bool isInside(const Point& p) const override;
};

class SphereKernel : public Kernel3D {
   public:
    SphereKernel(const Point& center, const double radius) : Kernel3D(center, radius){};

    virtual const bool isInside(const Point& p) const override;
};

class SquareKernel : public Kernel2D {
   public:
    SquareKernel(const Point& center, const double radius) : Kernel2D(center, radius){};

    virtual const bool isInside(const Point& p) const override;
};

class CubeKernel : public Kernel3D {
   public:
    CubeKernel(const Point& center, const double radius) : Kernel3D(center, radius){};

    virtual const bool isInside(const Point& p) const override;
};

std::unique_ptr<AbstractKernel> kernelFactory(const Point& center, const double radius, const Kernel_t& type);