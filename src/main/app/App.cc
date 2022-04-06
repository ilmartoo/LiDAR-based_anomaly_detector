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

void App::start() {
    scanner->init();                                                            // Inicializamos el escaner
    scanner->setCallback(([this](const Point &p) { this->oc->newPoint(p); }));  // Establecemos callback
    scanner->start();                                                           // Iniciamos el escaner

    oc->start(backgroundTime, minReflectivity);  // Iniciamos el caracterizador

    // ad->start(); // Iniciamos el detector de anomalías
}

void App::wait() {
    int sig;             // Creamos buffer de señal
    sigset_t wset;       // Creamos set
    sigemptyset(&wset);  // Vaciamos set

    sigaddset(&wset, SIGINT);  // Añadimos señal de interrupción

    sigprocmask(SIG_BLOCK, &wset, nullptr);  // Bloqueamos señal de interrupción

    sigwait(&wset, &sig);  // Recibimos señal de interrupción

    std::cout << "\r\r";  // Output formatting
}

void App::stop() {
    scanner->stop();  // Finalizamos el escaner

    oc->stop();  // Finalizamos el caracterizador
}