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
#include "app/CLICommand.hh"

Anomalies AnomalyDetector::compare(const CharacterizedObject& obj, const Model& model) {
    std::chrono::system_clock::time_point start, end;
    if (chrono) {
        start = std::chrono::high_resolution_clock::now();
    }

    /* TODO: Comparación de caras */

    if (chrono) {
        end = std::chrono::high_resolution_clock::now();

        double duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1.e9;

        CLI_STDOUT("Anomaly detection lasted " << std::setprecision(6) << duration << std::setprecision(2) << " s");
    }

    return Anomalies({false, 0}, {false, 0}, {false, 0});
}