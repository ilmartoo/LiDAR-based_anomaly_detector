/**
 * @file octree.cc
 * @author Miguel Yermo (miguel.yermo@usc.es)
 * @date 28/01/2022
 *
 * Implementación de las funciones de Kernel
 *
 */

#include "models/Kernel.hh"
#include "models/octree.hh"
#include <memory>

std::unique_ptr<AbstractKernel> kernelFactory(const Point& center, const double radius, const Kernel_t& type)
/**
 * @brief Kernel Factory. Given a type of kernel, returns a unique pointer to the created kernel.
 * @param center
 * @param radius
 * @param type
 * @return
 */
{
    switch (type) {
        case Kernel_t::circle:
            return std::make_unique<CircularKernel>(center, radius);
        case Kernel_t::square:
            return std::make_unique<SquareKernel>(center, radius);
        case Kernel_t::sphere:
            return std::make_unique<SphereKernel>(center, radius);
        case Kernel_t::cube:
            return std::make_unique<CubeKernel>(center, radius);

        default:
            return nullptr;
    }
}

const bool Kernel2D::boxOverlap(const Octree& octant) const
/**
 * @brief Checks if a given octant overlaps with the given kernel in 2 dimensions
 * @param octant
 * @return
 */
{
    if (octant.getCenter().getZ() + octant.getRadius() < boxMin.getZ() || octant.getCenter().getY() + octant.getRadius() < boxMin.getY())
        return false;
    if (octant.getCenter().getZ() - octant.getRadius() > boxMax.getZ() || octant.getCenter().getY() - octant.getRadius() > boxMax.getY())
        return false;

    return true;
}

const bool Kernel3D::boxOverlap(const Octree& octant) const
/**
 * @brief Checks if a given octant overlaps with the given kernel in 3 dimensions
 * @param octant
 * @return
 */
{
    if (octant.getCenter().getZ() + octant.getRadius() < boxMin.getZ() || octant.getCenter().getY() + octant.getRadius() < boxMin.getY() ||
        octant.getCenter().getX() + octant.getRadius() < boxMin.getX())
        return false;
    if (octant.getCenter().getZ() - octant.getRadius() > boxMax.getZ() || octant.getCenter().getY() - octant.getRadius() > boxMax.getY() ||
        octant.getCenter().getX() - octant.getRadius() > boxMax.getX())
        return false;

    return true;
}

const bool SquareKernel::isInside(const Point& p) const
/**
 * @brief Checks if a given point lies inside the kernel
 * @param p
 * @return
 */
{
    if (p.getZ() > boxMin.getZ() && p.getY() > boxMin.getY())
        if (p.getZ() < boxMax.getZ() && p.getY() < boxMax.getY())
            return true;

    return false;
}

const bool CubeKernel::isInside(const Point& p) const
/**
 * @brief Checks if a given point lies inside the kernel
 * @param p
 * @return
 */
{
    if (p.getZ() > boxMin.getZ() && p.getY() > boxMin.getY() && p.getX() > boxMin.getX())
        if (p.getZ() < boxMax.getZ() && p.getY() < boxMax.getY() && p.getX() < boxMax.getX())
            return true;

    return false;
};

const bool CircularKernel::isInside(const Point& p) const
/**
 * @brief Checks if a given point lies inside the kernel
 * @param p
 * @return
 */
{
    return (p.getZ() - center.getZ()) * (p.getZ() - center.getZ()) + (p.getY() - center.getY()) * (p.getY() - center.getY()) <
           radius * radius;
}

const bool SphereKernel::isInside(const Point& p) const
/**
 * @brief Checks if a given point lies inside the kernel
 * @param p
 * @return
 */
{
    return (p.getZ() - center.getZ()) * (p.getZ() - center.getZ()) + (p.getY() - center.getY()) * (p.getY() - center.getY()) +
               (p.getX() - center.getX()) * (p.getX() - center.getX()) <
           radius * radius;
}