/**
 * @file Command.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 19/04/2022
 *
 * Definicion e implementación del objeto Command
 *
 */

#ifndef COMMAND_CLASS_H
#define COMMAND_CLASS_H

#include <vector>
#include <string>

typedef enum {
    kStart,                      ///< Comenzar ejecución secuencial
    kCharacterizatorBackground,  ///< Caracterizar el fondo
    kCharacterizatorObject,      ///< Caracterizar el objeto
    kAnomalyDetector,            ///< Detectar anomalías
    kExit,                       ///< Salir del programa
    kHelp,                       ///< Mostrar ayuda
    kUnknown,                    ///< Comando desconocido
} CommandType;

class Command {
   public:
    /**
     * Parser del comando
     * @param input Linea con el comando
     * @return Command
     */
    static Command parse(std::string& input) {
        size_t start = 0, end;
        CommandType type;
        std::vector<std::string> params;
        std::string param;

        end = input.find_first_of(" ", start);
        param = input.substr(start, end - start);  // Recuperamos comando
        start = end;
        // Exit
        if (param.compare("exit") == 0) {
            type = kExit;
        }
        // Start
        else if (param.compare("start") == 0) {
            type = kStart;
        }
        // Characterize Background
        else if (param.compare("background") == 0) {
            type = kCharacterizatorBackground;
        }
        // Characterize Object
        else if (param.compare("object") == 0) {
            type = kCharacterizatorObject;
        }
        // Anomaly Detection
        else if (param.compare("detect") == 0) {
            type = kAnomalyDetector;
        }
        // Comando desconocido
        else {
            return {};
        }

        while (end != std::string::npos) {
            end = input.find_first_of(" ", ++start);
            param = input.substr(start, end - start);  // Recuperamos parametro
            start = end;

            if (param.size() != 0) {
                params.push_back(param);  // Introducimos parametro}
            }
        }

        return {type, params};
    }
    /**
     * Devuelve el tipo del comando
     * @return Tipo de comando
     */
    const CommandType getType() const { return type; }

    /**
     * Compruba si el comando es válido
     * @return Devuelve true si el comando es válido
     */
    const bool isValid() const { return type != kUnknown; }

    /**
     * Obtiene el número de parametros del comando
     * @return Numero de parámetros 
     */
    const int nparams() const { return params.size(); }

    /**
     * Operador [] que permite obtener parametros del comando
     * @param i Index del comando a acceder
     * @return Devuelve el parametro en la posición especificada
     */
    const std::string& operator[](unsigned int i) const {
        static const std::string empty_string("");
        return params.size() > i ? params[i] : empty_string;
    }

    /**
     * Destructor
     */
    ~Command() {}

   private:
    Command(CommandType type, std::vector<std::string>& params) : type(type), params(params) {}
    Command() : type(kUnknown), params() {}

    CommandType type;                 ///< Tipo de comando
    std::vector<std::string> params;  ///< Vector de argumentos del comando
};

#endif  // COMMAND_CLASS_H
