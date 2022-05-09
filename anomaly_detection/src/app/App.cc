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
// Impresión de la ayuda
void printHelp(CLICommandType ct) {
    bool doBreak = !(ct == kUnknown);
    switch (ct) {
        // UNKNOWN COMMAND
        case kUnknown:
        default:

        // HELP
        case kHelp: {
            CLI_STDOUT(bold("help") "                           Prints this help text");
            if (doBreak) {
                break;
            }
        }

        // EXIT
        case kExit: {
            CLI_STDOUT(bold("exit") "                           Exits the program");
            if (doBreak) {
                break;
            }
        }

        // CHRONO
        case kChrono: {
            CLI_STDOUT(bold("chrono <set|unset> <...>") "       Activates/Deactivates a specific chronometer:");
            CLI_STDOUT("  - define                        Object and background definition routine");
            CLI_STDOUT("  - analyze                       Anomaly detection routine");
            CLI_STDOUT("  - all                           All of the above");
            if (doBreak) {
                break;
            }
        }

        // DEFINE
        case kDefine: {
            if (doBreak) {
                break;
            }
        }

        // DISCARD
        case kDiscard: {
            CLI_STDOUT(bold("discard <mseconds>") "             Discards points for the amount of miliseconds specified");
            if (doBreak) {
                break;
            }
        }

        // MODEL
        case kModel: {
            if (doBreak) {
                break;
            }
        }

        // INFO
        case kInfo: {
            if (doBreak) {
                break;
            }
        }

        // LIST
        case kList: {
            CLI_STDOUT(bold("list <...>") "                     List the items of:");
            CLI_STDOUT("  - objects                       Created objects");
            CLI_STDOUT("  - models                        Loaded models");
            if (doBreak) {
                break;
            }
        }

        // ANALYZE
        case kAnalyze: {
            CLI_STDOUT(bold("analyze <object> <model>") "       Analizes the diferences between the specified object and model");
            if (doBreak) {
                break;
            }
        }
    }
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
                printHelp(CLICommand::parse(command[0]).getType());
            } break;

            // EXIT
            case kExit: {
                exit = true;
            } break;

            // CHRONO
            case kChrono: {
                bool newChrono;
                if (command.numParams() == 2 && ((newChrono = command[0].compare("set") == 0) || command[0].compare("unset") == 0)) {
                    if (command[1].compare("define")) {
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
                        CLI_STDOUT("Object " << p.second << " created");
                    } else {
                        CLI_STDERR("Could not create object");
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
                    if (command[3].size() == 1) {
                        switch (command[3][0]) {
                            case 'f': {
                                if (om->newModel(command[1], command[2], mFrontal).first) {
                                    CLI_STDOUT("Saved " << command[1] << " as front face of model " << command[2]);
                                } else {
                                    CLI_STDERR("Could not save " << command[1] << " as front face of model " << command[2]);
                                }

                            } break;

                            case 'b': {
                                om->newModel(command[1], command[2], mBack);
                            } break;

                            case 'r': {
                                om->newModel(command[1], command[2], mRSide);
                            } break;

                            case 'l': {
                                om->newModel(command[1], command[2], mLSide);
                            } break;

                            case 'c': {
                                om->newModel(command[1], command[2], mCeil);
                            } break;

                            case 't': {
                                om->newModel(command[1], command[2], mBottom);
                            } break;
                            default: {
                                CLI_STDERR("Face not valid");
                            }
                        }

                    } else {
                        unknownCommand();
                    }

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

            // INFO
            case kInfo: {
                //...
            } break;

            // LIST
            case kList: {
                if (command[0].compare("objects") == 0) {
                    if (om->getObjects().size() > 0) {
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