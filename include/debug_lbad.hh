/**
 * @file debug_lbad.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 23/03/2022
 *
 * Header que define funciones de debugging
 * Es necesario que la macro DEBUG_LBAD se defina en tiempo de compilación
 *
 */

#ifndef DEBUG_HEADER_H
#define DEBUG_HEADER_H

#include <iostream>
#include <string>

/**
 * Imprime mensajes de debugging si la macro DEBUG_LBAD está definida
 * @param debugstr Mensaje de debugging
 */
inline void printDebug(std::string debugstr) {
#ifdef DEBUG_LBAD
    std::cout << "[DEBUG] " << debugstr << std::endl;
#endif
}

#endif  //DEBUG_HEADER_H