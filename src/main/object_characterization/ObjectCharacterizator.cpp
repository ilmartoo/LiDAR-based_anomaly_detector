/**
 * @file ObjectCharacterizator.cpp
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 2022-03-25
 *
 * Implementación del objeto ObjectCharacterizator
 *
 */

#include <string>

#include "object_characterization/ObjectCharacterizator.hpp"
#include "debug_lbad.hpp"

// Callback a donde se recebirán los puntos escaneados
void ObjectCharacterizator::newPoint(Point p) {
    // Escogemos la acción adecuada respecto del estado
    switch (this->state) {
        // Punto de background
        case defBackground:

            break;

        // Punto del objeto
        case defObject:

            break;

        // Punto descartado
        case defStopped:
            printDebug("Punto descartado" + std::to_string(p));  // debug
            break;
    }
}