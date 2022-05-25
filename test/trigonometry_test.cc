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

#include "armadillo"

#include <vector>

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

TEST_CASE_METHOD(TrigonometryFixture, "normal vectors", "[trigonometry]") {
    Vector normal = PlaneUtils::computeNormal(xplane);

    CHECK_MESSAGE(normal == nx, "Normal computation is not acurate");
}

TEST_CASE_METHOD(TrigonometryFixture, "plane calculation", "[trigonometry]") {
    arma::vec4 plane = PlaneUtils::computePlane(xplane);
    arma::vec4 xaxis = {1, 0, 0, 0};

	// La computación de planos usa una función que es rápida a costa de algo de precisión, por eso le damos un margen de e-15 en vez de e-16
    CHECK_MESSAGE((std::fabs(xaxis[0] - plane[0]) <= std::numeric_limits<double>::epsilon() * 10 &&
                   std::fabs(xaxis[1] - plane[1]) <= std::numeric_limits<double>::epsilon() * 10 &&
                   std::fabs(xaxis[2] - plane[2]) <= std::numeric_limits<double>::epsilon() * 10 &&
                   std::fabs(xaxis[3] - plane[3]) <= std::numeric_limits<double>::epsilon() * 10),
                  "Normal computation is not acurate");
}