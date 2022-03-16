#ifndef __ANOMALYDETECTOR_INTERFACE_H
#define __ANOMALYDETECTOR_INTERFACE_H

#include <string>
#include <vector>

#include "CharacteristicsVector.hpp"
#include "Model.hpp"

class IAnomalyDetector {
   public:
    /**
     * Carga los modelos disponibles del archivo especificado
     * @param
     */
    virtual void loadModels(std::string path) = 0;
    virtual std::vector<Model> getModels() = 0;
    virtual void setModel(Model m) = 0;
    virtual void compare(CharacteristicsVector cv) = 0;
};

#endif  //__ANOMALYDETECTOR_INTERFACE_H