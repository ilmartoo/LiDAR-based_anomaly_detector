/**
 * @file InputParser.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 20/03/2022
 *
 * @brief Parser de argumentos de linea de comandos
 *
 */

#ifndef INPUTPARSER_CLASS_H
#define INPUTPARSER_CLASS_H

#include <string>
#include <vector>

#include "logging/debug.hh"

/**
 * @brief Parser del input del usuario
 */
class InputParser {
   private:
    std::vector<std::string> params;

   public:
    /**
     * Constructor
     * @param nargs Numero de argumentos
     * @param args Vector de argumentos
     */
    InputParser(int nargs, const char **args) {
        for (int i = 1; i < nargs; ++i) {
            params.push_back(std::string(args[i]));
        }
    }
    /**
     * Devuelve un string con el parámetro de la opción especificada
     * @param option Opción a recuperar
     * @return String con los datos de la opción o string vacío si no se ha encontrado
     */
    const std::string &getParam(const std::string &option) const {
        for (auto itr = params.begin(); itr != params.end(); ++itr) {
            if (*itr == option && ++itr != params.end()) {
                return *itr;
            }
        }

        static const std::string empty_string("");
        return empty_string;
    }
    /**
     * Obtiene el valor en la posición especificada
     * @param index Posición del valor a recuperar
     * @return String con los datos de la opción o string vacío si no se ha encontrado
     */
    const std::string &getParam(unsigned int index) const {
        if (index < params.size()) {
            return params.at(index);
        }
        static const std::string emptyString("");
        return emptyString;
    }

    /**
     * Comprueba si una opción se ha especificado
     * @param option Opción a comprobar
     * @return true Existe la opción especificada
     */
    bool hasParam(const std::string &option) const {
        for (auto &p : params) {
            if (p == option) {
                return true;
            }
        }
        return false;
    }
};

#endif  // INPUTPARSER_CLASS_H