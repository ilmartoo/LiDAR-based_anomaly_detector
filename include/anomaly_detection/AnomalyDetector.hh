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

#include "object_characterization/CharacterizedObject.hh"
#include "object_characterization/Model.hh"

class AnomalyDetector {
   private:
    bool chrono;  ///< Activador de la medicion de tiempos

   public:
    /**
     * Constructor
     */
    AnomalyDetector(const bool chrono) : chrono(chrono) {}

    /**
     * Destructor virtual
     */
    ~AnomalyDetector() {}

    /**
     * Compara un objecto con un modelo especificado
     * @param obj Objeto a comparar
     * @param model Modelo sobre el que se comparará el objeto
     * @return ...
     */
    void /* Differences */ compare(const CharacterizedObject& obj, const Model& model);

    ////// Setters
    /**
     * Setter del cronometraje
     * @param chrono Booleano para establecer el nuevo cronometraje
     */
    void setChrono(bool chrono) { this->chrono = chrono; }

    ////// Getters
    /**
     * Devuelve si se está cronometrando la caracterización
     * @return Booleano conforme está activado el cronometraje
     */
    bool isChrono() const { return this->chrono; }
};

#endif  // ANOMALYDETECTOR_INTERFACE_H