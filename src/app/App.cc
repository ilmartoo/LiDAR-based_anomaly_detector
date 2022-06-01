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
#include <utility>

#include "app/App.hh"
#include "app/InputParser.hh"
#include "models/Point.hh"
#include "app/CLICommand.hh"

#include "logging/debug.hh"

void App::init() {
    oc->init();
}

void App::close() {
    oc->stop();
}

// Impresión cuando se detecta un comando desconocido
void inline unknownCommand(const char *command) { CLI_STDERR("Unknown command: Execute <help " << command << "> to get info about valid commands"); }
void inline unknownCommand() { unknownCommand("[command]"); }
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
            CLI_STDOUT_NO_NL(bold("help [command]"));
            CLI_STDOUT("                 Prints the help text of a command or all of them if not specified");
            if (doBreak) {
                break;
            }
        }

        // EXIT
        case kExit: {
            CLI_STDOUT_NO_NL(bold("exit"));
            CLI_STDOUT("                           Exits the program");
            if (doBreak) {
                break;
            }
        }

        // CHRONO
        case kChrono: {
            CLI_STDOUT_NO_NL(bold("chrono <set|unset> <...>"));
            CLI_STDOUT("       Activation/Deactivation of specific chronometers:");
            CLI_STDOUT("  - define                        Object and background definition routine");
            CLI_STDOUT("  - analyze                       Anomaly detection routine");
            CLI_STDOUT("  - all                           All of the above");
            if (doBreak) {
                break;
            }
        }

        // DEFINE
        case kDefine: {
            CLI_STDOUT_NO_NL(bold("define <...>"));
            CLI_STDOUT("                   Definition and characterization of objects and background:");
            CLI_STDOUT("  - background                    Defines the background");
            CLI_STDOUT("  - object [name]                 Defines an object with a specified name or an automatic generated one");
            if (doBreak) {
                break;
            }
        }

        // SET
        case kSet: {
            CLI_STDOUT_NO_NL(bold("set <...>"));
            CLI_STDOUT("                      Modification of current execution parameters:");
            CLI_STDOUT("  - backframe <millisecs>         Milliseconds (integer) to scan for background points");
            CLI_STDOUT("  - objframe <millisecs>          Milliseconds (integer) to scan for object points");
            CLI_STDOUT("  - backthreshold <meters>        Meters (decimal) away an object point must be from the background to not be discarded");
            CLI_STDOUT("  - reflthreshold <points>        Minimun reflectivity (decimal) a point must have to not be discarded");
            if (doBreak) {
                break;
            }
        }

        // DISCARD
        case kDiscard: {
            CLI_STDOUT_NO_NL(bold("discard <millisecs>"));
            CLI_STDOUT("           Discards points for the amount of miliseconds specified");
            if (doBreak) {
                break;
            }
        }

        // OBJECT
        case kObject: {
            CLI_STDOUT_NO_NL(bold("object <...>"));
            CLI_STDOUT("                    Management of objects:");
            CLI_STDOUT("  - describe <name>               Describes the object with the given name");
            CLI_STDOUT("  - load <name> <file>            Loads the contents of a file as a new object with the given name");
            CLI_STDOUT("  - save <name> <file>            Saves the object with the given name into a file");
            CLI_STDOUT("  - csv <name> <file>             Saves the object with the given name into a file in csv format");
            if (doBreak) {
                break;
            }
        }

        // MODEL
        case kModel: {
            CLI_STDOUT_NO_NL(bold("model <...>"));
            CLI_STDOUT("                    Management of models:");
            CLI_STDOUT("  - new <object> <new_model>      Creates a new model from an object with the given name");
            CLI_STDOUT("  - describe <name>               Describes the model with the given name");
            CLI_STDOUT("  - load <name> <file>            Loads the contents of a file as a new model with the given name");
            CLI_STDOUT("  - save <name> <file>            Saves the model with the given name into a file");
            CLI_STDOUT("  - csv <name> <file>             Saves the model with the given name into a file in csv format");
            if (doBreak) {
                break;
            }
        }

        // INFO
        case kInfo: {
            CLI_STDOUT_NO_NL(bold("info"));
            CLI_STDOUT("                           Prints the execution parameters currently in use");
            if (doBreak) {
                break;
            }
        }

        // LIST
        case kList: {
            CLI_STDOUT_NO_NL(bold("list <...>"));
            CLI_STDOUT("                     List loaded/stored items:");
            CLI_STDOUT("  - objects                       Created objects");
            CLI_STDOUT("  - models                        Loaded models");
            if (doBreak) {
                break;
            }
        }

        // ANALYZE
        case kAnalyze: {
            CLI_STDOUT_NO_NL(bold("analyze <object> <model>"));
            CLI_STDOUT("       Analizes the diferences between the specified object and model");
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
                if (command.numParams() == 2 && ((newChrono = (command[0] == "set")) || command[0] == "unset")) {
                    if (command[1] == "define") {
                        oc->setChrono(newChrono);
                        CLI_STDOUT((newChrono ? "S" : "Uns") << "et define chronometer");

                    } else if (command[1] == "analyze") {
                        ad->setChrono(newChrono);
                        CLI_STDOUT((newChrono ? "S" : "Uns") << "et analyze chronometer");

                    } else if (command[1] == "all") {
                        oc->setChrono(newChrono);
                        ad->setChrono(newChrono);
                        CLI_STDOUT((newChrono ? "S" : "Uns") << "et all chronometers");

                    } else {
                        unknownCommand("chrono");
                    }

                } else {
                    unknownCommand("chrono");
                }
            } break;

            // DEFINE
            case kDefine: {
                if (command[0] == "background") {
                    oc->defineBackground();

                } else if (command[0] == "object") {
                    std::pair<bool, CharacterizedObject> obj = oc->defineObject();

                    if (obj.first) {
                        std::pair<bool, std::string> p;
                        if (command.numParams() == 2) {
                            p = {om->newObject(command[1], obj.second), command[1]};
                        } else {
                            p = om->newObject(obj.second);
                        }

                        if (p.first) {
                            CLI_STDOUT("Object " << p.second << " created");
                        } else {
                            CLI_STDERR("Could not create object");
                        }
                    } else {
                        CLI_STDERR("Scanned object points are too sparse to correctly define an object");
                    }

                } else {
                    unknownCommand("define");
                }
            } break;

            // SET
            case kSet: {
                if (command.numParams() == 2) {
                    try {
                        if (command[0] == "backframe") {
                            oc->setBackFrame(static_cast<uint32_t>(std::stoi(command[1])));
                            CLI_STDOUT("New background frame set at " << command[1] << " ms");

                        } else if (command[0] == "objframe") {
                            oc->setObjFrame(static_cast<uint32_t>(std::stoi(command[1])));
                            CLI_STDOUT("New object frame set at " << command[1] << " ms");

                        } else if (command[0] == "backthreshold") {
                            oc->setBackDistance(std::stof(command[1]));
                            CLI_STDOUT("New background distance threshold set at " << command[1] << " m");

                        } else if (command[0] == "reflthreshold") {
                            oc->setMinReflectivity(std::stof(command[1]));
                            CLI_STDOUT("New minimun reflectivity set at " << command[1] << " points");

                        } else {
                            unknownCommand("set");
                        }
                    } catch (std::exception &e) {
                        CLI_STDERR("Invalid number");
                    }
                } else {
                    unknownCommand("set");
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

            // OBJECT
            case kObject: {
                if (command.numParams() >= 3) {
                    if (command[0] == "save") {
                        if (om->writeObject(command[2], command[1])) {
                            CLI_STDOUT("Object " << command[1] << " written into file " << command[2]);
                        } else {
                            CLI_STDERR("Could not save object " << command[1] << " into file " << command[2]);
                        }
                    } else if (command[0] == "load") {
                        if (om->loadObject(command[2], command[1])) {
                            CLI_STDOUT("Object " << command[1] << " loaded from file " << command[2]);
                        } else {
                            CLI_STDERR("Could not load object " << command[1] << " from file " << command[2]);
                        }

                    } else if (command[0] == "csv") {
                        if (om->writeObjectCSV(command[2], command[1])) {
                            CLI_STDOUT("Object " << command[1] << " written into csv file " << command[2]);
                        } else {
                            CLI_STDERR("Could not save object " << command[1] << " into csv file " << command[2]);
                        }
                    } else {
                        unknownCommand("object");
                    }
                } else if (command.numParams() == 2 && command[0] == "describe") {
                    if (om->existsObject(command[1])) {
                        const CharacterizedObject &co = om->getObjects().at(command[1]);

                        size_t npoints = 0;
                        for (auto &f : co.getFaces()) {
                            npoints += f.getPoints().size();
                        }

                        CLI_STDOUT("Object " << command[1] << " characteristics:");
                        CLI_STDOUT("  Total faces:      " << co.getFaces().size());
                        CLI_STDOUT("  Total points:     " << npoints);
                        CLI_STDOUT("  Width  / z_delta: " << co.getBBox().getDeltaZ());
                        CLI_STDOUT("  Height / y_delta: " << co.getBBox().getDeltaY());
                        CLI_STDOUT("  Depth  / x_delta: " << co.getBBox().getDeltaX());
                        CLI_STDOUT("  Normal vectors:");
                        for (auto &f : co.getFaces()) {
                            CLI_STDOUT("    [" << f.getNormal() << "]");
                        }
                    } else {
                        CLI_STDERR("Could not locate object " << command[1]);
                    }
                } else {
                    unknownCommand("object");
                }
            } break;

            // MODEL
            case kModel: {
                if (command.numParams() == 3) {
                    if (command[0] == "new") {
                        if (om->newModel(command[1], command[2])) {
                            CLI_STDOUT("Saved " << command[1] << " as model " << command[2]);
                        } else {
                            CLI_STDERR("Could not save " << command[1] << " as model " << command[2]);
                        }
                    } else if (command[0] == "save") {
                        if (om->writeModel(command[2], command[1])) {
                            CLI_STDOUT("Model " << command[1] << " written into file " << command[2]);
                        } else {
                            CLI_STDERR("Could not save model " << command[1] << " into file " << command[2]);
                        }
                    } else if (command[0] == "load") {
                        if (om->loadModel(command[2], command[1])) {
                            CLI_STDOUT("Model " << command[1] << " loaded from file " << command[2]);
                        } else {
                            CLI_STDERR("Could not load model " << command[1] << " from file " << command[2]);
                        }
                    } else {
                        unknownCommand("model");
                    }
                } else if (command.numParams() == 2 && command[0] == "describe") {
                    if (om->existsModel(command[1])) {
                        const Model &m = om->getModels().at(command[1]);

                        size_t npoints = 0;
                        for (auto &f : m.getFaces()) {
                            npoints += f.getPoints().size();
                        }

                        CLI_STDOUT("Model " << command[1] << " characteristics:");
                        CLI_STDOUT("  Total faces:      " << m.getFaces().size());
                        CLI_STDOUT("  Total points:     " << npoints);
                        CLI_STDOUT("  Width  / z_delta: " << m.getBBox().getDeltaZ());
                        CLI_STDOUT("  Height / y_delta: " << m.getBBox().getDeltaY());
                        CLI_STDOUT("  Depth  / x_delta: " << m.getBBox().getDeltaX());
                        CLI_STDOUT("  Normal vectors:");
                        for (auto &f : m.getFaces()) {
                            CLI_STDOUT("    [" << f.getNormal() << "]");
                        }
                    } else {
                        CLI_STDERR("Could not locate model " << command[1]);
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
                if (command[0] == "objects") {
                    if (om->getObjects().size() > 0) {
                        CLI_STDOUT("Defined objects list:");
                        for (auto &o : om->getObjects()) {
                            CLI_STDOUT("  " << o.first);
                        }
                    } else {
                        CLI_STDOUT("No objects were created yet");
                    }

                } else if (command[0] == "models") {
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

                            AnomalyReport ar = ad->compare(object, model);

                            Vector deltas;  // Aux variable

                            // Notes
                            CLI_STDOUT("------------------------ ANOMALY REPORT ------------------------\n"
                                       " // NOTES //\n"
                                       " Bounding box substractions represent the difference between\n"
                                       " a model bounding box and an object bouding box in millimeters\n"
                                       " with the dimensions in the following format:\n" bold("   [depth, height, width]"));
                            CLI_STDOUT(" Face comparisons are made between the two faces specified in\n"
                                       " the following format:\n" bold("   [model_face, object_face]"));
                            // General BBox comparison
                            CLI_STDOUT("\n // GENERAL COMPARISON //");
                            CLI_STDOUT(" NumFaces(model)  = " << model.getFaces().size());
                            CLI_STDOUT(" NumFaces(object) = " << object.getFaces().size());
                            CLI_STDOUT(" NumFaces(model) - NumFaces(object) = " << ar.deltaFaces);
                            if (ar.deltaFaces < 0) {
                                CLI_STDOUT(bold(" Model has " << ar.deltaFaces * -1 << " more faces"));
                            } else if (ar.deltaFaces > 0) {
                                CLI_STDOUT(bold(" Object has " << ar.deltaFaces << " more faces"));
                            } else {
                                CLI_STDOUT(bold(" Model and object have the same number of faces"));
                            }
                            CLI_STDOUT("");  // Newline
                            deltas = model.getBBox().getDelta();
                            CLI_STDOUT(" BoundBox(model)  = [" << (int)deltas.getX() << "mm, "
                                                               << (int)deltas.getY() << "mm, "
                                                               << (int)deltas.getZ() << "mm]");
                            deltas = object.getBBox().getDelta();
                            CLI_STDOUT(" BoundBox(object) = [" << (int)deltas.getX() << "mm, "
                                                               << (int)deltas.getY() << "mm, "
                                                               << (int)deltas.getZ() << "mm]");
                            deltas = ar.generalComparison.deltas;
                            CLI_STDOUT(" BoundBox(model) - BoundBox(object) = [" << (int)deltas.getX() << "mm, "
                                                                                 << (int)deltas.getY() << "mm, "
                                                                                 << (int)deltas.getZ() << "mm]");
                            // Face comparison
                            CLI_STDOUT(bold(" As a whole, model and object are " << (ar.generalComparison.similar ? "similar" : "different") << ""));

                            CLI_STDOUT("\n // FACE COMPARISONS //");
                            for (auto &fc : ar.faceComparisons) {
                                CLI_STDOUT(" [" << fc.modelFace << ", " << fc.objectFace << "]");
                                deltas = model.getFaces()[fc.modelFace].getMinBBox().getDelta();
                                CLI_STDOUT(" BoundBox(fmodel)  = [" << (int)deltas.getX() << "mm, "
                                                                    << (int)deltas.getY() << "mm, "
                                                                    << (int)deltas.getZ() << "mm]");
                                deltas = object.getFaces()[fc.objectFace].getMinBBox().getDelta();
                                CLI_STDOUT(" BoundBox(fobject) = [" << (int)deltas.getX() << "mm, "
                                                                    << (int)deltas.getY() << "mm, "
                                                                    << (int)deltas.getZ() << "mm]");
                                deltas = fc.deltas;
                                CLI_STDOUT(" BBox(fmodel) - BBox(fobject) = [" << (int)deltas.getX() << "mm, "
                                                                               << (int)deltas.getY() << "mm, "
                                                                               << (int)deltas.getZ() << "mm]");
                                CLI_STDOUT(bold(" Both faces are " << (fc.similar ? "similar" : "different") << "\n"));
                            }
                            // Conclusion
                            CLI_STDOUT("\n // CONCLUSION //");
                            CLI_STDOUT(bold(" The given model and object are " << (ar.similar ? "similar" : "different") << ""));
                            CLI_STDOUT("----------------------------------------------------------------");

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