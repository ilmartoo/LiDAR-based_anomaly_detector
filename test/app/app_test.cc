/**
 * @file app_test.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 10/05/2022
 *
 * Test para el módulo de la CLI
 *
 */

#include "catch.hpp"
#include "catch_utils.hh"

#include <string>

#include "app/CLICommand.hh"
#include "app/ObjectManager.hh"

class CLIFixture {
   public:

    CLIFixture() {
    }
};

TEST_CASE_METHOD(CLIFixture, "5.1, 5.2", "[CLICommand]") {
    CLICommand c1 = CLICommand::parse("chrono arg1 arg2");
    CLICommand c2 = CLICommand::parse("invalido");

    // 5.1
    CHECK(c1.isValid());
    CHECK(c1.numParams() == 2);
    // 5.2
    CHECK(!c2.isValid());
}

TEST_CASE_METHOD(CLIFixture, "5.3, 5.4", "[ObjectManager]") {
    ObjectManager om1;
    ObjectManager om2;

    auto r1 = om1.newObject(CharacterizedObject());
    bool r2 = om1.newObject("objeto", CharacterizedObject());

    // 5.3
    CHECK(r1.first);
    CHECK(r1.second == "object-0");
    // 5.4
    CHECK(r2);
}