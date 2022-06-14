/**
 * @file anomaly_test.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 10/05/2022
 *
 * Test para el módulo de la CLI
 *
 */

#include "catch.hpp"
#include "catch_utils.hh"

#include "anomaly_detection/AnomalyDetector.hh"
#include "object_characterization/CharacterizedObject.hh"

class AnomalyFixture {
   public:
    CharacterizedObject co1;
    CharacterizedObject co2;

    AnomalyFixture() : co1(CharacterizedObject::load("../../test/test_object1").second),
                       co2(CharacterizedObject::load("../../test/test_object2").second) {}
};

TEST_CASE_METHOD(AnomalyFixture, "4.1, 4.2", "[AnomalyDetector]") {
    AnomalyDetector adf(false);
    AnomalyDetector adt(true);

    CHECK(adf.compare(co1, co1).similar);
    CHECK(!adt.compare(co1, co2).similar);
}