/**
 * @file ObjectCharacterizator.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 25/03/2022
 *
 * @brief Implementación del objeto ObjectCharacterizator
 *
 */

#include <string>
#include <thread>
#include <vector>
#include <ctime>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <utility>
#include <omp.h>

#include "object_characterization/ObjectCharacterizator.hh"
#include "object_characterization/CharacterizedObject.hh"
#include "models/LidarPoint.hh"
#include "models/Point.hh"
#include "app/CLICommand.hh"
#include "app/config.h"

#include "logging/debug.hh"

void ObjectCharacterizator::newPoint(const LidarPoint &p) {
    static std::chrono::system_clock::time_point start, last_point, end;
    static uint32_t p_count;

    if (p.getReflectivity() >= minReflectivity) {
        switch (state) {
            case defBackground: {
                // Primer punto del marco temporal
                if (!background.getStartTime().first) {
                    DEBUG_STDOUT("First background point timestamp: " << p.getTimestamp().string());

                    if (chrono) {
                        start = std::chrono::high_resolution_clock::now();
                    }

                    p_count = 0;
                    background.setStartTime(p.getTimestamp());
                }

                // Punto dentro del marco temporal
                if (background.getStartTime().second + backFrame > p.getTimestamp()) {
                    DEBUG_POINT_STDOUT("Point added to the background: " << p.string());

                    ++p_count;
                    background.insert(p);
                }

                // Punto fuera del marco temporal
                else {
                    state = defStopped;

                    if (chrono) {
                        last_point = std::chrono::high_resolution_clock::now();
                    }

                    background.buildOctree();

                    if (chrono) {
                        end = std::chrono::high_resolution_clock::now();

                        double sc_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(last_point - start).count()) / 1.e9;
                        double ot_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - last_point).count()) / 1.e9;

                        CLI_STDOUT("Background scanning lasted " << std::setprecision(6) << (sc_duration + ot_duration) << std::setprecision(2) << "s (scanning: " << sc_duration << "s, map generation: " << ot_duration << "s)");
                    }

                    DEBUG_STDOUT("First out-of-frame point timestamp: " << p.getTimestamp().string());

                    CLI_STDOUT("Scanned background contains " << p_count << " unique points");

                    scanner->pause();
                }
            } break;

            // Punto del objeto
            case defObject: {
                // Primer punto del marco temporal
                if (!object.getStartTime().first) {
                    if (chrono) {
                        start = std::chrono::high_resolution_clock::now();
                    }

                    p_count = 0;
                    object.setStartTime(p.getTimestamp());

                    DEBUG_STDOUT("First point timestamp: " << p.getTimestamp().string());
                }

                // Punto dentro del marco temporal
                if (object.getStartTime().second + objFrame > p.getTimestamp()) {
                    ++p_count;

                    DEBUG_POINT_STDOUT("Point added to the object: " << p.string());

                    ++p_count;
                    object.insert(p);
                }

                // Punto fuera del marco temporal
                else {
                    state = defStopped;

                    if (chrono) {
                        end = std::chrono::high_resolution_clock::now();

                        double total_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1.e9;

                        CLI_STDOUT("Object scanning lasted " << std::setprecision(6) << total_duration << std::setprecision(2) << "s");
                    }

                    DEBUG_STDOUT("First out-of-frame point timestamp: " << p.getTimestamp().string());

                    scanner->pause();
                }
            } break;

            // Descarte de puntos intencionado
            case defDiscard: {
                // Primer punto del marco temporal
                if (!discardStartTime.first) {
                    DEBUG_STDOUT("First discarded point timestamp: " << p.getTimestamp().string());

                    p_count = 0;
                    discardStartTime = {true, p.getTimestamp()};
                }

                // Punto dentro del marco temporal
                if (discardStartTime.second + discardTime > p.getTimestamp()) {
                    DEBUG_POINT_STDOUT("Point discarded: " << p.string());

                    ++p_count;
                }

                // Punto fuera del marco temporal
                else {
                    state = defStopped;

                    CLI_STDOUT("A total of " << p_count << " points where discarded during " << discardTime / 1000000 << "ms");

                    DEBUG_STDOUT("Last discarded point timestamp: " << p.getTimestamp().string() << ".");

                    scanner->pause();
                }
            } break;

            // Punto descartado
            case defStopped:
            default: {
                DEBUG_POINT_STDOUT("Point discarded: " << p.string());
            } break;
        }
    } else {
        DEBUG_POINT_STDOUT("Punto con reflectividad insuficiente: " << p.string());
    }
}

void ObjectCharacterizator::init() {
    DEBUG_STDOUT("Initializing characterizator");

    scanner->init();
    scanner->setCallback(([this](const LidarPoint &p) { this->newPoint(p); }));
};

void ObjectCharacterizator::stop() {
    DEBUG_STDOUT("Ending characterization");

    scanner->stop();

    DEBUG_STDOUT("Ended characterization");
}

void ObjectCharacterizator::defineBackground() {
    background = {};

    state = defBackground;

    switch (scanner->scan()) {
        case kScanOk:
            break;
        case kScanError:
            CLI_STDERR("An error ocurred while scanning: Scan will end");
            break;
        case kScanEof:
            CLI_STDERR("End Of File reached: Scan will end and file will be reset");
            break;
    }
}

std::pair<bool, CharacterizedObject> ObjectCharacterizator::defineObject() {
    object = {};

    state = defObject;

    switch (scanner->scan()) {
        case kScanOk:
            break;
        case kScanError:
            CLI_STDERR("An error ocurred while scanning: Scan will end");
            return {false, {}}; // Error de escaneo
            break;
        case kScanEof:
            CLI_STDERR("End Of File reached: Scan will end and file will be reset");
            break;
    }

    std::chrono::system_clock::time_point start, end;
    if (chrono) {
        start = std::chrono::high_resolution_clock::now();
    }

    // Object points filtering
    std::vector<Point> filtered;
#pragma omp parallel for num_threads(PARALELIZATION_NUM_THREADS) schedule(guided)
    for (size_t i = 0; i < object.getPoints().size(); ++i) {
        if (!isBackground(object.getPoints()[i])) {
#pragma omp critical
            {
                filtered.push_back(object.getPoints()[i]);
            }
        }
    }

    if (chrono) {
        end = std::chrono::high_resolution_clock::now();

        double duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1.e9;

        CLI_STDOUT("Object point filtering lasted " << std::setprecision(6) << duration << std::setprecision(2) << "s");
    }

    CLI_STDOUT("Scanned object contains " << filtered.size() << " unique points (a total of " << object.getPoints().size() << " points were scanned)");

    return CharacterizedObject::parse(filtered, chrono);
}

void ObjectCharacterizator::wait(uint32_t miliseconds) {
    discardTime = static_cast<uint64_t>(miliseconds) * 1000000;
    discardStartTime.first = false;

    state = defDiscard;

    switch (scanner->scan()) {
        case kScanOk:
            break;
        case kScanError:
            CLI_STDERR("An error ocurred while scanning: Scan will end");
            break;
        case kScanEof:
            CLI_STDERR("End Of File reached: Scan will end and file will be reset");
            break;
    }
}

bool ObjectCharacterizator::isBackground(const Point &p) const { return background.getMap().searchNeighbors(p, backDistance, Kernel_t::sphere).size() > 0; }