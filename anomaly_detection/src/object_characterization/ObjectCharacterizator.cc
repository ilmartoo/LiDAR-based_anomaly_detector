/**
 * @file ObjectCharacterizator.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 25/03/2022
 *
 * Implementación del objeto ObjectCharacterizator
 *
 */

#include <string>
#include <thread>
#include <vector>
#include <ctime>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <condition_variable>

#include "object_characterization/ObjectCharacterizator.hh"
#include "object_characterization/CharacterizedObject.hh"
#include "object_characterization/ObjectManager.hh"
#include "models/Point.hh"
#include "models/Kernel.hh"
#include "models/CLICommand.hh"

#include "logging/debug.hh"

void ObjectCharacterizator::newPoint(const Point &p) {
    static uint32_t p_count;
    static std::chrono::system_clock::time_point start, last_point, end;

    if (p.getReflectivity() >= minReflectivity) {
        switch (state) {
            case defBackground: {
                // Primer punto del marco temporal
                if (!background->getStartTime().first) {
                    DEBUG_STDOUT("First background point timestamp: " << p.getTimestamp().string());

                    if (chrono) {
                        start = std::chrono::high_resolution_clock::now();
                    }

                    p_count = 1;
                    background->insert(p);
                }

                // Punto dentro del marco temporal
                else if (background->getStartTime().second + backFrame > p.getTimestamp()) {
                    DEBUG_POINT_STDOUT("Point added to the background: " << p.string());

                    ++p_count;
                    background->insert(p);
                }

                // Punto fuera del marco temporal
                else {
                    state = defStopped;

                    if (chrono) {
                        last_point = std::chrono::high_resolution_clock::now();
                    }

                    background->buildOctree();

                    if (chrono) {
                        end = std::chrono::high_resolution_clock::now();

                        double point_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(last_point - start).count()) / 1.e9;
                        double total_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1.e9;

                        CLI_STDOUT("Background characterization lasted " << std::setprecision(6) << total_duration << std::setprecision(2) << " s (Process speed: " << p_count / point_duration << " points/s)");
                    }

                    CLI_STDOUT("Defined background contains " << p_count << " unique points");

                    DEBUG_STDOUT("First out-of-frame point timestamp: " << p.getTimestamp().string());

                    scanner->pause();
                }
            } break;

            // Punto del objeto
            case defObject: {
                // Primer punto del marco temporal
                if (!object->getStartTime().first) {
                    if (chrono) {
                        start = std::chrono::high_resolution_clock::now();
                    }

                    if (!isBackground(p)) {
                        DEBUG_STDOUT("First object point timestamp: " << p.getTimestamp().string());

                        p_count = 1;
                        object->insert(p);
                    }
                }

                // Punto dentro del marco temporal
                else if (object->getStartTime().second + objFrame > p.getTimestamp()) {
                    if (!isBackground(p)) {
                        DEBUG_POINT_STDOUT("Point added to the object: " << p.string());

                        ++p_count;
                        object->insert(p);
                    }
                }

                // Punto fuera del marco temporal
                else {
                    state = defStopped;

                    if (chrono) {
                        last_point = std::chrono::high_resolution_clock::now();
                    }

                    object->buildOctree();

                    CLI_STDOUT("Defined object contains " << p_count << " unique points");

                    if (chrono) {
                        end = std::chrono::high_resolution_clock::now();

                        double point_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(last_point - start).count()) / 1.e9;
                        double total_duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1.e9;

                        CLI_STDOUT("Object characterization lasted " << std::setprecision(6) << total_duration << std::setprecision(2) << " s (Process speed: " << p_count / point_duration << " points/s)");
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

                    p_count = 1;
                    discardStartTime = {true, p.getTimestamp()};
                }

                // Punto dentro del marco temporal
                else if (discardStartTime.second + discardTime > p.getTimestamp()) {
                    DEBUG_POINT_STDOUT("Point discarded: " << p.string());

                    ++p_count;
                }

                // Punto fuera del marco temporal
                else {
                    state = defStopped;

                    discardStartTime.first = false;

                    CLI_STDOUT("A total of " << p_count << " points where discarded during " << discardTime / 1000000 << " ms");

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
    scanner->setCallback(([this](const Point &p) { this->newPoint(p); }));
};

void ObjectCharacterizator::stop() {
    DEBUG_STDOUT("Ending characterization");

    scanner->stop();
    
    DEBUG_STDOUT("Ended characterization");
}

void ObjectCharacterizator::defineBackground() {
    background->clear();

    state = defBackground;

    scanner->scan();
}

CharacterizedObject ObjectCharacterizator::defineObject() {
    object->clear();

    state = defObject;

    scanner->scan();

    return CharacterizedObject(*object);
}

void ObjectCharacterizator::wait(uint32_t miliseconds) {
    discardTime = miliseconds * 1000000;

    state = defDiscard;

    scanner->scan();
}

bool ObjectCharacterizator::isBackground(const Point &p) const {
    std::vector<Point *> neighbours = background->getMap().searchNeighbors(p, backDistance, Kernel_t::circle);

    for (Point *&pb : neighbours) {
        if (p.distance3D(*pb) < backDistance) {
            return true;  // Pertenece al background
        }
    }
    return false;  // No pertenece al background
}