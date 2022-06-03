/**
 * @file catch_utils.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 25/05/2022
 * 
 * Definición de macros de testing
 * 
 */

#ifndef CATCH_UTILS_HEADER_H
#define CATCH_UTILS_HEADER_H

#include "catch.hpp"

#define CHECK_MESSAGE(cond, msg) do { INFO(msg); CHECK(cond); } while(0)
#define REQUIRE_MESSAGE(cond, msg) do { INFO(msg); REQUIRE(cond); } while(0)

#endif  // CATCH_UTILS_HEADER_H