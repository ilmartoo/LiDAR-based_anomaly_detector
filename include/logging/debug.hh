/**
 * @file debug_lbad.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 23/03/2022
 *
 * @brief Header que define macros de debugging
 *
 */

#ifndef DEBUG_HEADER_H
#define DEBUG_HEADER_H

#include <iostream>
#include <string>

#include "logging/string_format.h"

#ifdef DEBUG
#define DEBUG_CODE(code) code
#define DEBUG_STDOUT(msg) do { std::cout << "[" yellow("DEBUG") "] " << msg << std::endl; } while (0)
#define DEBUG_STDERR(msg) do { std::cerr << "[" red("DEBUG") "] " << msg << std::endl; } while (0)

#ifdef DEBUG_POINTS
#define DEBUG_POINT_STDOUT(msg) do { std::cout << "[" yellow("DEBUG") "] " << msg << std::endl; } while (0)
#define DEBUG_POINT_STDERR(msg) do { std::cerr << "[" red("DEBUG") "] " << msg << std::endl; } while (0)
#else // DEBUG_POINTS
#define DEBUG_POINT_STDOUT(x)
#define DEBUG_POINT_STDERR(x)
#endif  // DEBUG_POINTS

#else // DEBUG
#define DEBUG_CODE(x)
#define DEBUG_STDOUT(x)
#define DEBUG_STDERR(x)
#define DEBUG_POINT_STDOUT(x)
#define DEBUG_POINT_STDERR(x)
#endif  // DEBUG

#endif  // DEBUG_HEADER_H