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

#include "object_characterization/ObjectCharacterizator.hh"
#include "debug_lbad.hh"

// Callback a donde se recebirán los puntos escaneados
void ObjectCharacterizator::newPoint(Point &p) {
    // Escogemos la acción adecuada respecto del estado
    switch (state) {
        // Punto de background
        case defBackground:
            printDebug("Punto añadido al background:\n" + p.string());  // debug

            background->push_back(p);  // Guardamos punto

            break;

        // Punto del objeto
        case defObject:
            printDebug("Punto añadido al objeto:\n" + p.string());  // debug

            object->push(p);  // Guardamos punto

            break;

        // Punto descartado
        case defStopped:
            printDebug("Punto descartado:\n" + p.string());  // debug

            break;
    }
}

// Comienza la definición de objetos, definiendo el background durante los milisegundos
// especificados para despues definir los objetos
void ObjectCharacterizator::start(uint16_t backgroundTime) {
    printDebug("Iniciando caracterización.");  // debug

    state = defBackground;  // Empezamos a obtener puntos de background

    std::this_thread::sleep_for(std::chrono::milliseconds(backgroundTime));

    state = defObject;  // Empezamos a obtener puntos del objeto

    timestampChecker = std::thread(&ObjectCharacterizator::removeOldObjectPoints, this);
};

// Para la caracterización de objetos
void ObjectCharacterizator::stop() {
    printDebug("Finalizando caracterización.");  // debug

    state = defStopped;

    timestampChecker.~thread();  // Parar el hilo de comprobación de timestamps

    printDebug("Finalizada caracterización.");  // debug
}

// Elimina los puntos del objeto cullo tiempo de vida sea mayor que el máximo establecido
void ObjectCharacterizator::removeOldObjectPoints() {
    while (true) {
        if (!object->empty()) {
            Point &p = object->front();

            if (p.getTimestamp() + frameDuration < *object->getLastTimestamp()) {
                printDebug("Punto eliminado:\n" + p.string());  // debug

                object->pop();  // Eliminamos punto
            }
        }
    }
}