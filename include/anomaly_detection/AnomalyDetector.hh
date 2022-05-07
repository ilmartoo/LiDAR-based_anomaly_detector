/**
 * @file AnomalyDetector.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 06/05/2022
 * 
 * 
 */

#ifndef ANOMALYDETECTOR_CLASS_H
#define ANOMALYDETECTOR_CLASS_H

#include <string>
#include <vector>
#include <thread>

#include "models/CharacterizedObject.hh"

class AnomalyDetector {
   public:
    /**
     * Destructor virtual
     */
    ~AnomalyDetector() {}

    /**
     * Carga los modelos disponibles del archivo especificado
     * @param
     */
    void loadModels(std::string path);
    std::vector<Model> getModels();
    void setModel(Model m);
    void compare(CharacteristicsVector cv);

   protected:
    std::thread executionThread;  ///< Hilo de ejecución del detector de anomalías
};

#endif  //ANOMALYDETECTOR_INTERFACE_H