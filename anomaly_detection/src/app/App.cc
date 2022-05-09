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

void App::init() {
    oc->init();
    // ad->init();
}

void App::close() {
    oc->stop();
    // ad->stop();
}

// Macro de comparación de string con c_string
#define STR_STRC_CMP(string, c_string) (string.compare(c_string) == 0)
// Impresión cuando se detecta un comando desconocido
void inline unknownCommand(const char *command) { CLI_STDERR("Unknown command: Execute " bold("help [") << command << bold("]") " to get info about valid commands"); }
void inline unknownCommand() { unknownCommand("command"); }
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
            CLI_STDOUT(bold("help [command]") "                 Prints the help text of a command or all of them if not specified");
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
            CLI_STDOUT(bold("chrono <set|unset> <...>") "       Activation/Deactivation of specific chronometers:");
            CLI_STDOUT("  - define                        Object and background definition routine");
            CLI_STDOUT("  - analyze                       Anomaly detection routine");
            CLI_STDOUT("  - all                           All of the above");
            if (doBreak) {
                break;
            }
        }

        // DEFINE
        case kDefine: {
            CLI_STDOUT(bold("define <...>") "                   Definition and characterization of objects and background:");
            CLI_STDOUT("  - background                    Defines the background");
            CLI_STDOUT("  - object [name]                 Defines an object with a specified name or an automatic generated one");
            if (doBreak) {
                break;
            }
        }

        // SET
        case kSet: {
            CLI_STDOUT(bold("set <...>") "                      Modification of current execution parameters:");
            CLI_STDOUT("  - backframe                     Miliseconds (integer) to scan for background points");
            CLI_STDOUT("  - objframe                      Miliseconds (integer) to scan for object points");
            CLI_STDOUT("  - backthreshold                 Meters (decimal) away an object point must be from the background to not be discarded");
            CLI_STDOUT("  - reflthreshold                 Minimun reflectivity (decimal) a point must have to not be discarded");
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
            CLI_STDOUT(bold("model <...>") "                    Management of models:");
            CLI_STDOUT("  - save <obj> <model> <...>      Saves an object as a specified face of a existing or new model:");
            CLI_STDOUT("     - f                             Front face");
            CLI_STDOUT("     - b                             Back face");
            CLI_STDOUT("     - l                             Left face");
            CLI_STDOUT("     - r                             Right face");
            CLI_STDOUT("     - c                             Ceil face");
            CLI_STDOUT("     - t                             Bottom face");
            CLI_STDOUT("  - load <file>                   Loads a model from a file");
            CLI_STDOUT("  - write <model> <file>          Writes a model into a file");
            if (doBreak) {
                break;
            }
        }

        // INFO
        case kInfo: {
            CLI_STDOUT(bold("info") "                           Prints the execution parameters currently in use");
            if (doBreak) {
                break;
            }
        }

        // LIST
        case kList: {
            CLI_STDOUT(bold("list <...>") "                     List loaded/stored items:");
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

    /* cout floating point formatting */
    std::cout << std::fixed;
    std::cout.precision(2);
    /* end cout formatting */

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
                if (command.numParams() == 2 && ((newChrono = STR_STRC_CMP(command[0], "set")) || STR_STRC_CMP(command[0], "unset"))) {
                    if (STR_STRC_CMP(command[1], "define")) {
                        oc->setChrono(newChrono);

                    } else if (STR_STRC_CMP(command[1], "analyze")) {
                        ad->setChrono(newChrono);

                    } else if (STR_STRC_CMP(command[1], "all")) {
                        oc->setChrono(newChrono);
                        ad->setChrono(newChrono);

                    } else {
                        unknownCommand("chrono");
                    }

                } else {
                    unknownCommand("chrono");
                }
            } break;

            // DEFINE
            case kDefine: {
                if (STR_STRC_CMP(command[0], "background")) {
                    oc->defineBackground();

                } else if (STR_STRC_CMP(command[0], "object")) {
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

                } else {
                    unknownCommand("define");
                }
            } break;

            // SET
            case kSet: {
                if (command.numParams() == 2) {
                    try {
                        if (STR_STRC_CMP(command[2], "backframe")) {
                            oc->setBackFrame(static_cast<uint32_t>(std::stoi(command[1])));
                            CLI_STDOUT("New background frame set at " << command[1] << "ms");

                        } else if (STR_STRC_CMP(command[2], "objframe")) {
                            oc->setObjFrame(static_cast<uint32_t>(std::stoi(command[1])));
                            CLI_STDOUT("New object frame set at " << command[1] << "ms");

                        } else if (STR_STRC_CMP(command[2], "backthreshold")) {
                            oc->setBackDistance(std::stof(command[1]));
                            CLI_STDOUT("New background distance threshold set at " << command[1] << "m");

                        } else if (STR_STRC_CMP(command[2], "reflthreshold")) {
                            oc->setMinReflectivity(std::stof(command[1]));
                            CLI_STDOUT("New minimun reflectivity set at " << command[1]);

                        } else {
                            unknownCommand("set");
                        }
                    } catch (std::exception &e) {
                        CLI_STDERR("Invalid number");
                    }
                }
            } break;

            // DISCARD
            case kDiscard: {
                if (command.numParams() == 1) {
                    try {
                        uint32_t ms = static_cast<uint32_t>(std::stoi(command[0]));
                        oc->wait(ms);

                    } catch (std::exception &e) {
                        CLI_STDERR("Invalid time");
                    }

                } else {
                    unknownCommand("discard");
                }
            } break;

            // MODEL
            case kModel: {
                if (command.numParams() == 4 && STR_STRC_CMP(command[0], "save")) {
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
                                if (om->newModel(command[1], command[2], mBack).first) {
                                    CLI_STDOUT("Saved " << command[1] << " as back face of model " << command[2]);
                                } else {
                                    CLI_STDERR("Could not save " << command[1] << " as back face of model " << command[2]);
                                }
                            } break;

                            case 'r': {
                                if (om->newModel(command[1], command[2], mRSide).first) {
                                    CLI_STDOUT("Saved " << command[1] << " as right face of model " << command[2]);
                                } else {
                                    CLI_STDERR("Could not save " << command[1] << " as right face of model " << command[2]);
                                }
                            } break;

                            case 'l': {
                                if (om->newModel(command[1], command[2], mLSide).first) {
                                    CLI_STDOUT("Saved " << command[1] << " as left face of model " << command[2]);
                                } else {
                                    CLI_STDERR("Could not save " << command[1] << " as left face of model " << command[2]);
                                }
                            } break;

                            case 'c': {
                                if (om->newModel(command[1], command[2], mCeil).first) {
                                    CLI_STDOUT("Saved " << command[1] << " as ceil face of model " << command[2]);
                                } else {
                                    CLI_STDERR("Could not save " << command[1] << " as ceil face of model " << command[2]);
                                }
                            } break;

                            case 't': {
                                if (om->newModel(command[1], command[2], mBottom).first) {
                                    CLI_STDOUT("Saved " << command[1] << " as bottom face of model " << command[2]);
                                } else {
                                    CLI_STDERR("Could not save " << command[1] << " as bottom face of model " << command[2]);
                                }
                            } break;

                            default: {
                                CLI_STDERR("Inexistent face");
                            }
                        }
                    } else {
                        unknownCommand("model");
                    }

                } else if (command.numParams() == 3 && STR_STRC_CMP(command[0], "write")) {
                    if (om->writeModel(command[2], command[1])) {
                        CLI_STDOUT("Model " << command[1] << " written into file " << command[2]);
                    } else {
                        CLI_STDERR("Could not write model " << command[1] << " into file " << command[2]);
                    }

                } else if (command.numParams() == 2 && STR_STRC_CMP(command[0], "load")) {
                    std::pair<bool, std::string> p = om->loadModel(command[1]);
                    if (p.first) {
                        CLI_STDOUT("Model " << p.second << " loaded");
                    } else {
                        CLI_STDERR("Could not load model " << command[1]);
                    }

                } else {
                    unknownCommand("model");
                }
            } break;

            // INFO
            case kInfo: {
                CLI_STDOUT("Object frame:            " << oc->getObjFrame() / 1000000 << " ms");
                CLI_STDOUT("Background frame:        " << oc->getBackFrame() / 1000000 << " ms");
                CLI_STDOUT("Background threshold:    " << oc->getBackDistance() << " m");
                CLI_STDOUT("Reflectivity threshold:  " << oc->getMinReflectivity() << " points");
                CLI_STDOUT("define chronometer:      " << (oc->isChrono() ? "Activated" : "Deactivated"));
                CLI_STDOUT("analyze chronometer:     " << (ad->isChrono() ? "Activated" : "Deactivated"));

            } break;

            // LIST
            case kList: {
                if (STR_STRC_CMP(command[0], "objects")) {
                    if (om->getObjects().size() > 0) {
                        CLI_STDOUT("Defined objects list:");
                        for (auto &o : om->getObjects()) {
                            CLI_STDOUT("  " << o.first);
                        }
                    } else {
                        CLI_STDOUT("No objects were created yet");
                    }

                } else if (STR_STRC_CMP(command[0], "models")) {
                    if (om->getModels().size() > 0) {
                        CLI_STDOUT("Models list:");
                        for (auto &m : om->getModels()) {
                            CLI_STDOUT("  " << m.first);
                        }
                    } else {
                        CLI_STDOUT("No models were loaded yet");
                    }

                } else {
                    unknownCommand("list");
                }
            } break;

            // ANALYZE
            case kAnalyze: {
                if (command.numParams() == 2) {
                    if (om->existsObject(command[0])) {
                        if (om->existsModel(command[1])) {
                            const CharacterizedObject &object = om->getObjects().at(command[0]);
                            const Model &model = om->getModels().at(command[1]);

                            /* DO THINGS WITH RETURN */ ad->compare(object, model);

                        } else {
                            CLI_STDERR("Could not locate model " << command[1]);
                        }
                    } else {
                        CLI_STDERR("Could not locate object " << command[0]);
                    }
                } else {
                    unknownCommand("analyze");
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