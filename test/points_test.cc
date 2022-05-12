/**
 * @file points_test.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 10/05/2022
 *
 * Test para las clases Timestamp, Point y LidarPoint
 *
 */

#include "catch_ext.hh"

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
    Timestamp t2(0, 1000000001);
    Timestamp t3(-1, -1);
    Timestamp t4(0.1, 1.5);

    CHECK_MESSAGE(((ta.getSeconds() == 100) && (ta.getNanoseconds() == 100)), "Constructor Timestamp(string) failed");
    CHECK_MESSAGE(((tb.getSeconds() == 6) && (tb.getNanoseconds() == 100)), "Constructor Timestamp(unt32_t, uint32_t) failed");
    CHECK_MESSAGE(((t1.getSeconds() == 100) && (t1.getNanoseconds() == 100)), "Constructor Timestamp(uint8_t[8]) failed");
    CHECK_MESSAGE(((t2.getSeconds() == 1) && (t2.getNanoseconds() == 1)), "Timestamp must not have seconds in the nanoseconds field");
    CHECK_MESSAGE(((t3.getSeconds() >= 0) && (t3.getNanoseconds() >= 0)), "Timestamps must not be negative");
    CHECK_MESSAGE(((t4.getSeconds() != 0.1) && (t4.getNanoseconds() != 1.5)), "Timestamps must not be decimal");
}

TEST_CASE_METHOD(PointFixture, "timestamp equals", "[timestamp]") {
    Timestamp t1(6, 100);
    Timestamp t2(100, 100);

    CHECK_MESSAGE(tb == t1, "Timestamps are equal");
    CHECK_MESSAGE(tb != t2, "Timestamps are not equal");
}

TEST_CASE_METHOD(PointFixture, "timestamp less than", "[timestamp]") {
    Timestamp t1(6, 100);
    Timestamp t2(100, 100);
    Timestamp t3(0, 0);

    CHECK_MESSAGE(!(tb < t1), "Timestamp is lesser");
    CHECK_MESSAGE(tb < t2, "Timestamp is not lesser");
    CHECK_MESSAGE(!(tb < t3), "Timestamp is lesser");
}

TEST_CASE_METHOD(PointFixture, "timestamp greater than", "[timestamp]") {
    Timestamp t1(6, 100);
    Timestamp t2(100, 100);
    Timestamp t3(0, 0);

    CHECK_MESSAGE(!(tb > t1), "Timestamp is greater");
    CHECK_MESSAGE(!(tb > t2), "Timestamp is greater");
    CHECK_MESSAGE(tb > t3, "Timestamp is not greater");
}

TEST_CASE_METHOD(PointFixture, "timestamp sum", "[timestamp]") {
    Timestamp t1 = ta + Timestamp(0, 0);
    Timestamp t2 = Timestamp(0, 1) + Timestamp(0, 999999999);

    CHECK_MESSAGE(t1 == ta, "Sum is not acurate");
    CHECK_MESSAGE(t2 == Timestamp(1, 0), "Sum is not acurate");
}

TEST_CASE_METHOD(PointFixture, "point constructors", "[point]") {
    Point p;
    LidarPoint lp(Timestamp(0, 0), -100);

    CHECK_MESSAGE(((p.getX() == 0.0) && (p.getY() == 0.0) && (p.getZ() == 0.0)), "Constructor Point() failed");
    CHECK_MESSAGE(((pdb.getX() == 1.0) && (pdb.getY() == -1.1) && (pdb.getZ() == 0.1)), "Constructor Point(double, double, double) failed");
    CHECK_MESSAGE(((pia.getX() == 0.0) && (pia.getY() == 5.0) && (pia.getZ() == -5.0)), "Constructor Point(int, int, int) failed");
    CHECK_MESSAGE(((lpc.getTimestamp().getSeconds() == 0) && (lpc.getTimestamp().getNanoseconds() == 0) && (lpc.getReflectivity() == 0)), "Constructor LidarPoint() failed");
    CHECK_MESSAGE(((lpa.getTimestamp().getSeconds() == 0) && (lpa.getTimestamp().getNanoseconds() == 0)), "Constructor LidarPoint(Timestamp, uint32_t) failed");
    CHECK_MESSAGE(lp.getReflectivity() >= 0, "LidarPoint must not have negative reflectivity");
}

TEST_CASE_METHOD(PointFixture, "point equals", "[point]") {
    Point p(6., 6., 6.);

    CHECK_MESSAGE(p == pib, "Points are not the same");
    CHECK_MESSAGE(p != pia, "Points are the same");
}

TEST_CASE_METHOD(PointFixture, "point sum", "[point]") {
    Point p1 = pia + pda;
    Point p2 = pib + pdb;
    Point p3 = pdb + 6;

    CHECK_MESSAGE(p1 == Point(0.0, 5.0, -5.0), "Sum is not acurate");
    CHECK_MESSAGE(p2 == Point(7.0, 4.9, 6.1), "Sum is not acurate");
    CHECK_MESSAGE(p3 == Point(7.0, 4.9, 6.1), "Sum is not acurate");
}

TEST_CASE_METHOD(PointFixture, "point substraction", "[point]") {
    Point p1 = pia - pda;
    Point p2 = pib - pdb;
    Point p3 = pdb - 6;

    CHECK_MESSAGE(p1 == Point(0.0, 5.0, -5.0), "Substraction is not acurate");
    CHECK_MESSAGE(p2 == Point(5.0, 7.1, 5.9), "Substraction is not acurate");
    CHECK_MESSAGE(p3 == Point(-5.0, -7.1, -5.9), "Substraction is not acurate");
}

TEST_CASE_METHOD(PointFixture, "point product", "[point]") {
    Point p1 = Point(0, 0, 0) * pda;
    Point p2 = pib * pdb;
    Point p3 = pdb * 6.0;

    CHECK_MESSAGE(((p1.getX() < 1e-16 && p1.getX() > -1e-16) && (p1.getY() < 1e-16 && p1.getY() > -1e-16) && (p1.getZ() < 1e-16 && p1.getZ() > -1e-16)), "Product is not acurate");
    CHECK_MESSAGE(p2 == Point(1.0 * 6.0, -1.1 * 6.0, 0.1 * 6.0), "Product is not acurate");
    CHECK_MESSAGE(p3 == Point(1.0 * 6.0, -1.1 * 6.0, 0.1 * 6.0), "Product is not acurate");
}

TEST_CASE_METHOD(PointFixture, "point division", "[point]") {
    Point p1 = pda / pdb;
    Point p2 = pib / pdb;
    Point p3 = pdb / 6.0;

    CHECK_MESSAGE(((p1.getX() < 1e-16 && p1.getX() > -1e-16) && (p1.getY() < 1e-16 && p1.getY() > -1e-16) && (p1.getZ() < 1e-16 && p1.getZ() > -1e-16)), "Division is not acurate");
    CHECK_MESSAGE(p2 == Point(6.0 / 1.0, 6.0 / (-1.1), 6.0 / 0.1), "Division is not acurate");
    CHECK_MESSAGE(p3 == Point(1.0 / 6.0, (-1.1) / 6.0, 0.1 / 6.0), "Division is not acurate");
}