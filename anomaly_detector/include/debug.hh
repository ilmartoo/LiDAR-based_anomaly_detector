/**
 * @file debug_lbad.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 23/03/2022
 *
 * Header que define macros de debugging
 *
 */

#ifndef DEBUG_HEADER_H
#define DEBUG_HEADER_H

////////////////////////////
// Inutilizamos las macros de debugging
#define DEBUG_CODE(x)
#define DEBUG_STDOUT(x)
#define DEBUG_STDOUT(x)
#define DEBUG_STDERR(x)
#define DEBUG_POINT_STDOUT(x)
#define DEBUG_POINT_STDERR(x)
////////////////////////////

#include <iostream>
#include <string>

#include "app/string_format.h"

////////////////////////////

// Redefinimos las macros de debugging si procede
#ifdef DEBUG
#define DEBUG_CODE(x) x
#define DEBUG_STDOUT(x) do { std::cout << yellow_s("[DEBUG] " + std::string(x)) << std::endl; } while (0)
#define DEBUG_STDERR(x) do { std::cerr << red_s("[DEBUG] " + std::string(x)) << std::endl; } while (0)

#ifdef DEBUG_POINTS
#define DEBUG_POINT_STDOUT(x) do { std::cout << yellow_s("[DEBUG] " + std::string(x)) << std::endl; } while (0)
#define DEBUG_POINT_STDERR(x) do { std::cerr << red_s("[DEBUG] " + std::string(x)) << std::endl; } while (0)

#endif  // DEBUG_POINTS
#endif  // DEBUG
////////////////////////////

#endif  // DEBUG_HEADER_H