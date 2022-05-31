/**
 * @file AnomalyDetector.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 09/05/2022
 *
 * Implementación de la clase AnomalyDetector
 *
 */

#include <iomanip>
#include <list>
#include <cmath>

#include "armadillo"

#include "anomaly_detection/AnomalyDetector.hh"
#include "object_characterization/CharacterizedObject.hh"
#include "models/Geometry.hh"
#include "app/CLICommand.hh"
#include "app/config.h"

AnomalyReport AnomalyDetector::compare(const CharacterizedObject& obj, const Model& mod) {
    std::chrono::system_clock::time_point start, end;
    if (chrono) {
        start = std::chrono::high_resolution_clock::now();
    }

    bool similar = obj.getFaces().size() > 0;
    long deltaFaces = mod.getFaces().size() - obj.getFaces().size();

    /////////////////////////
    // Comparación general //
    /////////////////////////

    Vector generalDelta = mod.getBBox().getDelta() - obj.getBBox().getDelta();

    Comparison generalComparison((std::fabs(generalDelta.getX()) <= MAX_DIMENSION_DELTA &&
                                  std::fabs(generalDelta.getY()) <= MAX_DIMENSION_DELTA &&
                                  std::fabs(generalDelta.getZ()) <= MAX_DIMENSION_DELTA)
                                     ? true
                                     : false,
                                 generalDelta);

    similar = similar ? generalComparison.similar : similar;

    //////////////////////////
    // Comparación de caras //
    //////////////////////////

    std::vector<FaceComparison> faceComparisons;

    double deltaVolumes[obj.getFaces().size()][mod.getFaces().size()];  // Matriz de deltas de volumenes
    std::vector<bool> objFaceUsage(obj.getFaces().size(), false);       // Caras del objeto ya usadas
    std::vector<bool> modFaceUsage(mod.getFaces().size(), false);       // Caras del modelo ya usadas

    size_t tcomp = deltaFaces < 0 ? mod.getFaces().size() : obj.getFaces().size();  // Máximo de iteraciones posibles en la comparación de caras
    size_t objFaceIndex, modFaceIndex;
    double minDeltaVolume;
    // #pragma omp parallel num_threads(PARALELIZATION_NUM_THREADS)
    {
        // #pragma omp for collapse(2) schedule(guided)
        for (size_t i = 0; i < mod.getFaces().size(); ++i) {
            for (size_t j = 0; j < obj.getFaces().size(); ++j) {
                deltaVolumes[i][j] = std::fabs(mod.getFaces()[i].getMinBBox().volume() - obj.getFaces()[j].getMinBBox().volume());
            }
        }
        // Buscamos en cada iteración el delta mínimo de las caras restantes
        for (size_t comp = 0; comp < tcomp; ++comp) {
            // #pragma omp single
            {
                for (objFaceIndex = 0; objFaceUsage[objFaceIndex] && objFaceIndex < objFaceUsage.size(); ++objFaceIndex) {}
                for (modFaceIndex = 0; modFaceUsage[modFaceIndex] && modFaceIndex < modFaceUsage.size(); ++modFaceIndex) {}
                minDeltaVolume = deltaVolumes[objFaceIndex][modFaceIndex];
            }
            // Implicit barrier
            // Recorremos los elementos restantes de la matriz en busca del mínimo
            // #pragma omp for collapse(2) schedule(guided)
            for (size_t oi = 0; oi < obj.getFaces().size(); ++oi) {
                for (size_t mi = 0; mi < mod.getFaces().size(); ++mi) {
                    // El primer elemento es el mínimo por defecto
                    // Pasamos las caras ya comparadas
                    if (objFaceUsage[oi] || modFaceUsage[mi] || (oi == objFaceIndex && mi == modFaceIndex)) {
                        continue;
                    } else {
                        // #pragma omp critical
                        if (deltaVolumes[oi][mi] < minDeltaVolume) {
                            objFaceIndex = oi;
                            modFaceIndex = mi;
                            minDeltaVolume = deltaVolumes[objFaceIndex][modFaceIndex];
                        }
                    }
                }
            }

            // #pragma omp single
            {
                // Eliminamos caras
                modFaceUsage[modFaceIndex] = true;
                objFaceUsage[objFaceIndex] = true;

                // Comparación
                Vector faceDelta = mod.getFaces()[objFaceIndex].getMinBBox().getDelta() - obj.getFaces()[objFaceIndex].getMinBBox().getDelta();
                faceComparisons.push_back(FaceComparison((std::fabs(faceDelta.getX()) <= MAX_DIMENSION_DELTA &&
                                                          std::fabs(faceDelta.getY()) <= MAX_DIMENSION_DELTA &&
                                                          std::fabs(faceDelta.getZ()) <= MAX_DIMENSION_DELTA)
                                                             ? true
                                                             : false,
                                                         objFaceIndex,
                                                         modFaceIndex,
                                                         faceDelta));

                similar = similar ? faceComparisons.back().similar : similar;
            }
            // Implicit barrier
        }
    }

    if (chrono) {
        end = std::chrono::high_resolution_clock::now();

        double duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1.e9;

        CLI_STDOUT("Anomaly detection lasted " << std::setprecision(6) << duration << std::setprecision(2) << " s");
    }

    return AnomalyReport(similar, generalComparison, deltaFaces, faceComparisons);
}