/**
 * @file App.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 20/03/2022
 *
 * Implementación de la clase de gestión de la aplicación
 *
 */

#include <iostream>
#include <string>
#include <vector>
#include <exception>

#include "app/App.hh"
#include "app/InputParser.hh"
#include "models/Point.hh"
#include "models/CLICommand.hh"

#include "logging/debug.hh"
#include "logging/logging.hh"

void App::init() {
    oc->init();
    // ad->init();
}

void App::close() {
    oc->stop();
    // ad->stop();
}

// Impresión cuando se detecta un comando desconocido
void inline unknownCommand() { CLI_STDERR("Unknown command: Execute " bold("help") " to get info about valid commands"); }
// Comprueba si un string está vacío (sin contar espacios)
bool isBlank(const std::string &str) {
    for (const char &c : str) {
        if (c != ' ') {
            return false;
        }
    }
    return true;
}

void App::cli() {
    std::string input;
    bool exit = false;

    do {
        do {
            std::cout << "$ ";
            std::getline(std::cin, input);
            DEBUG_STDOUT("Readed command: " << input);
        } while (isBlank(input));

        CLICommand command = CLICommand::parse(input);

        switch (command.getType()) {
            // HELP
            case kHelp: {
                //...

            } break;

            // EXIT
            case kExit: {
                exit = true;
            } break;

            // CHRONO
            case kChrono: {
                bool newChrono;
                if (command.numParams() == 2 && ((newChrono = command[0].compare("set") == 0) || command[0].compare("unset") == 0)) {
                    if (command[1].compare("char")) {
                        oc->setChrono(newChrono);

                    } else if (command[1].compare("analyze")) {
                        // ad->setChrono(newChrono);

                    } else if (command[1].compare("all")) {
                        oc->setChrono(newChrono);
                        // ad->setChrono(newChrono);

                    } else {
                        unknownCommand();
                    }

                } else {
                    unknownCommand();
                }
            } break;

            // DEFINE
            case kDefine: {
                if (command[0].compare("background") == 0) {
                    oc->defineBackground();

                } else if (command[0].compare("object") == 0) {
                    CharacterizedObject obj = oc->defineObject();

                    std::pair<bool, std::string> p;
                    if (command.numParams() == 2) {
                        p = om->newObject(command[1], obj);
                    } else {
                        p = om->newObject(obj);
                    }

                    if (p.first) {
                        CLI_STDOUT("Object " << command[1] << " created");
                    } else {
                        CLI_STDERR("Could not create object " << command[1]);
                    }

                } else if (command.numParams() == 3 && command[0].compare("set") == 0) {
                    if (command[1].compare("backframe") == 0) {
                        //...

                    } else if (command[1].compare("objframe") == 0) {
                        //...

                    } else {
                        unknownCommand();
                    }
                } else {
                    unknownCommand();
                }
            } break;

            // DISCARD
            case kDiscard: {
                if (command.numParams() == 1) {
                    try {
                        uint32_t ms = static_cast<uint32_t>(std::stoi(command[0]));
                        oc->wait(ms);

                    } catch (std::exception &e) {
                        CLI_STDERR("Time not valid");
                    }

                } else {
                    unknownCommand();
                }
            } break;

            // MODEL
            case kModel: {
                if (command.numParams() == 4 && command[0].compare("save") == 0) {
                    //...

                } else if (command.numParams() == 2) {
                    if (command[0].compare("set") == 0) {
                        //...

                    } else if (command[0].compare("load") == 0) {
                        std::pair<bool, std::string> p = om->loadModel(command[1]);
                        if (p.first) {
                            CLI_STDOUT("Model " << p.second << " loaded");
                        } else {
                            CLI_STDERR("Could not load model " << command[1]);
                        }

                    } else {
                        unknownCommand();
                    }
                } else {
                    unknownCommand();
                }
            } break;

            // EXPLAIN
            case kInfo: {
                //...
            } break;

            // LIST
            case kList: {
                if (command[0].compare("objects") == 0) {
                    if (om->getModels().size() > 0) {
                        CLI_STDOUT("Defined objects list:");
                        for (auto &o : om->getObjects()) {
                            CLI_STDOUT("  " << o.first);
                        }
                    } else {
                        CLI_STDOUT("No objects were created yet");
                    }

                } else if (command[0].compare("models") == 0) {
                    if (om->getModels().size() > 0) {
                        CLI_STDOUT("Models list:");
                        for (auto &m : om->getModels()) {
                            CLI_STDOUT("  " << m.first);
                        }
                    } else {
                        CLI_STDOUT("No models were loaded yet");
                    }

                } else {
                    unknownCommand();
                }
            } break;

            // ANALYZE
            case kAnalyze: {
                if (command.numParams() == 2) {
                    //...

                } else {
                    unknownCommand();
                }
            } break;

            // UNKNOWN COMMAND
            case kUnknown:
            default: {
                unknownCommand();
            } break;
        }
    } while (!exit);
}