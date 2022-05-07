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
#include "models/CharacterizedObject.hh"
#include "models/ObjectManager.hh"
#include "models/Point.hh"
#include "models/Kernel.hh"

#include "logging/debug.hh"
#include "logging/logging.hh"

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

                        LOG_INFO("Background characterization lasted " << std::fixed << total_duration << "s (Process speed: " << std::setprecision(2) << p_count / point_duration << std::setprecision(6) << " points/s).");
                    }

                    LOG_INFO("Defined background contains " << p_count << " unique points.");

                    DEBUG_STDOUT("First out-of-background-frame point timestamp: " << p.getTimestamp().string() << ".");

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

                    DEBUG_STDOUT("First object point timestamp: " << p.getTimestamp().string());

                    p_count = 1;
                    object->insert(p);
                }

                // Punto dentro del marco temporal
                else if (object->getStartTime().second + backFrame > p.getTimestamp()) {
                    DEBUG_POINT_STDOUT("Point added to the object: " << p.string());

                    ++p_count;
                    object->insert(p);
                }

                // Punto fuera del marco temporal
                else {
                    state = defStopped;

                    object->buildOctree();

                    LOG_INFO("Defined object contains " << p_count << " unique points.");

                    if (chrono) {
                        end = std::chrono::high_resolution_clock::now();
                        double seconds = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1.e9;

                        LOG_INFO("Object characterization lasted " << std::fixed << seconds << "s (Process speed: " << std::setprecision(2) << p_count / seconds << std::setprecision(6) << " points/s).");
                    }

                    DEBUG_STDOUT("First out-of-object-frame point timestamp: " << p.getTimestamp().string() << ".");

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
    DEBUG_STDOUT("Inicializando el caracterizador.");

    scanner->init();
    scanner->setCallback(([this](const Point &p) { this->newPoint(p); }));
};

void ObjectCharacterizator::stop() {
    DEBUG_STDOUT("Finalizando caracterización.");

    scanner->stop();

    // DEBUG_CODE({  // Impresión de los puntos a archivos csv
    //     std::ofstream os("tmp/object.csv", std::ios::out);
    //     while (!object->empty()) {
    //         os << object->front().csv_string() << std::endl;
    //         object->pop();
    //     }
    //     os.close();
    //     os.open("tmp/background.csv", std::ios::out);
    //     for (auto &p : *background) {
    //         os << p.csv_string() << std::endl;
    //     }
    //     os.close();
    // })

    DEBUG_STDOUT("Finalizada caracterización.");
}

void ObjectCharacterizator::defineBackground() {
    state = defBackground;

    std::condition_variable cv;

    if (scanner->scan(cv, mtx)) {
        std::unique_lock<std::mutex> lock(mtx);

        cv.wait(lock, [this] { return !this->scanner->isScanning(); });  // Esperamos a que termine el escaneo
        lock.unlock();

        mtx.unlock();
    }
}

CharacterizedObject ObjectCharacterizator::defineObject(const std::string &name) {
    state = defObject;

    std::condition_variable cv;

    if (scanner->scan(cv, mtx)) {
        std::unique_lock<std::mutex> lock(mtx);

        cv.wait(lock, [this] { return !this->scanner->isScanning(); });  // Esperamos a que termine el escaneo
        lock.unlock();
    }

    return CharacterizedObject(name, object->getMap());
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