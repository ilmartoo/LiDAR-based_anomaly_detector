/**
 * @file Octree.cc
 * @author Miguel Yermo (miguel.yermo@usc.es)
 * @date 05/03/2020
 *
 * Implementación del objeto Octree
 * 
 */

#include <limits>

#include "models/Octree.hh"
#include "models/Kernel.hh"


Octree::Octree() {}

Octree::Octree(std::vector<Point> &points) {
    center_ = mbb(points, radius_);
    octants_.reserve(8);
    buildOctree(points);
}

Octree::Octree(const Vector &center, const float radius)
    : octants_(), center_(center), min_{}, max_{}, points_(), numPoints_(0), radius_(radius) {
    octants_.reserve(8);
};

Octree::Octree(Vector center, float radius, std::vector<Point *> &points)
    : octants_(), center_(center), min_{}, max_{}, points_(), numPoints_(0), radius_(radius) {
    octants_.reserve(8);
    buildOctree(points);
}

Octree::Octree(Vector center, float radius, std::vector<Point> &points)
    : octants_(), center_(center), min_{}, max_{}, points_(), numPoints_(0), radius_(radius) {
    octants_.reserve(8);
    buildOctree(points);
}

void Octree::computeOctreeLimits()
/**
 * Compute the minimum and maximum coordinates of the octree bounding box.
 */
{
    min_.setZ(center_.getZ() - radius_);
    min_.setY(center_.getY() - radius_);
    max_.setZ(center_.getZ() + radius_);
    max_.setY(center_.getY() + radius_);
}

bool Octree::isInside2D(Point &p) const
/**
 * Checks if a point is inside the octree limits.
 * @param p
 * @return
 */
{
    if (p.getZ() >= min_.getZ() && p.getY() >= min_.getY()) {
        if (p.getZ() <= max_.getZ() && p.getY() <= max_.getY()) {
            return true;
        }
    }

    return false;
}

void Octree::insertPoints(std::vector<Point> &points) {
    for (Point &p : points) {
        insertPoint(&p);
    }
}

void Octree::insertPoints(std::vector<Point *> &points) {
    for (Point *p : points) {
        insertPoint(p);
    }
}

void Octree::insertPoint(Point *p) {
    unsigned int idx = 0;

    if (isLeaf()) {
        if (isEmpty()) {
            points_.emplace_back(p);
            numPoints_ = 1;
        } else {
            if (numPoints_ > MAX_POINTS) {
                createOctants();  // Creation of children octree
                fillOctants();    // Move points from current Octree to its corresponding children.
                idx = octantIdx(p);
                octants_[idx].insertPoint(p);
            } else {
                points_.emplace_back(p);
                numPoints_++;
            }
        }
    } else {
        idx = octantIdx(p);
        octants_[idx].insertPoint(p);
    }
}

void Octree::createOctants() {
    Vector newCenter;
    for (int i = 0; i < 8; i++) {
        newCenter = center_;
        newCenter.setZ(newCenter.getZ() + radius_ * (i & 4 ? 0.5f : -0.5f));
        newCenter.setY(newCenter.getY() + radius_ * (i & 2 ? 0.5f : -0.5f));
        newCenter.setX(newCenter.getX() + radius_ * (i & 1 ? 0.5f : -0.5f));
        octants_.emplace_back(Octree(newCenter, 0.5f * radius_));
    }
}

void Octree::fillOctants() {
    int idx = 0;  // Idx of the octant where a point should be inserted.

    for (Point *&p : points_) {
        idx = octantIdx(p);
        octants_[idx].insertPoint(p);
    }

    numPoints_ = 0;
    points_.clear();
}

int Octree::octantIdx(Point *p) {
    int child = 0;

    if (p->getZ() >= center_.getZ())
        child |= 4;
    if (p->getY() >= center_.getY())
        child |= 2;
    if (p->getX() >= center_.getX())
        child |= 1;

    return child;
}

bool Octree::isLeaf() const { return octants_.empty(); }
bool Octree::isEmpty() const { return numPoints_ == 0; }

void Octree::buildOctree(std::vector<Point> &points)
/**
 * Build the Octree
 */
{
    computeOctreeLimits();
    insertPoints(points);
}

void Octree::buildOctree(std::vector<Point *> &points)
/**
 * Build the Octree
 */
{
    computeOctreeLimits();
    insertPoints(points);
}

const Vector &Octree::getCenter() const { return center_; }
float Octree::getRadius() const { return radius_; }

void Octree::makeBox(const Point &p, double radius, Vector &min, Vector &max) {
    min.setZ(p.getZ() - radius);
    min.setY(p.getY() - radius);
    min.setX(p.getX() - radius);
    max.setZ(p.getZ() + radius);
    max.setY(p.getY() + radius);
    max.setX(p.getX() + radius);
}

void Octree::makeBoxUntilGround(const Point &p, float radius, Vector &min, Vector &max) {
    min.setZ(p.getZ() - radius);
    min.setY(p.getY() - radius);
    min.setX(p.getX());
    max.setZ(p.getZ() + radius);
    max.setY(p.getY() + radius);
    max.setX(p.getX());
}

void Octree::makeBoxCylinder(const Point &p, double radius, Vector &min, Vector &max) {
    min.setZ(p.getZ() - radius);
    min.setY(p.getY() - radius);
    min.setX(0);
    max.setZ(p.getZ() + radius);
    max.setY(p.getY() + radius);
    max.setX(1000);
}

void Octree::makeBox(const Point &p, Vector radius, Vector &min, Vector &max)
/**
 * This functions defines the box whose inner points will be considered as neighs of Point p.
 * @param p
 * @param radius Vector of radius. One per spatial direction.
 * @param min
 * @param max
 */
{
    min.setZ(p.getZ() - radius.getZ());
    min.setY(p.getY() - radius.getY());
    min.setX(p.getX() - radius.getX());
    max.setZ(p.getZ() + radius.getZ());
    max.setY(p.getY() + radius.getY());
    max.setX(p.getX() + radius.getX());
}

bool Octree::boxOverlap2D(const Vector &boxMin, const Vector &boxMax) const {
    if (center_.getZ() + radius_ < boxMin.getZ() || center_.getY() + radius_ < boxMin.getY())
        return false;
    if (center_.getZ() - radius_ > boxMax.getZ() || center_.getY() - radius_ > boxMax.getY())
        return false;

    return true;
}

bool Octree::boxOverlap3D(const Vector &boxMin, const Vector &boxMax) const {
    if (center_.getZ() + radius_ < boxMin.getZ() || center_.getY() + radius_ < boxMin.getY() || center_.getX() + radius_ < boxMin.getX())
        return false;
    if (center_.getZ() - radius_ > boxMax.getZ() || center_.getY() - radius_ > boxMax.getY() || center_.getX() - radius_ > boxMax.getX())
        return false;

    return true;
}

bool Octree::insideBox2D(const Point &point, const Vector &min, const Vector &max) {
    if (point.getZ() > min.getZ() && point.getY() > min.getY()) {
        if (point.getZ() < max.getZ() && point.getY() < max.getY()) {
            return true;
        }
    }

    return false;
}

bool Octree::insideCircle(const Point &p, const Point &c, float r) {
    return (p.getZ() - c.getZ()) * (p.getZ() - c.getZ()) + (p.getY() - c.getY()) * (p.getY() - c.getY()) < r * r;
}

bool Octree::insideBox3D(const Point &point, const Vector &min, const Vector &max) {
    if (point.getZ() > min.getZ() && point.getY() > min.getY() && point.getX() > min.getX()) {
        if (point.getZ() < max.getZ() && point.getY() < max.getY() && point.getX() < max.getX()) {
            return true;
        }
    }
    return false;
}

bool Octree::insideRing(const Point &p, const Box &boxMin, const Box &boxMax) {
    if (!boxMin.isInside(p) && boxMax.isInside(p))
        return true;

    return false;
}

std::vector<Point *> Octree::neighbors2D(const Point &p, const Vector &boxMin, const Vector &boxMax,
                                         std::vector<Point *> &ptsInside) const {
    if (isLeaf()) {
        if (!isEmpty()) {
            for (Point *point_ptr : points_) {
                if (insideBox2D(*point_ptr, boxMin, boxMax)) {
                    ptsInside.emplace_back(point_ptr);
                }
            }
        }
    } else {
        for (const Octree &octant : octants_) {
            if (!octant.boxOverlap2D(boxMin, boxMax)) {
                continue;
            } else {
                ptsInside = octant.neighbors2D(p, boxMin, boxMax, ptsInside);
            }
        }
    }

    return std::move(ptsInside);
}

std::vector<Point *> Octree::circleNeighbors(const Point &p, const Vector &boxMin, const Vector &boxMax, std::vector<Point *> &ptsInside,
                                             float circleRadius) const {
    if (isLeaf()) {
        if (!isEmpty()) {
            for (Point *point_ptr : points_) {
                const Point &point = *point_ptr;
                if (insideCircle(point, p, circleRadius)) {
                    ptsInside.emplace_back(point_ptr);
                }
            }
        }
    } else {
        for (const Octree &octant : octants_) {
            if (!octant.boxOverlap2D(boxMin, boxMax)) {
                continue;
            } else {
                ptsInside = octant.circleNeighbors(p, boxMin, boxMax, ptsInside, circleRadius);
            }
        }
    }

    return std::move(ptsInside);
}

std::vector<Point *> Octree::neighbors3D(const Point &p, const Vector &boxMin, const Vector &boxMax,
                                         std::vector<Point *> &ptsInside) const {
    if (isLeaf()) {
        if (!isEmpty()) {
            for (Point *point_ptr : points_) {
                const Point &point = *point_ptr;
                // The neighbors do not inc the point itself.
                if (insideBox3D(point, boxMin, boxMax)) {
                    ptsInside.emplace_back(point_ptr);
                }
            }
        }
    } else {
        for (const Octree &octant : octants_) {
            if (!octant.boxOverlap3D(boxMin, boxMax)) {
                continue;
            } else {
                ptsInside = octant.neighbors3D(p, boxMin, boxMax, ptsInside);
            }
        }
    }

    return std::move(ptsInside);
}

std::vector<Point *> Octree::neighbors3DFlagged(const Point &p, const Vector &boxMin, const Vector &boxMax, std::vector<Point *> &ptsInside,
                                                const std::vector<bool> &flags) const {
    if (isLeaf()) {
        if (!isEmpty()) {
            for (Point *point_ptr : points_) {
                const Point &point = *point_ptr;
                // The neighbors do not inc the point itself.
                if (insideBox3D(point, boxMin, boxMax)) {
                    ptsInside.emplace_back(point_ptr);
                }
            }
        }
    } else {
        for (const Octree &octant : octants_) {
            if (!octant.boxOverlap3D(boxMin, boxMax)) {
                continue;
            } else {
                ptsInside = octant.neighbors3DFlagged(p, boxMin, boxMax, ptsInside, flags);
            }
        }
    }

    return std::move(ptsInside);
}

std::vector<Point *> Octree::searchNeighbors(const Point &p, double radius, const Kernel_t &k_t) const
/**
 * @brief Search neighbors function. Given a point and a radius, return the points inside a given kernel type
 * @param p Center of the kernel to be used
 * @param radius Radius of the kernel to be used
 * @param k_t Kernel type (circle, sphere, square, cube)
 * @return Points inside the given kernel type
 */
{
    std::vector<Point *> ptsInside{};
    std::unique_ptr<AbstractKernel> kernel = kernelFactory(p, radius, k_t);

    return neighbors(kernel, ptsInside);
}

std::vector<Point *> Octree::neighbors(std::unique_ptr<AbstractKernel> &k, std::vector<Point *> &ptsInside) const {
    if (isLeaf()) {
        if (!isEmpty()) {
            for (Point *point_ptr : points_) {
                if (k->isInside(*point_ptr)) {
                    ptsInside.emplace_back(point_ptr);
                }
            }
        }
    } else {
        for (const Octree &octant : octants_) {
            if (!k->boxOverlap(octant))  // TODO: Include in the kernel?
            {
                continue;
            } else {
                ptsInside = octant.neighbors(k, ptsInside);
            }
        }
    }

    return std::move(ptsInside);
}

std::vector<Point *> Octree::searchNeighbors2D(const Point &p, double radius) {
    Vector boxMin, boxMax;
    std::vector<Point *> ptsInside;

    makeBox(p, radius, boxMin, boxMax);
    ptsInside = neighbors2D(p, boxMin, boxMax, ptsInside);

    return ptsInside;
}

std::vector<Point *> Octree::searchNeighbors2D(Point &p, double radius) {
    Vector boxMin, boxMax;
    std::vector<Point *> ptsInside;

    makeBox(p, radius, boxMin, boxMax);
    ptsInside = neighbors2D(p, boxMin, boxMax, ptsInside);

    return ptsInside;
}

std::vector<Point *> Octree::searchNeighbors2DUntilGround(const Point &p, double radius) {
    Vector boxMin, boxMax;
    std::vector<Point *> ptsInside;

    makeBoxUntilGround(p, radius, boxMin, boxMax);
    ptsInside = neighbors2D(p, boxMin, boxMax, ptsInside);

    return ptsInside;
}

std::vector<Point *> Octree::searchCylinderNeighbors(const Point &p, double radius) {
    Vector boxMin, boxMax;
    std::vector<Point *> ptsInside;

    makeBoxUntilGround(p, radius, boxMin, boxMax);
    ptsInside = circleNeighbors(p, boxMin, boxMax, ptsInside, radius);

    return ptsInside;
}

std::vector<Point *> Octree::searchCircleNeighbors(const Point &p, double radius) const {
    Vector boxMin, boxMax;
    std::vector<Point *> ptsInside;

    makeBox(p, radius, boxMin, boxMax);
    ptsInside = circleNeighbors(p, boxMin, boxMax, ptsInside, radius);

    return ptsInside;
}

std::vector<Point *> Octree::searchGroundTruthNeighbors(const Point &p, double radius) {
    Vector boxMin, boxMax;
    std::vector<Point *> ptsInside;

    makeBoxCylinder(p, radius, boxMin, boxMax);
    ptsInside = circleNeighbors(p, boxMin, boxMax, ptsInside, radius);

    return ptsInside;
}

std::vector<Point *> Octree::searchNeighbors3D(const Point &p, Vector radius)
/**
 * Searching neighbors in 3D using a different radius for each direction
 * @param p Point around the neighbors will be search
 * @param radius Vector of radiuses: one per spatial direction
 * @return
 */
{
    Vector boxMin, boxMax;
    std::vector<Point *> ptsInside;

    makeBox(p, radius, boxMin, boxMax);
    ptsInside = neighbors3D(p, boxMin, boxMax, ptsInside);

    return ptsInside;
}

std::vector<Point *> Octree::searchNeighbors3D(const Point &p, double radius)
/**
 * Searching neighbors in 3D using a different radius for each direction
 * @param p Point around the neighbors will be search
 * @param radius Vector of radiuses: one per spatial direction
 * @return
 */
{
    Vector boxMin, boxMax;
    std::vector<Point *> ptsInside;

    makeBox(p, radius, boxMin, boxMax);
    ptsInside = neighbors3D(p, boxMin, boxMax, ptsInside);

    return ptsInside;
}

std::vector<Point *> Octree::searchNeighbors3D(const Point &p, double radius, const std::vector<bool> &flags)
/**
 * Searching neighbors in 3D using a different radius for each direction
 * @param p Point around the neighbors will be search
 * @param radius Vector of radiuses: one per spatial direction
 * @param flags Vector of flags: return only points which flags[pointId] == false
 * @return
 */
{
    Vector boxMin, boxMax;
    std::vector<Point *> ptsInside;

    makeBox(p, radius, boxMin, boxMax);
    ptsInside = neighbors3DFlagged(p, boxMin, boxMax, ptsInside, flags);

    return ptsInside;
}

std::vector<Point *> Octree::searchNeighbors3DNoGroup(const Point &point, float radius, int rId)
/**
 * Ignore points inside the specified region */
{
    Vector boxMin, boxMax;
    std::vector<Point *> ptsInside;

    makeBox(point, radius, boxMin, boxMax);
    ptsInside = this->neighbors3DNoGroup(point, boxMin, boxMax, ptsInside, rId);
    return ptsInside;
}

std::vector<Point *> Octree::neighbors3DNoGroup(const Point &point, Vector boxMin, Vector boxMax, std::vector<Point *> ptsInside, int rId) {
    int i = 0;

    if (this->isLeaf()) {
        if (!this->isEmpty()) {
            for (i = 0; (unsigned int)i < this->numPoints_; i++) {
                if (this->insideBox3D(*this->points_[i], boxMin, boxMax)) {
                    ptsInside.push_back(this->points_[i]);
                }
            }
        }
    } else {
        for (i = 0; i < 8; i++) {
            if (!this->octants_[i].boxOverlap3D(boxMin, boxMax)) {
                continue;
            } else {
                ptsInside = this->octants_[i].neighbors3DNoGroup(point, boxMin, boxMax, ptsInside, rId);
            }
        }
    }

    return ptsInside;
}

int Octree::numNeighbors2D(const Point &point, Vector boxMin, Vector boxMax, int *numInside) {
    if (this->isLeaf()) {
        if (!this->isEmpty()) {
            for (int i = 0; (unsigned int)i < this->points_.size(); i++) {
                if (this->insideBox2D(*this->points_[i], boxMin, boxMax)) {
                    ++*numInside;
                }
            }
        }
    } else {
        for (int i = 0; i < 8; i++) {
            if (!this->octants_[i].boxOverlap2D(boxMin, boxMax)) {
                continue;
            } else {
                *numInside = this->octants_[i].numNeighbors2D(point, boxMin, boxMax, numInside);
            }
        }
    }

    return *numInside;
}

int Octree::numOctreeNeighbors2D(const Point &point, float radius) {
    Vector boxMin, boxMax;
    int numInside = 0;

    makeBox(point, radius, boxMin, boxMax);
    numInside = numNeighbors2D(point, boxMin, boxMax, &numInside);

    return numInside;
}

std::vector<Point *> Octree::searchSphereNeighbors(const Point &point, float radius) {
    int i = 0;
    Vector boxMin, boxMax;
    std::vector<Point *> ptsInsideVoxel, ptsInsideSphere;

    makeBox(point, radius, boxMin, boxMax);
    ptsInsideVoxel = neighbors3D(point, boxMin, boxMax, ptsInsideVoxel);

    for (i = 0; (unsigned int)i < ptsInsideVoxel.size(); i++)
        if (point.distance3D(*ptsInsideVoxel[i]) < radius)
            ptsInsideSphere.push_back(ptsInsideVoxel[i]);

    return ptsInsideSphere;
}

std::vector<Point *> Octree::neighborsRing(const Point &p, Box &innerBox, Box &outerBox, std::vector<Point *> &ptsInside) const
/**
 * Function that searchs neighbors inside a ring.
 * @param p
 * @param innerBox Vector of Radii of the inside ring
 * @param outerBox Vector of Radii of the outside ring
 * @param ptsInside
 * @return
 */
{
    if (isLeaf()) {
        if (!isEmpty()) {
            for (Point *point_ptr : points_) {
                const Point &point = *point_ptr;
                if (insideRing(point, innerBox, outerBox)) {
                    ptsInside.emplace_back(point_ptr);
                }
            }
        }
    } else {
        for (const Octree &octant : octants_) {
            if (!octant.boxOverlap3D(outerBox.min(), outerBox.max())) {
                continue;
            } else {
                ptsInside = octant.neighborsRing(p, innerBox, outerBox, ptsInside);
            }
        }
    }

    return std::move(ptsInside);
}

std::vector<Point *> Octree::searchNeighborsRing(const Point &p, const Vector &innerRingRadii, const Vector &outerRingRadii)
/**
 * A point is considered to be inside a Ring around a point if its outside the innerRing and inside the outerRing
 * @param p
 * @param innerRingRadius
 * @param outerRingRadius
 * @return
 */
{
    std::vector<Point *> ptsInside;

    Box innerBox(p, innerRingRadii);
    Box outerBox(p, outerRingRadii);

    ptsInside = neighborsRing(p, innerBox, outerBox, ptsInside);
    return ptsInside;
}

int Octree::numNeighbors2DDiffGroup(const Point &point, Vector &boxMin, Vector &boxMax, int &numInside) {
    int i = 0;

    if (this->isLeaf()) {
        if (this->isEmpty()) {
            for (i = 0; (unsigned int)i < this->numPoints_; i++) {
                if (insideBox2D(*this->points_[i], boxMin, boxMax)) {
                    ++numInside;
                }
            }
        }
    } else {
        for (i = 0; i < 8; i++) {
            if (!boxOverlap2D(boxMin, boxMax)) {
                continue;
            } else {
                numInside = numNeighbors2DDiffGroup(point, boxMin, boxMax, numInside);
            }
        }
    }

    return numInside;
}

int Octree::numOctreeNeighbors2DDiffGroup(const Point &point, float radius) {
    Vector boxMin, boxMax;
    int ptsInside = 0;

    makeBox(point, radius, boxMin, boxMax);
    numNeighbors2DDiffGroup(point, boxMin, boxMax, ptsInside);

    return ptsInside;
}

/** Calculate the radius in each axis and save the max radius of the bounding box */
Vector mbbRadii(Vector &min, Vector &max, float &maxRadius) {
    double x = (max.getZ() - min.getZ()) / 2.0;
    double y = (max.getY() - min.getY()) / 2.0;
    double z = (max.getX() - min.getX()) / 2.0;

    Vector radii(x, y, z);

    if (radii.getZ() >= radii.getY() && radii.getZ() >= radii.getX()) {
        maxRadius = radii.getZ();
    } else if (radii.getY() >= radii.getZ() && radii.getY() >= radii.getX()) {
        maxRadius = radii.getY();
    } else {
        maxRadius = radii.getX();
    }

    return radii;
}

/** Calculate the center of the bounding box */
Vector mbbCenter(Vector &min, Vector &radius) {
    Vector center(min.getZ() + radius.getZ(), min.getY() + radius.getY(), min.getX() + radius.getX());

    return center;
}

Vector mbb(const std::vector<Point> &points, float &maxRadius)
/**
 * Computes the minimum bounding box of a set of points
 * @param points Array of points
 * @param numPoints Number of points
 * @param[out] maxRadius Maximum radius of the bounding box
 * @return (Vector) center of the bounding box
 */
{
    Vector center, min, max, radii;

    min.setZ(std::numeric_limits<double>::max());
    min.setY(std::numeric_limits<double>::max());
    min.setX(std::numeric_limits<double>::max());

    max.setZ(-std::numeric_limits<double>::max());
    max.setY(-std::numeric_limits<double>::max());
    max.setX(-std::numeric_limits<double>::max());

    for (const Point &p : points) {
        if (p.getZ() < min.getZ())
            min.setZ(p.getZ());
        if (p.getZ() > max.getZ())
            max.setZ(p.getZ());
        if (p.getY() < min.getY())
            min.setY(p.getY());
        if (p.getY() > max.getY())
            max.setY(p.getY());
        if (p.getX() < min.getX())
            min.setX(p.getX());
        if (p.getX() > max.getX())
            max.setX(p.getX());
    }

    radii = mbbRadii(min, max, maxRadius);
    center = mbbCenter(min, radii);
    DEBUG_STDOUT("Octree Radius: " << maxRadius);

    return center;
}

void Octree::writeOctree(std::ofstream &f, size_t index) const {
    index++;
    f << "Depth: " << index << " "
      << "numPoints: " << numPoints_ << "\n";
    f << "Center: " << center_ << " Radius: " << radius_ << "\n";

    if (isLeaf()) {
        for (const auto &p : points_) {
            f << "\t " << p << "\n";
        }
    } else {
        for (const auto &octant : octants_) {
            octant.writeOctree(f, index);
        }
    }
}

// BOX
Box::Box(const Point &p, const Vector &radii) {
    min_.setZ(p.getZ() - radii.getZ());
    min_.setY(p.getY() - radii.getY());
    min_.setX(p.getX() - radii.getX());
    max_.setZ(p.getZ() + radii.getZ());
    max_.setY(p.getY() + radii.getY());
    max_.setX(p.getX() + radii.getX());
}

bool Box::isInside(const Point &p) const {
    if (p.getZ() > min_.getZ() && p.getY() > min_.getY() && p.getX() > min_.getX()) {
        if (p.getZ() < max_.getZ() && p.getY() < max_.getY() && p.getX() < max_.getX()) {
            return true;
        }
    }
    return false;
}
