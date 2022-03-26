/**
 * @file InputParser.hh
 * @author iain (https://stackoverflow.com/users/85381/iain)
 * @date 20/03/2022
 *
 * Parser de argumentos de linea de comandos
 *
 */

#ifndef __INPUTPARSER_CLASS_H
#define __INPUTPARSER_CLASS_H

#include <string>
#include <vector>
#include <algorithm>

class InputParser {
   public:
    InputParser(int &argc, char **argv) {
        for (int i = 1; i < argc; ++i)
            this->tokens.push_back(std::string(argv[i]));
    }
    /**
     * Devuelve un string con el parámetro de la opción especificada
     * @author iain (https://stackoverflow.com/users/85381/iain)
     * @param option Opción a recuperar
     * @return const std::string& String con los datos de la opción (string vacío si no se ha encontrado)
     */
    const std::string &getCmdOption(const std::string &option) const {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
            return *itr;
        }
        static const std::string empty_string("");
        return empty_string;
    }

    /**
     * Comprueba si una opción se ha especificado
     * @author iain (https://stackoverflow.com/users/85381/iain)
     * @param option Opción a comprobar
     * @return true Existe la opción especificada
     * @return false No existe la opción especificada
     */
    bool cmdOptionExists(const std::string &option) const {
        return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
    }

   private:
    std::vector<std::string> tokens;
};

#endif  //__INPUTPARSER_CLASS_H