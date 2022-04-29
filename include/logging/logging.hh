/**
 * @file logging.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 17/04/2022
 *
 * Header que define macros de logging
 *
 */

#ifndef LOGGING_HEADER_H
#define LOGGING_HEADER_H

#include <iostream>
#include <string>

#include "logging/string_format.h"

#define LOG_INFO(msg) do { std::cout << "[" green("INFO") "] " << msg << std::endl; } while (0)
#define LOG_ERROR(msg) do { std::cerr << "[" red("ERROR") "] " << msg << std::endl; } while (0)
#define LOG_WARN(msg) do { std::cout << "[" yellow("WARN") "] " << msg << std::endl; } while (0)

#endif  // LOGGING_HEADER_H
