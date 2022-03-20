/**
 * @file App.cpp
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 20/03/2022
 *
 * Implementación de la clase de gestión de la aplicación
 *
 */

#include <stdint.h>

#include "app/App.hpp"
#include "app/InputParser.hpp"

App::App(TimerMode timerMode, const std::string &filename) : timerMode(timerMode) {
    scanner = ScannerFile(filename);
    // oc = ...
    // ad = ...
}

App::App(TimerMode timerMode, const std::string &id) : timerMode(timerMode) {
    // scanner = ...
    // oc = ...
    // ad = ...
}

void App::start() {}

void App::start(uint32_t seconds) {}

void App::stop() {}