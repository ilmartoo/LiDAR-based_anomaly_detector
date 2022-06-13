/**
 * @file AnomalyDetector.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 09/05/2022
 *
 * @brief Implementación de la clase AnomalyDetector
 *
 */

#include <iomanip>
#include <list>
#include <cmath>

#include "armadillo"

#include "anomaly_detection/AnomalyDetector.hh"
#include "object_characterization/CharacterizedObject.hh"
#include "app/CLI.hh"
#include "app/config.h"

AnomalyReport AnomalyDetector::compare(const CharacterizedObject& obj, const Model& mod) {
    std::chrono::system_clock::time_point start, end;
    if (chrono) {
        start = std::chrono::high_resolution_clock::now();
    }

    bool similar = obj.getFaces().size() > 0 && obj.getFaces().size() == mod.getFaces().size();
    long deltaFaces = mod.getFaces().size() - obj.getFaces().size();
    long totalAnomalies = deltaFaces < 0 ? -deltaFaces : deltaFaces;

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

    if (similar) {
        similar = generalComparison.similar;
    }
    if (!generalComparison.similar) {
        ++totalAnomalies;
    }

    //////////////////////////
    // Comparación de caras //
    //////////////////////////

    std::vector<FaceComparison>
        faceComparisons;

    double deltaVolumes[obj.getFaces().size()][mod.getFaces().size()];  // Matriz de deltas de volumenes
    std::vector<bool> objFaceUsage(obj.getFaces().size(), false);       // Caras del objeto ya usadas
    std::vector<bool> modFaceUsage(mod.getFaces().size(), false);       // Caras del modelo ya usadas

    size_t tcomp = deltaFaces < 0 ? mod.getFaces().size() : obj.getFaces().size();  // Máximo de iteraciones posibles en la comparación de caras
    size_t objFaceIndex, modFaceIndex;
    double minDeltaVolume;
#pragma omp parallel
    {
#pragma omp for collapse(2) schedule(OMP_SCHEDULE_TYPE, OMP_CHUNK_SIZE)
        for (size_t i = 0; i < obj.getFaces().size(); ++i) {
            for (size_t j = 0; j < mod.getFaces().size(); ++j) {
                deltaVolumes[i][j] = std::fabs(mod.getFaces()[j].getMinBBox().volume() - obj.getFaces()[i].getMinBBox().volume());
            }
        }
        // Buscamos en cada iteración el delta mínimo de las caras restantes
        for (size_t comp = 0; comp < tcomp; ++comp) {
#pragma omp single
            {
                for (objFaceIndex = 0; objFaceUsage[objFaceIndex] && objFaceIndex < objFaceUsage.size(); ++objFaceIndex) {}
                for (modFaceIndex = 0; modFaceUsage[modFaceIndex] && modFaceIndex < modFaceUsage.size(); ++modFaceIndex) {}
                minDeltaVolume = deltaVolumes[objFaceIndex][modFaceIndex];
            }
            // Implicit barrier
            // Recorremos los elementos restantes de la matriz en busca del mínimo
#pragma omp for collapse(2) schedule(OMP_SCHEDULE_TYPE, OMP_CHUNK_SIZE)
            for (size_t oi = 0; oi < obj.getFaces().size(); ++oi) {
                for (size_t mi = 0; mi < mod.getFaces().size(); ++mi) {
                    // El primer elemento es el mínimo por defecto
                    // Pasamos las caras ya comparadas
                    if (objFaceUsage[oi] || modFaceUsage[mi] || (oi == objFaceIndex && mi == modFaceIndex)) {
                        continue;
                    } else {
#pragma omp critical
                        if (deltaVolumes[oi][mi] < minDeltaVolume) {
                            objFaceIndex = oi;
                            modFaceIndex = mi;
                            minDeltaVolume = deltaVolumes[objFaceIndex][modFaceIndex];
                        }
                    }
                }
            }

#pragma omp single
            {
                // Eliminamos caras
                modFaceUsage[modFaceIndex] = true;
                objFaceUsage[objFaceIndex] = true;

                // Comparación
                Vector faceDelta = mod.getFaces()[modFaceIndex].getMinBBox().getDelta() - obj.getFaces()[objFaceIndex].getMinBBox().getDelta();
                faceComparisons.push_back(FaceComparison((std::fabs(faceDelta.getX()) <= MAX_DIMENSION_DELTA &&
                                                          std::fabs(faceDelta.getY()) <= MAX_DIMENSION_DELTA &&
                                                          std::fabs(faceDelta.getZ()) <= MAX_DIMENSION_DELTA)
                                                             ? true
                                                             : false,
                                                         modFaceIndex,
                                                         objFaceIndex,
                                                         faceDelta));

                if (similar) {
                    similar = faceComparisons.back().similar;
                }
                if (!faceComparisons.back().similar) {
                    ++totalAnomalies;
                }
            }
            // Implicit barrier
        }
    }

    // Guardado de caras sin emparejar
    std::vector<size_t> unmatched;
    std::vector<bool>& faceUsage = deltaFaces < 0 ? objFaceUsage : modFaceUsage;
    for (size_t i = 0; i < faceUsage.size(); ++i) {
        if (!faceUsage[i]) {
            unmatched.push_back(i);
        }
    }

    if (chrono) {
        end = std::chrono::high_resolution_clock::now();

        double duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1.e9;

        CLI_STDOUT("Anomaly detection lasted " << std::setprecision(6) << duration << std::setprecision(2) << " s");
    }

    return AnomalyReport(similar, generalComparison, deltaFaces, totalAnomalies, faceComparisons, unmatched);
}