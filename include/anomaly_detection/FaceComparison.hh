/**
 * @file FaceComparison.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 29/05/2022
 *
 * Definición e implementación de la clase FaceComparison
 *
 */

#ifndef FACECOMPARISON_CLASS_H
#define FACECOMPARISON_CLASS_H

#include <cstddef>

#include "anomaly_detection/Comparison.hh"
#include "models/Point.hh"
#include "app/config.h"

class FaceComparison : public Comparison {
   public:
    const size_t modelFace;   ///< Índice de la cara del modelo usada
    const size_t objectFace;  ///< Índice de la cara del objeto usada

    /**
     * Constructor
     * @param objectFace Índice de la cara del objeto
     * @param modelFace Índice de la cara del modelo
     * @param deltas Deltas de los lados de la bounding box mínima de la cara
     * @param desviation Ángulo entre normales de las caras en radianes
     * @param result Resultado final de la comparación
     */
    FaceComparison(bool similar, size_t modelFace, size_t objectFace, const Vector &deltas)
        : Comparison(similar, deltas),
          modelFace(modelFace),
          objectFace(objectFace) {}
    /**
     * Destructor
     */
    ~FaceComparison() {}
};

#endif  // FACECOMPARISON_CLASS_H
