/**
 * @file catch_ext.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 12/05/2022
 * 
 * Inclusión de Catch2 y definición de macros necesarias
 * 
 */

#ifndef CATCH_EXTENSION_HEADER_H
#define CATCH_EXTENSION_HEADER_H

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#define CHECK_MESSAGE(cond, msg) do { INFO(msg); CHECK(cond); } while(0)
#define REQUIRE_MESSAGE(cond, msg) do { INFO(msg); REQUIRE(cond); } while(0)

#endif // CATCH_EXTENSION_HEADER_H