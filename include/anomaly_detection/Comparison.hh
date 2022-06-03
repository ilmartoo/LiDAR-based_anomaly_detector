/**
 * @file Comparison.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 29/05/2022
 *
 * @brief Definición e implementación de la clase Comparison
 *
 */

#ifndef COMPARISON_CLASS_H
#define COMPARISON_CLASS_H

#include "models/Point.hh"

/**
 * @brief Comparación entre objeto y modelo
 */
class Comparison {
   public:
    const bool similar;   ///< Booleano que establece si las bouding boxes son similares
    const Vector deltas;  ///< Delta de las dimensiones de las bounding boxes (deltaModelo - deltaObjeto)

    /**
     * Constructor
     * @param similar Resultado final de la comparación
     * @param deltas Deltas de los lados de la bounding box mínima de la cara
     */
    Comparison(bool similar, const Vector &deltas) : similar(similar), deltas(deltas) {}
    /**
     * Destructor
     */
    ~Comparison() {}
};

#endif  // COMPARISON_CLASS_H
