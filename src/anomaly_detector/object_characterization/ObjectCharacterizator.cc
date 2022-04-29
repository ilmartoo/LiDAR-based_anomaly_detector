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
#include <cmath>

#include "object_characterization/ObjectCharacterizator.hh"

#include "logging/debug.hh"
#include "logging/logging.hh"

///////////////
#include <fstream>
///////////////

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

                DEBUG_STDOUT("Start Timestamp: " + startTimestamp->string());

            // Punto de background
            case defBackground:

                // Contador de puntos del background
                DEBUG_CODE(static uint32_t bp_count = 0);

                // Punto del background
                if (*startTimestamp + backgroundTime > p.getTimestamp()) {
                    DEBUG_POINT_STDOUT("Punto añadido al background: " + p.string());
                    DEBUG_CODE(++bp_count);

                    background->push_back(p);  // Guardamos punto

                    break;  // Finalizamos
                }

                // Punto fuera del tiempo límite del background. Comenzamos con la caracterización del objeto
                else {
                    executionThread = new std::thread(&ObjectCharacterizator::managePoints, this);  // Creamos hilo

                    state = defObject;  // Empezamos a obtener puntos del objeto
                }

                DEBUG_STDOUT("Background formado por " + std::to_string(bp_count) + " puntos");
                DEBUG_STDOUT("Timestamp del punto límite: " + p.getTimestamp().string());

            // Punto del objeto
            case defObject:
                if (!isBackground(p)) {
                    DEBUG_POINT_STDOUT("Punto añadido al objeto: " + p.string());

                    object->push(p);  // Guardamos punto

                } else {
                    DEBUG_POINT_STDOUT("Punto enviado pertenece al background: " + p.string());
                }

                break;

            // Punto descartado
            case defStopped:
                DEBUG_POINT_STDOUT("Punto descartado: " + p.string());

                break;
        }
    } else {
        DEBUG_POINT_STDOUT("Punto con reflectividad insuficiente: " + p.string());
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

    DEBUG_CODE(
        std::ofstream os("tmp/object.csv", std::ios::out);
        while (!object->empty()) {
            os << object->front().csv_string() << td::endl;
            object->pop();
        }
        os.close();
        os.open("tmp/background.csv", std::ios::out);
        for (auto &p : *background) os << p.csv_string() << std::endl;
        os.close();
    )

    LOG_INFO("Finalizada caracterización.");
}

// Guarda en background y elimina los puntos del objeto fuera del frame
void ObjectCharacterizator::managePoints() {
    while (!exit) {
        if (!object->empty()) {
            Point &p = object->front();

            // Eliminamos si su timestamp es viejo
            const std::pair<bool, Timestamp *> &lp = object->getLastTimestamp();
            if (lp.first && p.getTimestamp() + frameDuration < *lp.second) {
                DEBUG_POINT_STDOUT("Punto caducado: " + p.string());

                object->pop();  // Eliminamos punto
            }
        }
    }
}

// Comprueba si un punto pertenece al background
bool ObjectCharacterizator::isBackground(const Point &p) const {
    for (Point &pb : *background) {
        // Distancia euclidea
        double distance = pow(p.getX() - pb.getX(), 2) + pow(p.getY() - pb.getY(), 2) + pow(p.getZ() - pb.getZ(), 2);
        distance = sqrt(distance);

        if (distance < backgroundDistance) {
            return true;  // Pertenece al background
        }
    }
    return false;  // No pertenece al background
};