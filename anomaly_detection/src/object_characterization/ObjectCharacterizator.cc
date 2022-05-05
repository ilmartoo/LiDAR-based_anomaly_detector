/**
 * @file ObjectCharacterizator.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 25/03/2022
 *
 * Implementación del objeto ObjectCharacterizator
 *
 */

#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <ctime>
#include <chrono>
#include <fstream>
#include <iomanip>

#include "object_characterization/ObjectCharacterizator.hh"
#include "models/Point.hh"
#include "models/Kernel.hh"

#include "logging/debug.hh"
#include "logging/logging.hh"

// Callback a donde se recebirán los puntos escaneados
void ObjectCharacterizator::newPoint(const Point &p) {
    // Comprobamos reflectividad
    if (p.getReflectivity() >= minReflectivity) {
        // Escogemos la acción adecuada respecto del estado
        switch (state) {
            // Establecemos primer timestamp
            case defStartTime:
                startTimestamp = new Timestamp(p.getTimestamp());
                state = defBackground;  // Comenzamos la definicion del background

                DEBUG_STDOUT("Start Timestamp: " << startTimestamp->string());

                static uint32_t bp_count = 0;
                static std::chrono::system_clock::time_point start, end;

                if (timer) {
                    start = std::chrono::high_resolution_clock::now();
                }

            // Punto de background
            case defBackground:

                // Punto del background
                if (*startTimestamp + backgroundTime > p.getTimestamp()) {
                    DEBUG_POINT_STDOUT("Punto añadido al background: " << p.string());
                    ++bp_count;

                    background->insert(p);  // Guardamos punto

                    break;  // Finalizamos
                }

                // Punto fuera del tiempo límite del background. Comenzamos con la caracterización del objeto
                else {
                    executionThread = new std::thread(&ObjectCharacterizator::managePoints, this);  // Creamos hilo

                    state = defObject;  // Empezamos a obtener puntos del objeto
                }

                background->buildOctree();

                LOG_INFO("Background formado por " << bp_count << " puntos.");

                if (timer) {
                    end = std::chrono::high_resolution_clock::now();
                    double seconds =
                        static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1.e9;

                    LOG_INFO("Duración de la caracterización del fondo: " << std::fixed << seconds << "s (" << std::setprecision(2) << bp_count / seconds
                                        << std::setprecision(6) << " puntos/s).");
                }

                DEBUG_STDOUT("Timestamp del punto límite: " << p.getTimestamp().string() << ".");

            // Punto del objeto
            case defObject:
                if (!isBackground(p)) {
                    DEBUG_POINT_STDOUT("Punto añadido al objeto: " << p.string());

                    object->push(p);  // Guardamos punto

                } else {
                    DEBUG_POINT_STDOUT("Punto enviado pertenece al background: " << p.string());
                }

                break;

            // Punto descartado
            case defStopped:
                DEBUG_POINT_STDOUT("Punto descartado: " << p.string());

                break;
        }
    } else {
        DEBUG_POINT_STDOUT("Punto con reflectividad insuficiente: " << p.string());
    }
}

// Comienza la definición de objetos
void ObjectCharacterizator::start() {
    LOG_INFO("Iniciando caracterización.");

    exit = false;  // Permitimos la ejecución del hilo

    state = defStartTime;  // Estado inicial de obtención de puntos
};

// Para la caracterización de objetos
void ObjectCharacterizator::stop() {
    DEBUG_STDOUT("Finalizando caracterización.");

    state = defStopped;  // Paramos de obtener puntos

    exit = true;              // Comunicamos al hilo que finalice la ejecución
    executionThread->join();  // Realizamos unión del hilo de gestión de puntos

    DEBUG_CODE({
        std::ofstream os("tmp/object.csv", std::ios::out);
        while (!object->empty()) {
            os << object->front().csv_string() << std::endl;
            object->pop();
        }
        os.close();
        // os.open("tmp/background.csv", std::ios::out);
        // for (auto &p : *background) {
        //     os << p.csv_string() << std::endl;
        // }
        // os.close();
    })

    LOG_INFO("Finalizada caracterización.");
}

// Define el fondo de la escena
void ObjectCharacterizator::defineBackground() {

}

// Define un objeto en la escena
void ObjectCharacterizator::defineObject() {}

#pragma GCC push_options
#pragma GCC optimize("O1")
// Guarda en background y elimina los puntos del objeto fuera del frame
void ObjectCharacterizator::managePoints() {
    while (!exit) {
        if (!object->empty()) {
            Point &p = object->front();

            // Eliminamos si su timestamp es viejo
            const std::pair<bool, Timestamp *> &lp = object->getLastTimestamp();
            if (lp.first && p.getTimestamp() + frameDuration < *lp.second) {
                DEBUG_POINT_STDOUT("Punto caducado: " << p.string());

                object->pop();  // Eliminamos punto
            }
        }
    }
}
#pragma GCC pop_options

// Comprueba si un punto pertenece al background
bool ObjectCharacterizator::isBackground(const Point &p) const {
    std::vector<Point *> neighbours = background->getMap()->searchNeighbors(p, backgroundDistance, Kernel_t::circle);

    for (Point *&pb : neighbours) {
        if (p.distance3D(*pb) < backgroundDistance) {
            return true;  // Pertenece al background
        }
    }
    return false;  // No pertenece al background
}