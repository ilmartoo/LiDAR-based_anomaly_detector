/**
 * @file AnomalyDetector.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 09/05/2022
 *
 * Implementación de la clase AnomalyDetector
 *
 */

#include <iomanip>

#include "anomaly_detection/AnomalyDetector.hh"
#include "object_characterization/CharacterizedObject.hh"
#include "object_characterization/Model.hh"
#include "app/CLICommand.hh"

Anomalies AnomalyDetector::compare(const CharacterizedObject& obj, const Model& model) {
    bool first = true;
    bool isAnomaly[3];

    ModelFace bestFace;
    Vector bestDiff;
    double bestError;

    std::chrono::system_clock::time_point start, end;
    if (chrono) {
        start = std::chrono::high_resolution_clock::now();
    }

    // Escogemos la mejor cara
    for (int face = 0; face < Model::numFaces; ++face) {
        if (model.isFace((ModelFace)face)) {
            Vector objRadii = obj.getBBox().getRadii();
            Vector modRadii = model.getFace((ModelFace)face).getBBox().getRadii();

            double error = objRadii.distance3D(modRadii);  // Error cuadratico

            // No hay cara con la que comparar
            if (first) {
                first = false;
                bestError = error;
                bestFace = (ModelFace)face;
                bestDiff = objRadii - modRadii;
            }
            // Comparación de errores cuadraticos
            else {
                if (error < bestError) {
                    bestFace = (ModelFace)face;
                    bestDiff = objRadii - modRadii;
                }
            }
        }
    }

    // Detección de anomalías
    for (int i = 0; i < 3; ++i) {
        isAnomaly[i] = bestDiff[i] <= radiiMaxDiff ? false : true;
    }

    if (chrono) {
        end = std::chrono::high_resolution_clock::now();

        double duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1.e9;

        CLI_STDOUT("Anomaly detection lasted " << std::setprecision(6) << duration << std::setprecision(2) << " s");
    }

    return Anomalies(bestFace, {isAnomaly[0], bestDiff[0]}, {isAnomaly[1], bestDiff[1]}, {isAnomaly[2], bestDiff[2]});
}