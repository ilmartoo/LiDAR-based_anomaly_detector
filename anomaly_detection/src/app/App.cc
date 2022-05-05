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
#include <sstream>
#include <vector>

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

int cli() {
    std::vector<std::string> command;
    bool exit = false;

    do {
        command = getCommand();

        ////
        std::cout << command.size() << " >> ";
        for (auto& e : command)
            std::cout << e << ' ';
        std::cout << '\n';
        ////

        if (command.size() >= 1) {
            // HELP
            if (!command[0].compare("help")) {
                //...
            }

            // EXIT
            else if (!command[0].compare("exit")) {
                exit = true;
            }

            // CHAR
            else if (!command[0].compare("char")) {
                if (command.size() >= 2) {
                    if (!command[1].compare("background")) {
                        //...
                    } else if (!command[1].compare("object")) {
                        //...
                    } else if (!command[1].compare("set")) {
                        if (command.size() == 4) {
                            if (!command[2].compare("bframe")) {
                                //...
                            } else if (!command[2].compare("oframe")) {
                                //...
                            } else {
                                badCommand();
                            }
                        } else {
                            badCommand();
                        }
                    } else {
                        badCommand();
                    }
                } else {
                    badCommand();
                }
            }

            // MODEL
            else if (!command[0].compare("model")) {
                if (command.size() >= 2) {
                    if (!command[1].compare("save")) {
                        if (command.size() == 4) {
                            //...
                        } else {
                            badCommand();
                        }
                    } else if (!command[1].compare("set")) {
                        if (command.size() == 3) {
                            //...
                        } else {
                        }
                    } else {
                        badCommand();
                    }
                }
            }

            // EXPLAIN
            else if (!command[0].compare("explain")) {
                //...
            }

            // LIST
            else if (!command[0].compare("list")) {
                if (command.size() == 2) {
                    if (!command[1].compare("objects")) {
                        //...
                    } else if (!command[1].compare("models")) {
                        //...
                    } else {
                        badCommand();
                    }
                }
            }

            // ANALYZE
            else if (!command[0].compare("analyze")) {
                if (command.size() == 3) {
                    //...
                } else {
                    badCommand();
                }
            }

            // BAD COMMAND
            else {
                badCommand();
            }
        }
    } while (!exit);
}

void App::cli() {
    std::string input;
    bool exit = false;

    do {
        std::getline(std::cin, input);  // Leemos comando

        Command command = Command::parse(input);  // Parseamos comando

        switch (command.getType()) {
            // HELP
            case kHelp:
                DEBUG_STDOUT("Help command");
                break;

            // EXIT
            case kExit:
                DEBUG_STDOUT("Exit command");
                exit = true;
                break;

            // CHAR
            case kChar:
                DEBUG_STDOUT("Char command");
                start();
                break;

            // MODEL
            case kModel:
                DEBUG_STDOUT("Model command");
                // TODO
                break;

            // EXPLAIN
            case kExplain:
                DEBUG_STDOUT("Explain command");
                // TODO
                break;

            // LIST
            case kList:
                DEBUG_STDOUT("List command");
                // TODO
                break;

            // ANALYZE
            case kAnalyze:
                DEBUG_STDOUT("Analyze command");
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

void App::wait() {
    std::string input;
    do {
        std::cin >> input;  // Esperamos señal de salida

    } while (input.compare("q") != 0);
}

void App::close() {
    scanner->stop();  // Finalizamos el escaner
    oc->stop();       // Finalizamos el caracterizador
    // ad->stop();       // Finalizamos el detector de anomalias
}