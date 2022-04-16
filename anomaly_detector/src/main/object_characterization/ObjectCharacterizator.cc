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

#include "debug.hh"

///////////////
#include <fstream>
#include "app/string_format.h"
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

            // Punto de background
            case defBackground:

                // Punto del background
                if (*startTimestamp + backgroundTime > p.getTimestamp()) {
                    DEBUG_POINT_STDOUT("Punto añadido al background: " + p.string());

                    background->push_back(p);  // Guardamos punto

                    // Contador de puntos del background
                    DEBUG_CODE(static uint32_t count = 0; ++count;);

                    break;  // Finalizamos
                }

                // Punto fuera del tiempo límite del background. Comenzamos con la caracterización del objeto
                else {
                    executionThread = new std::thread(&ObjectCharacterizator::managePoints, this);  // Creamos hilo

                    state = defObject;  // Empezamos a obtener puntos del objeto
                }

                // Total de puntos del background
                DEBUG_STDOUT("Background formado por " + to_string(count) + " puntos");

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
    std::cout << "Iniciando caracterización." << std::endl;

    exit = false;  // Permitimos la ejecución del hilo

    state = defStartTime;  // Estado inicial de obtención de puntos
};

// Para la caracterización de objetos
void ObjectCharacterizator::stop() {
    DEBUG_STDOUT("Finalizando caracterización.");

    state = defStopped;  // Paramos de obtener puntos

    exit = true;              // Comunicamos al hilo que finalice la ejecución
    executionThread->join();  // Realizamos unión del hilo de gestión de puntos

    ////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////
    std::ofstream os("object.csv", std::ios::out);
    while (!object->empty()) {
        // std::cout << skyblue_s(object->front().string()) << std::endl;
        os << object->front().csv_string() << std::endl;
        object->pop();
    }
    os.close();

    os.open("background.csv", std::ios::out);
    std::cout << "BG size END: " << background->size() << std::endl;
    for (auto &p : *background)
        os << p.csv_string() << std::endl;
    os.close();
    ////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////

    std::cout << "Finalizada caracterización." << std::endl;
}

// Guarda en background y elimina los puntos del objeto fuera del frame
void ObjectCharacterizator::managePoints() {
    while (!exit) {
        if (!object->empty()) {
            Point &p = object->front();

            // Eliminamos si su timestamp es viejo
            if (p.getTimestamp() + frameDuration < object->getLastTimestamp()) {
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