/**
 * @file trigonometry_test.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 25/05/2022
 *
 * Test para las funciones de trigonometricas y vectoriales
 *
 */

#include "catch.hpp"
#include "catch_utils.hh"

#include <vector>

#include "armadillo"

#include "object_characterization/PlaneUtils.hh"
#include "models/Point.hh"

class TrigonometryFixture {
   protected:
    std::vector<Point> testpoints;
    std::vector<Point> xplane;
    Vector nx;
    Vector ny;
    Vector nz;

   public:
    TrigonometryFixture() : testpoints(1000, Point()),
                            xplane(100, Point()),
                            nx(1, 0, 0),
                            ny(0, 1, 0),
                            nz(0, 0, 1) {
        int i;
        for (i = 0; i < 100; ++i) {
            Point p = {0, i / 10, i % 10};
            testpoints[i] = p;
            xplane[i] = p;
        }
        for (; i < 1000; ++i) {
            testpoints[i] = {i / 100, (i / 10) % 10, i % 10};
        }
    }
    ~TrigonometryFixture() {}
};

TEST_CASE_METHOD(TrigonometryFixture, "normal computation", "[trigonometry]") {
    Vector normal = PlaneUtils::computeNormal(xplane);

    CHECK_MESSAGE(normal == nx, "Normal computation is not acurate enough");
}

TEST_CASE_METHOD(TrigonometryFixture, "centroid computation", "[trigonometry]") {
    Point centroidp = PlaneUtils::computeCentroid(testpoints);
    Point centroidx = PlaneUtils::computeCentroid(xplane);

    CHECK_MESSAGE(centroidp == Point(4.5, 4.5, 4.5), "Centroid computation is not acurate enough");
    CHECK_MESSAGE(centroidx == Point(0., 4.5, 4.5), "Centroid computation is not acurate enough");
}

TEST_CASE_METHOD(TrigonometryFixture, "plane calculation", "[trigonometry]") {
    arma::vec4 plane = PlaneUtils::computePlane(xplane);
    arma::vec4 xaxis = {1, 0, 0, 0};

    // La computación de planos usa una función que es rápida a costa de algo de precisión, por eso le damos un margen de e-14 en vez de e-16
    CHECK_MESSAGE((std::fabs(xaxis(0) - plane(0)) <= std::numeric_limits<double>::epsilon() * 100 &&
                   std::fabs(xaxis(1) - plane(1)) <= std::numeric_limits<double>::epsilon() * 100 &&
                   std::fabs(xaxis(2) - plane(2)) <= std::numeric_limits<double>::epsilon() * 100 &&
                   std::fabs(xaxis(3) - plane(3)) <= std::numeric_limits<double>::epsilon() * 100),
                  "Normal computation is not acurate enough");
}

TEST_CASE_METHOD(TrigonometryFixture, "rotation matrix calculation", "[trigonometry]") {
    arma::mat33 rot = PlaneUtils::rotationMatrix(90, 45, 0);
    double onesof2 = 1. / sqrt(2.);
    arma::mat33 result = {{onesof2, onesof2, 0}, {0, 0, -1}, {-onesof2, onesof2, 0}};

    // Debido a la imprecisión implicita en la conversión de grados a radianes, damos un margen de error de e-14 en vez de e-16
    CHECK_MESSAGE((std::fabs(result(0, 0) - rot(0, 0)) <= std::numeric_limits<double>::epsilon() * 100 &&
                   std::fabs(result(0, 1) - rot(0, 1)) <= std::numeric_limits<double>::epsilon() * 100 &&
                   std::fabs(result(0, 2) - rot(0, 2)) <= std::numeric_limits<double>::epsilon() * 100 &&
                   std::fabs(result(1, 0) - rot(1, 0)) <= std::numeric_limits<double>::epsilon() * 100 &&
                   std::fabs(result(1, 1) - rot(1, 1)) <= std::numeric_limits<double>::epsilon() * 100 &&
                   std::fabs(result(1, 2) - rot(1, 2)) <= std::numeric_limits<double>::epsilon() * 100 &&
                   std::fabs(result(2, 0) - rot(2, 0)) <= std::numeric_limits<double>::epsilon() * 100 &&
                   std::fabs(result(2, 1) - rot(2, 1)) <= std::numeric_limits<double>::epsilon() * 100 &&
                   std::fabs(result(2, 2) - rot(2, 2)) <= std::numeric_limits<double>::epsilon()),
                  "Rotation matrix calculation is not acurate enough");
}

TEST_CASE_METHOD(TrigonometryFixture, "point rotation", "[trigonometry][point]") {
    Point rotated = Point(5, 10, 7).rotate(PlaneUtils::rotationMatrix(90, 90, 90));
    Point result(7, 10, -5);

    // La computación de planos usa una función que es rápida a costa de algo de precisión, por eso le damos un margen de e-15 en vez de e-16
    CHECK_MESSAGE((std::fabs(rotated.getX() - result.getX()) <= std::numeric_limits<double>::epsilon() * 100 &&
                   std::fabs(rotated.getY() - result.getY()) <= std::numeric_limits<double>::epsilon() * 100 &&
                   std::fabs(rotated.getZ() - result.getZ()) <= std::numeric_limits<double>::epsilon() * 100),
                  "Point rotation is not acurate enough");
}