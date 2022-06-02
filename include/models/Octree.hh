/**
 * @file Octree.cc
 * @author Miguel Yermo (miguel.yermo@usc.es)
 * @date 05/03/2020
 *
 * Definición del objeto Octree
 *
 */

#pragma once

#include <array>
#include <fstream>
#include <iostream>
#include <vector>
#include <memory>

#include "models/Point.hh"
#include "models/Kernel.hh"

class Box {
   private:
    Vector min_{};
    Vector max_{};

   public:
    Box(const Point &p, const Vector &radii);
    bool isInside(const Point &p) const;

    const Vector &min() const { return min_; }
    const Vector &max() const { return max_; }
    double minX() const { return min_.getX(); }
    double minY() const { return min_.getY(); }
    double minZ() const { return min_.getZ(); }
    double maxX() const { return max_.getX(); }
    double maxY() const { return max_.getY(); }
    double maxZ() const { return max_.getZ(); }
};

// Keep dividing the octree while octants have more points than these.
static constexpr unsigned int MAX_POINTS = 100;

/**
 * @brief Implementación de un octree utilizado para el almacenaje y búsqueda de puntos de forma eficiente
 */
class Octree {
   private:
    std::vector<Octree> octants_{};
    Vector center_{};
    Point min_{};
    Point max_{};
    std::vector<Point *> points_{};
    unsigned int numPoints_{};
    float radius_{};

   public:
    Octree();

    const std::vector<Octree> &getOctants() const { return octants_; }
    void setOctants(const std::vector<Octree> &octants) { octants_ = octants; }
    void setCenter(const Vector &center) { center_ = center; }
    void setMin(const Point &min) { min_ = min; }
    void setMax(const Point &max) { max_ = max; }
    const std::vector<Point *> &getPoints() const { return points_; }
    void setPoints(const std::vector<Point *> &points) { points_ = points; }
    unsigned int getNumPoints() const { return numPoints_; }
    void setNumPoints(unsigned int numPoints) { numPoints_ = numPoints; }
    void setRadius(float radius) { radius_ = radius; }
    const Point &getMin() const { return min_; }
    const Point &getMax() const { return max_; }

    Octree(std::vector<Point> &points);
    Octree(const Vector &center, const float radius);
    Octree(Vector center, float radius, std::vector<Point *> &points);
    Octree(Vector center, float radius, std::vector<Point> &points);

    void computeOctreeLimits();
    bool isInside2D(Point &p) const;
    void insertPoints(std::vector<Point> &points);
    void insertPoints(std::vector<Point *> &points);
    void insertPoint(Point *p);
    void createOctants();
    void fillOctants();
    int octantIdx(Point *p);
    bool isLeaf() const;
    bool isEmpty() const;
    void buildOctree(std::vector<Point> &points);
    void buildOctree(std::vector<Point *> &points);
    const Vector &getCenter() const;
    float getRadius() const;
    static void makeBox(const Point &p, double radius, Vector &min, Vector &max);
    static void makeBoxUntilGround(const Point &p, float radius, Vector &min, Vector &max);
    static void makeBoxCylinder(const Point &p, double radius, Vector &min, Vector &max);
    static void makeBox(const Point &p, Vector radius, Vector &min, Vector &max);
    bool boxOverlap2D(const Vector &boxMin, const Vector &boxMax) const;
    bool boxOverlap3D(const Vector &boxMin, const Vector &boxMax) const;
    static bool insideBox2D(const Point &point, const Vector &min, const Vector &max);
    static bool insideCircle(const Point &p, const Point &center, float r);
    static bool insideBox3D(const Point &point, const Vector &min, const Vector &max);
    static bool insideRing(const Point &p, const Box &boxMin, const Box &boxMax);
    std::vector<Point *> neighbors2D(const Point &p, const Vector &boxMin, const Vector &boxMax, std::vector<Point *> &ptsInside) const;
    std::vector<Point *> circleNeighbors(const Point &p, const Vector &boxMin, const Vector &boxMax, std::vector<Point *> &ptsInside,
                                         float circleRadius) const;
    std::vector<Point *> neighbors3D(const Point &p, const Vector &boxMin, const Vector &boxMax, std::vector<Point *> &ptsInside) const;
    std::vector<Point *> neighbors3DFlagged(const Point &p, const Vector &boxMin, const Vector &boxMax, std::vector<Point *> &ptsInside,
                                            const std::vector<bool> &flags) const;

    std::vector<Point *> searchNeighbors(const Point &p, double radius, const Kernel_t &k_t) const;
    std::vector<Point *> neighbors(std::unique_ptr<AbstractKernel> &k, std::vector<Point *> &ptsInside) const;
    std::vector<Point *> searchNeighbors2D(const Point &p, double radius);
    std::vector<Point *> searchNeighbors2D(Point &p, double radius);
    std::vector<Point *> searchNeighbors2DUntilGround(const Point &p, double radius);
    std::vector<Point *> searchCylinderNeighbors(const Point &p, double radius);
    std::vector<Point *> searchCircleNeighbors(const Point &p, double radius) const;
    std::vector<Point *> searchGroundTruthNeighbors(const Point &p, double radius);
    std::vector<Point *> searchNeighbors3D(const Point &p, Vector radius);
    std::vector<Point *> searchNeighbors3D(const Point &p, double radius);
    std::vector<Point *> searchNeighbors3D(const Point &p, double radius, const std::vector<bool> &flags);
    std::vector<Point *> searchNeighbors3DNoGroup(const Point &point, float radius, int rId);
    std::vector<Point *> neighbors3DNoGroup(const Point &point, Vector boxMin, Vector boxMax, std::vector<Point *> ptsInside, int rId);

    int numNeighbors2D(const Point &point, Vector boxMin, Vector boxMax, int *numInside);

    int numOctreeNeighbors2D(const Point &point, float radius);

    /** Inside a sphere */
    std::vector<Point *> searchSphereNeighbors(const Point &point, float radius);

    std::vector<Point *> neighborsRing(const Point &p, Box &innerBox, Box &outerBox, std::vector<Point *> &ptsInside) const;

    std::vector<Point *> searchNeighborsRing(const Point &p, const Vector &innerRingRadii, const Vector &outerRingRadii);

    int numNeighbors2DDiffGroup(const Point &point, Vector &boxMin, Vector &boxMax, int &numInside);

    int numOctreeNeighbors2DDiffGroup(const Point &point, float radius);

    void writeOctree(std::ofstream &f, size_t index) const;
};

// Functions
Vector mbbCenter(Vector &min, Vector &radius);
Vector mbbRadii(Vector &min, Vector &max, float &maxRadius);
Vector mbb(const std::vector<Point> &points, float &maxRadius);