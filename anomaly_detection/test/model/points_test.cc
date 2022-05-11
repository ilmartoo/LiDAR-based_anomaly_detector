/**
 * @file points_test.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 10/05/2022
 *
 * Test para las clases Timestamp, Point y LidarPoint
 *
 */

#include "catch.hpp"

#include "models/Timestamp.hh"
#include "models/Point.hh"
#include "models/LidarPoint.hh"

class PointFixture {
   protected:
    Timestamp ta;
    Timestamp tb;
    Point pia;
    Point pib;
    Point pda;
    Point pdb;
    LidarPoint lpa;
    LidarPoint lpb;
    LidarPoint lpc;
    ;

   public:
    PointFixture() : ta("100000000100"),
                     tb(6, 100),
                     pia(0, 5, -5),
                     pib(6, 6, 6),
                     pda(0.0, 0.0, 0.0),
                     pdb(1.0, -1.1, 0.1),
                     lpa(Timestamp(0, 0), 100),
                     lpb(Point()),
                     lpc() {}
    ~PointFixture() {}
};

TEST_CASE_METHOD(PointFixture, "timestamp constructors", "[timestamp]") {
    uint8_t ui8t[8] = {0, 0, 0, 0x17, 0x48, 0x76, 0xE8, 0x64};
    Timestamp t1(ui8t);
    Timestamp t2(0, 1000000001);
    Timestamp t3(-1, -1);
    Timestamp t4(0.1, 1.5);

    REQUIRE_THROWS_WITH((ta.getSeconds() == 100) && (ta.getNanoseconds() == 100), "Constructor Timestamp(string) failed");
    REQUIRE_THROWS_WITH((tb.getSeconds() == 6) && (tb.getNanoseconds() == 100), "Constructor Timestamp(unt32_t, uint32_t) failed");
    REQUIRE_THROWS_WITH((t1.getSeconds() == 100) && (t1.getNanoseconds() == 100), "Constructor Timestamp(uint8_t[8]) failed");
    REQUIRE_THROWS_WITH((t2.getSeconds() == 1) && (t2.getNanoseconds() == 1), "Timestamp must not have seconds in the nanoseconds field");
    REQUIRE_THROWS_WITH((t3.getSeconds() != -1) && (t3.getNanoseconds() == -1), "Timestamps must not be negative");
    REQUIRE_THROWS_WITH((t4.getSeconds() != 0.1) && (t4.getNanoseconds() != 1.5), "Timestamps must not be decimal");
}

TEST_CASE_METHOD(PointFixture, "equals", "[timestamp]") {
    Timestamp t1(6, 100);
    Timestamp t2(100, 100);

    REQUIRE_THROWS_WITH(tb == t1, "Timestamps are equal");
    REQUIRE_THROWS_WITH(tb != t2, "Timestamps are not equal");
}

TEST_CASE_METHOD(PointFixture, "less than", "[timestamp]") {
    Timestamp t1(6, 100);
    Timestamp t2(100, 100);
    Timestamp t3(0, 0);

    REQUIRE_THROWS_WITH(!(tb < t1), "Timestamp is lesser");
    REQUIRE_THROWS_WITH(tb < t2, "Timestamp is not lesser");
    REQUIRE_THROWS_WITH(!(tb < t3), "Timestamp is lesser");
}

TEST_CASE_METHOD(PointFixture, "greater than", "[timestamp]") {
    Timestamp t1(6, 100);
    Timestamp t2(100, 100);
    Timestamp t3(0, 0);

    REQUIRE_THROWS_WITH(!(tb > t1), "Timestamp is greater");
    REQUIRE_THROWS_WITH(!(tb > t2), "Timestamp is greater");
    REQUIRE_THROWS_WITH(tb > t3, "Timestamp is not greater");
}

TEST_CASE_METHOD(PointFixture, "sum", "[timestamp]") {
    Timestamp t1 = ta + Timestamp(0, 0);
    Timestamp t2 = Timestamp(0, 1) + Timestamp(0, 999999999);

    REQUIRE_THROWS_WITH(t1 == ta, "Sum is not acurate");
    REQUIRE_THROWS_WITH(t2 == Timestamp(1, 0), "Sum is not acurate");
}

TEST_CASE_METHOD(PointFixture, "point constructors", "[point]") {
    Point p;
    LidarPoint lp(Timestamp(0, 0), -100);

    REQUIRE_THROWS_WITH((p.getX() == 0.0) && (p.getY() == 0.0) && (p.getZ() == 0.0), "Constructor Point() failed");
    REQUIRE_THROWS_WITH((pdb.getX() == 1.0) && (pdb.getY() == -1.1) && (pdb.getZ() == 0.1), "Constructor Point(double, double, double) failed");
    REQUIRE_THROWS_WITH((pia.getX() == 0.0) && (pia.getY() == 5.0) && (pia.getZ() == -5.0), "Constructor Point(int, int, int) failed");
    REQUIRE_THROWS_WITH((lpc.getTimestamp().getSeconds() == 0) && (lpc.getTimestamp().getNanoseconds() == 0) && (lpc.getReflectivity() == 0), "Constructor LidarPoint() failed");
    REQUIRE_THROWS_WITH((lpa.getTimestamp().getSeconds() == 0) && (lpa.getTimestamp().getNanoseconds() == 0), "Constructor LidarPoint(Timestamp, uint32_t) failed");
    REQUIRE_THROWS_WITH(lp.getReflectivity() != -100, "LidarPoint must not have negative reflectivity");
}

TEST_CASE_METHOD(PointFixture, "equals", "[point]") {
    Point p(6., 6., 6.);

    REQUIRE_THROWS_WITH(p == pib, "Points are not the same");
    REQUIRE_THROWS_WITH(p != pia, "Points are the same");
}

TEST_CASE_METHOD(PointFixture, "sum", "[point]") {
    Point p1 = pia + pda;
    Point p2 = pib + pdb;
    Point p3 = pdb + 6;

    REQUIRE_THROWS_WITH(p1 == Point(0.0, 5.0, -5.0), "Sum is not acurate");
    REQUIRE_THROWS_WITH(p2 == Point(7.0, 4.9, 6.1), "Sum is not acurate");
    REQUIRE_THROWS_WITH(p3 == Point(7.0, 4.9, 6.1), "Sum is not acurate");
}

TEST_CASE_METHOD(PointFixture, "substraction", "[point]") {
    Point p1 = pia - pda;
    Point p2 = pib - pdb;
    Point p3 = pdb - 6;

    REQUIRE_THROWS_WITH(p1 == Point(0.0, 5.0, -5.0), "Substraction is not acurate");
    REQUIRE_THROWS_WITH(p2 == Point(5.0, 7.1, 5.9), "Substraction is not acurate");
    REQUIRE_THROWS_WITH(p3 == Point(5.0, 7.1, 5.9), "Substraction is not acurate");
}

TEST_CASE_METHOD(PointFixture, "product", "[point]") {
    Point p1 = pia * pda;
    Point p2 = pib * pdb;
    Point p3 = pdb * 6;

    REQUIRE_THROWS_WITH(p1 == Point(0.0, 0.0, 0.0), "Product is not acurate");
    REQUIRE_THROWS_WITH(p2 == Point(6.0, -6.6, 0.6), "Product is not acurate");
    REQUIRE_THROWS_WITH(p3 == Point(6.0, -6.6, 0.6), "Product is not acurate");
}

TEST_CASE_METHOD(PointFixture, "division", "[point]") {
    Point p1 = pda / pia;
    Point p2 = pib / pdb;
    Point p3 = pdb / 6;

    REQUIRE_THROWS_WITH(p1 == Point(0.0, 0.0, 0.0), "Division is not acurate");
    REQUIRE_THROWS_WITH(p2 == Point(6.0, 6.0 / (-1.1), 60.0), "Division is not acurate");
    REQUIRE_THROWS_WITH(p3 == Point(6.0, 6.0 / (-1.1), 60.0), "Division is not acurate");
}