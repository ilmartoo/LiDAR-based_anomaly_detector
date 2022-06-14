/**
 * @file models_test.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 10/05/2022
 *
 * Test para el módulo de funcionalidades globales
 *
 */

#include "catch.hpp"
#include "catch_utils.hh"

#include <vector>

#include "app/config.h"

#include "models/BBox.hh"
#include "models/Geometry.hh"
#include "models/Kernel.hh"
#include "models/Octree.hh"
#include "models/OctreeMap.hh"
#include "models/Point.hh"
#include "models/Timestamp.hh"

class ModelsFixture {
   public:
    std::vector<Point> v;
    std::vector<Point> xplane;

    ModelsFixture() {
        v.push_back({2, 2, 2});
        v.push_back({1, 1, 1});
        v.push_back({0, 0, 0});
        v.push_back({-1, -1, -1});

        for (int i = 0; i < 100; ++i) {
            Point p = {0, i / 10, i % 10};
            xplane[i] = p;
        }
    }
};

TEST_CASE_METHOD(ModelsFixture, "2.1, 2.2, 2.3", "[BBox]") {
    std::vector<Point> vv, v2;
    v2.push_back({2, 2, 2});
    v2.push_back({1, 1, 1});
    v2.push_back({0, 0, 0});
    v2.push_back({-1, -1, -1});

    BBox b(v);
    BBox bv(vv);
    BBox b2(v2);

    CHECK(b.getDelta() != Point(0, 0, 0));
    CHECK(bv.getDelta() == Point(0, 0, 0));
    CHECK(b2.volume() == 8.0);
}

TEST_CASE_METHOD(ModelsFixture, "2.4, 2.5, 2.6", "[Timestamp]") {
    uint8_t ui8t[8] = {
        0x64,
        0xE8,
        0x76,
        0x48,
        0x17,
        0,
        0,
        0};
    Timestamp t1(ui8t);
    Timestamp t2("100000000100");
    Timestamp t3(0, 1000000001);

    CHECK(((t1.getSeconds() == 100) && (t1.getNanoseconds() == 100)));
    CHECK(((t2.getSeconds() == 100) && (t2.getNanoseconds() == 100)));
    CHECK(((t3.getSeconds() == 1) && (t3.getNanoseconds() == 1)));
}

TEST_CASE_METHOD(ModelsFixture, "2.7, 2.8, 2.9, 2.10, 2.11, 2.12, 2.17", "[Point][Geometry]") {
    Point p000(0, 0, 0);
    Point p001(0, 0, 1);
    Point p010(0, 1, 0);
    Point p100(1, 0, 0);
    Point p0n13(0, -1, 3);
    Point p21n2(2, 1, -2);

    arma::mat33 rot90X = Geometry::rotationMatrix(90, 0, 0);
    arma::mat33 result = {{1, 0, 0}, {0, 0, 1}, {0, -1, 0}};

    Point rotated = p010.rotate(rot90X);

    // ROTMATRIX
    CHECK(std::fabs(result(0, 0) - rot90X(0, 0)) <= std::numeric_limits<double>::epsilon() * 100);
    CHECK(std::fabs(result(0, 1) - rot90X(0, 1)) <= std::numeric_limits<double>::epsilon() * 100);
    CHECK(std::fabs(result(0, 2) - rot90X(0, 2)) <= std::numeric_limits<double>::epsilon() * 100);
    CHECK(std::fabs(result(1, 0) - rot90X(1, 0)) <= std::numeric_limits<double>::epsilon() * 100);
    CHECK(std::fabs(result(1, 1) - rot90X(1, 1)) <= std::numeric_limits<double>::epsilon() * 100);
    CHECK(std::fabs(result(1, 2) - rot90X(1, 2)) <= std::numeric_limits<double>::epsilon() * 100);
    CHECK(std::fabs(result(2, 0) - rot90X(2, 0)) <= std::numeric_limits<double>::epsilon() * 100);
    CHECK(std::fabs(result(2, 1) - rot90X(2, 1)) <= std::numeric_limits<double>::epsilon() * 100);
    CHECK(std::fabs(result(2, 2) - rot90X(2, 2)) <= std::numeric_limits<double>::epsilon() * 100);

    // POINT ROTATION
    CHECK(std::fabs(rotated.getX() - p001.getX()) <= std::numeric_limits<double>::epsilon() * 100);
    CHECK(std::fabs(rotated.getY() - p001.getY()) <= std::numeric_limits<double>::epsilon() * 100);
    CHECK(std::fabs(rotated.getZ() - p001.getZ()) <= std::numeric_limits<double>::epsilon() * 100);

    CHECK(p000.distance3D(p21n2) == 3.0);

    CHECK(p21n2.module() == 3.0);

    CHECK(p0n13.scalarProduct(p21n2) == -7.0);

    CHECK(p010.crossProduct(p21n2) == Point(-2, 0, -2));

    CHECK(p010.vectorialAngle(p100) == 90 * RAD_PER_DEG);
}

TEST_CASE_METHOD(ModelsFixture, "2.13, 2.14, 2.15, 2.16, 2.18", "[Geometry]") {
    std::vector<Point> v, vm, vb;
    v.push_back({-1, -1, -1});
    v.push_back({1, 1, 1});

    vm.push_back({1, 0, 1});
    vm.push_back({-1, 2, 1});

    vb.push_back({2, 2, 2});
    vb.push_back({1, 1, 1});
    vb.push_back({0, 0, 0});
    vb.push_back({-1, -1, -1});

    Point p003(0, 0, 3);

    arma::vec4 plane = Geometry::computePlane(xplane);
    arma::vec4 xaxis = {1, 0, 0, 0};

    auto rbb = Geometry::minimumBBox(vb);

    CHECK(Geometry::computeCentroid(v) == Point(0, 0, 0));

    CHECK(Geometry::computeNormal(xplane) == Point(1, 0, 0));

    // PLANE COMPUTATION
    CHECK(std::fabs(xaxis(0) - plane(0)) <= std::numeric_limits<double>::epsilon() * 100);
    CHECK(std::fabs(xaxis(1) - plane(1)) <= std::numeric_limits<double>::epsilon() * 100);
    CHECK(std::fabs(xaxis(2) - plane(2)) <= std::numeric_limits<double>::epsilon() * 100);
    CHECK(std::fabs(xaxis(3) - plane(3)) <= std::numeric_limits<double>::epsilon() * 100);

    CHECK(Geometry::mean(vm) == Point(0, 1, 1));

	// MINIMUM BBOX
    CHECK(std::fabs(rbb.first.getDelta().getX() - p003.getX()) <= std::numeric_limits<double>::epsilon() * 100);
    CHECK(std::fabs(rbb.first.getDelta().getY() - p003.getY()) <= std::numeric_limits<double>::epsilon() * 100);
    CHECK(std::fabs(rbb.first.getDelta().getZ() - p003.getZ()) <= std::numeric_limits<double>::epsilon() * 100);
}

TEST_CASE_METHOD(ModelsFixture, "2.19, 2.20", "[Octree][OctreeMap]") {
    Octree oc(xplane);
    OctreeMap om;

    om.insert(Point(0, 0, 0));
    om.insert(Point(0, 0, 0));

    CHECK(oc.searchNeighbors(Point(5, 5, 5), 1, Kernel_t::sphere).size() == (4 + 1));
    CHECK(om.getPoints().size() == 1);
}