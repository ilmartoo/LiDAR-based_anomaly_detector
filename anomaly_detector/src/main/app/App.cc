/**
 * @file App.cpp
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 20/03/2022
 *
 * Implementación de la clase de gestión de la aplicación
 *
 */

#include <iostream>
#include <functional>
#include <string>

#include "app/App.hh"
#include "app/InputParser.hh"
#include "models/Point.hh"
#include "models/Command.hh"

#include "logging/debug.hh"
#include "logging/logging.hh"

void App::init() {
    scanner->init();                                                            // Inicializamos el escaner
    scanner->setCallback(([this](const Point &p) { this->oc->newPoint(p); }));  // Establecemos callbacks
}

void App::start() {
    // ad->start();       // Iniciamos el detector de anomalías
    oc->start();       // Iniciamos el caracterizador
    scanner->start();  // Iniciamos el escaner
}

void App::cli() {
    std::string input;
    bool exit = false;

    do {
        std::getline(std::cin, input);  // Leemos comando

        Command command = Command::parse(input);  // Parseamos comando

        switch (command.getType()) {
            // Mostrar ayuda
            case kHelp:
                DEBUG_STDOUT("Help command");
                break;

            // Finalización del programa
            case kExit:
                DEBUG_STDOUT("Exit command");
                exit = true;
                break;

            // Inicio secuencial
            case kStart:
                DEBUG_STDOUT("Start command");
                start();
                break;

            // Caracterización del background
            case kCharacterizatorBackground:
                DEBUG_STDOUT("Characterize Background command");
                // TODO
                break;

            // Caracterización del objeto
            case kCharacterizatorObject:
                DEBUG_STDOUT("Characterize Object command");
                // TODO
                break;

            // Detector de anomalías
            case kAnomalyDetector:
                DEBUG_STDOUT("Anomaly Detector command");
                // TODO
                break;

            // Comando desconocido
            default:
                DEBUG_STDOUT("Uknown command");
                // Ayuda o uso de comandos
                break;
        }

    } while (!exit);  // Esperamos a recibir el comando de finalización
}

void App::close() {
    scanner->stop();  // Finalizamos el escaner
    oc->stop();       // Finalizamos el caracterizador
    // ad->stop();       // Finalizamos el detector de anomalias
}