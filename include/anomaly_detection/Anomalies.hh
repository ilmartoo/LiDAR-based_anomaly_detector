/**
 * @file Anomalies.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 10/05/2022
 *
 * Definición e implementación de la clase Anomalies
 *
 */

#include <utility>

#include "object_characterization/Model.hh"

#ifndef ANOMALIES_CLASS_H
#define ANOMALIES_CLASS_H

class Anomalies {
   public:
    const ModelFace face;                   ///< Cara del modelo que mejor se corresponde con el objecto
    const std::pair<bool, double> xradius;  ///< Anomalía del radio en x
    const std::pair<bool, double> yradius;  ///< Anomalía del radio en y
    const std::pair<bool, double> zradius;  ///< Anomalía del radio en z

    /**
     * Constructor
     * @param face Cara del modelo que mejor se corresponde con el objecto
     * @param xrd Anomalía del radio en x
     * @param yrd Anomalía del radio en y
     * @param zrd Anomalía del radio en z
     */
    Anomalies(ModelFace face, const std::pair<bool, double>& xrd, const std::pair<bool, double>& yrd, const std::pair<bool, double>& zrd) : face(face), xradius(xrd), yradius(yrd), zradius(zrd) {}
};

#endif  // ANOMALIES_CLASS_H