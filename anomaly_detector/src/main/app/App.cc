/**
 * @file App.cpp
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 20/03/2022
 *
 * Implementación de la clase de gestión de la aplicación
 *
 */

#include <iostream>
#include <stdint.h>
#include <functional>
#include <signal.h>

#include "app/App.hh"
#include "app/InputParser.hh"
#include "models/Point.hh"

#include "debug.hh"

void App::start() {
    scanner->init();                                                            // Inicializamos el escaner
    scanner->setCallback(([this](const Point &p) { this->oc->newPoint(p); }));  // Establecemos callback
    scanner->start();                                                           // Iniciamos el escaner

    oc->start();  // Iniciamos el caracterizador

    // ad->start(); // Iniciamos el detector de anomalías
}

void App::wait() {
    std::string input;
    do {
        std::cin >> input;  // Leemos comando

    } while (input.compare("q"));  // Esperamos a recibir el comando de finalización
}

void App::stop() {
    scanner->stop();  // Finalizamos el escaner

    oc->stop();  // Finalizamos el caracterizador
}