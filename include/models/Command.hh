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
    kExit,     ///< Finalización del programa
    kHelp,     ///< Impresión de la ayuda
    kChar,     ///< Caracterización de objetos y fondo
    kModel,    ///< Opciones de los modelos
    kExplain,  ///< Impresión de las opciones actuales
    kList,     ///< Listado de objetos y modelos
    kAnalyze,  ///< Analizador de anomalías
    kUnknown   ///< Comando desconocido
} CommandType;

class Command {
   public:
    /**
     * Parser del comando
     * @params[0] input Linea con el comando
     * @return Command
     */
    static Command parse(std::string& input) {
        size_t start = 0, end;
        CommandType type;
        std::vector<std::string> params;
        std::string item;

        std::getline(std::cin, item);

        std::istringstream stream(item);

        while (std::getline(stream, item, ' ')) {
            params.push_back(item);
        }

        // Exit
        if (params[0].compare("exit") == 0) {
            return {kExit, params};
        }
        // Start
        else if (params[0].compare("help") == 0) {
            return {kHelp, params};
        }
        // Characterize Background
        else if (params[0].compare("char") == 0) {
            return {kChar, params};
        }
        // Characterize Object
        else if (params[0].compare("model") == 0) {
            return {kModel, params};
        }
        // Anomaly Detection
        else if (params[0].compare("explain") == 0) {
            return {kExplain, params};
        }
        // Listar objectos u modelos
        else if (params[0].compare("list") == 0) {
            return {kList, params};
        }
        // Detector de anomalias
        else if (params[0].compare("analyze") == 0) {
            return {kAnalyze, params};
        }

        // Comando desconocido
        return {};
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
    const int nparams() const { return params.size() - 1; }

    /**
     * Operador [] que permite obtener parametros del comando
     * @params[0] i Index del comando a acceder
     * @return Devuelve el parametro en la posición especificada
     */
    const std::string& operator[](unsigned int i) const {
        static const std::string empty_string("");
        return params.size() - 1 > i ? params[i + 1] : empty_string;
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
