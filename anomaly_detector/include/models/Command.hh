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
    kStart,
    kCharacterizatorBackground,
    kCharacterizatorObject,
    kAnomalyDetector,
    kExit,
    kUnknown,
} CommandType;

class Command {
   public:
    /**
     * Parser del comando
     * @param input Linea con el comando
     * @return Command
     */
    static Command parse(std::string& input) {
        size_t start = -1, end;
        std::vector<std::string> command;

        do {
            end = input.find_first_of(" ", ++start);

            std::string param = input.substr(start, end - start);  // Recuperamos parametro

            if (param.size() != 0) {
                command.push_back(param);  // Introducimos parametro}
            }

        } while (end != std::string::npos);

        if (command.size() == 0) {
            return {};
        } else {
            CommandType type;
            // TODO
            return {type, command};
        }
    }

    /**
     * Compruba si el comando está vacío
     * @return Devuelve true si el comando no contiene parametros
     */
    const bool empty() const { return command.size() == 0; }

    /**
     * Operador [] que permite obtener parametros del comando
     * @param i Index del comando a acceder
     * @return Devuelve el parametro en la posición especificada
     */
    const std::string& operator[](int i) const {
        static const std::string empty_string("");
        return command.size() > i ? command[i] : empty_string;
    }

    /**
     * Destructor
     */
    ~Command() {}

   private:
    Command(CommandType type, std::vector<std::string>& command) : type(type), command(command) {}
    Command() : type(kUnknown), command() {}

    CommandType type;                  ///< Tipo de comando
    std::vector<std::string> command;  ///< Vector de argumentos del comando
}

#endif  // COMMAND_CLASS_H
