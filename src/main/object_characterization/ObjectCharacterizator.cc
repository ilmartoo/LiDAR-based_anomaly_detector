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
#include "debug_lbad.hh"

///////////////
#include <fstream>
#include "app/string_format.h"
///////////////

// Callback a donde se recebirán los puntos escaneados
void ObjectCharacterizator::newPoint(const Point &p) {
    ////////////////
    std::ofstream os("input_points.csv", std::ios::out);
    while (!object->empty()) {
        // std::cout << skyblue_s(object->front().string()) << std::endl;
        os << object->front().csv_string() << std::endl;
        object->pop();
    }
    os.close();
    ////////////////

    // Comprobamos reflectividad
    if (p.getReflectivity() >= minReflectivity) {
        // Escogemos la acción adecuada respecto del estado
        switch (state) {
            // Punto de background
            case defBackground:
                printDebug("Punto añadido al background: " + p.string());  // debug

                background->push_back(p);  // Guardamos punto

                break;

            // Punto del objeto
            case defObject:
                if (!isBackground(p)) {
                    printDebug("Punto añadido al objeto: " + p.string());  // debug
                    object->push(p);                                       // Guardamos punto

                } else {
                    printDebug("Punto enviado pertenece al background: " + p.string());  // debug
                }

                break;

            // Punto descartado
            case defStopped:
                printDebug("Punto descartado: " + p.string());  // debug

                break;
        }

    } else {
        printDebug("Punto con reflectividad insuficiente: " + p.string());  // debug
    }
}

// Comienza la definición de objetos
void ObjectCharacterizator::start(uint32_t backgroundTime, float minReflectivity) {
    std::cout << "Iniciando caracterización." << std::endl;

    this->minReflectivity = minReflectivity;  ///< Establecemos nueva reflectividad minima

    exit = false;  // Permitimos la ejecución del hilo
    executionThread = new std::thread(&ObjectCharacterizator::managePoints, this, backgroundTime);  // Creamos hilo
};

// Para la caracterización de objetos
void ObjectCharacterizator::stop() {
    printDebug("Finalizando caracterización.");  // debug

    state = defStopped;  // Paramos de obtener puntos

    exit = true;              // Comunicamos al hilo que finalice la ejecución
    executionThread->join();  // Realizamos unión del hilo de gestión de puntos

    ////////
    // std::ofstream os("output_points.csv", std::ios::out);
    // while(!object->empty()) {
    //     // std::cout << skyblue_s(object->front().string()) << std::endl;
    //     os << object->front().csv_string() << std::endl;
    //     object->pop();
    // }
    // os.close();
    //
    // for (auto &p : *background)
    //     std::cout << lightred_s(p.string()) << std::endl;
    ////////

    std::cout << "Finalizada caracterización." << std::endl;
}

// Guarda en background y elimina los puntos del objeto fuera del frame
void ObjectCharacterizator::managePoints(uint32_t backgroundTime) {
    state = defBackground;  // Empezamos a obtener puntos de background

    std::this_thread::sleep_for(std::chrono::milliseconds(backgroundTime));

    state = defObject;  // Empezamos a obtener puntos del objeto

    while (!exit) {
        if (!object->empty()) {
            Point &p = object->front();

            // Eliminamos si su timestamp es viejo
            if (p.getTimestamp() + frameDuration < object->getLastTimestamp()) {
                // printDebug("Punto caducado: " + p.string());  // debug

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