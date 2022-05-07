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

#include "app/App.hh"
#include "app/InputParser.hh"
#include "models/Point.hh"
#include "models/CLICommand.hh"

#include "logging/debug.hh"
#include "logging/logging.hh"

void App::init() {
    oc->init();
    // ac->init();
}

void App::close() {
    oc->stop();
    // ad->stop();
}

// Impresión cuando se detecta un comando desconocido
void inline unknownCommand() { LOG_ERROR("Unknown command! Please execute " bold("help") " to get info about valid commands."); }
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

            // CHAR
            case kChar: {
                if (command[0].compare("background") == 0) {
                    oc->defineBackground();

                } else if (command[0].compare("object") == 0) {
                    //...

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

            // MODEL
            case kModel: {
                if (command.numParams() == 3 && command[0].compare("save") == 0) {
                    //...

                } else if (command.numParams() == 2 && command[0].compare("set") == 0) {
                    // ...

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
                    //...

                } else if (command[0].compare("models") == 0) {
                    //...

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