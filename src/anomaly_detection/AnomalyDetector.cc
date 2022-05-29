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

    //////////////////////////
    // Comparación de caras //
    //////////////////////////

    std::vector<FaceComparison> faceComparisons;

    arma::mat deltaVolumes(obj.getFaces().size(), mod.getFaces().size());  // Matriz de deltas de volumenes
    std::list<size_t> oIndex;                                              // Caras del objeto por escoger
    std::list<size_t> mIndex;                                              // Caras del modelo por escoger

    for (size_t i = obj.getFaces().size() - 1; i > 0; --i) {
        oIndex.push_front(i);
    }
    for (size_t i = mod.getFaces().size() - 1; i > 0; --i) {
        mIndex.push_front(i);
    }

    size_t oMinIndex, mMinIndex;
    double minDeltaVolume;
#pragma omp parallel num_threads(PARALELIZATION_NUM_THREADS)
    {
#pragma omp for collapse(2) schedule(guided)
        for (size_t i = 0; i < obj.getFaces().size(); ++i) {
            for (size_t j = 0; j < mod.getFaces().size(); ++j) {
                deltaVolumes(i, j) = std::fabs(obj.getFaces()[i].getMinBBox().volume() - mod.getFaces()[i].getMinBBox().volume());
            }
        }
        // Buscamos en cada iteración el delta mínimo de las caras restantes
        while (oIndex.size() > 0 && mIndex.size() > 0) {
#pragma omp single
            {
                oMinIndex = *oIndex.begin();
                mMinIndex = *mIndex.begin();
                minDeltaVolume = deltaVolumes(oMinIndex, mMinIndex);
            }
            // Implicit barrier
            // Recorremos los elementos restantes de la matriz en busca del mínimo
#pragma omp for collapse(2) schedule(guided)
            for (auto oitr = oIndex.begin(); oitr != oIndex.end(); oitr++) {
                for (auto mitr = mIndex.begin(); mitr != mIndex.end(); mitr++) {
                    // El primer elemento es el mínimo por defecto
                    if (oitr == oIndex.begin() && mitr == mIndex.begin()) {
                        continue;
                    } else {
#pragma omp critical
                        if (deltaVolumes(*oitr, *mitr) < minDeltaVolume) {
                            oMinIndex = *oitr;
                            mMinIndex = *mitr;
                            minDeltaVolume = deltaVolumes(oMinIndex, mMinIndex);
                        }
                    }
                }
            }
        }
#pragma omp single
        {
            Vector faceDelta = mod.getFaces()[oMinIndex].getMinBBox().getDelta() - obj.getFaces()[oMinIndex].getMinBBox().getDelta();
            Vector mfnormal = Geometry::computeNormal(mod.getFaces()[oMinIndex].getPoints());
            Vector ofnormal = Geometry::computeNormal(obj.getFaces()[oMinIndex].getPoints());
            faceComparisons.push_back(FaceComparison((std::fabs(faceDelta.getX()) <= MAX_DIMENSION_DELTA &&
                                                      std::fabs(faceDelta.getY()) <= MAX_DIMENSION_DELTA &&
                                                      std::fabs(faceDelta.getZ()) <= MAX_DIMENSION_DELTA)
                                                         ? true
                                                         : false,
                                                     oMinIndex,
                                                     mMinIndex,
                                                     faceDelta));
        }
        // Implicit barrier
    }

    if (chrono) {
        end = std::chrono::high_resolution_clock::now();

        double duration = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1.e9;

        CLI_STDOUT("Anomaly detection lasted " << std::setprecision(6) << duration << std::setprecision(2) << " s");
    }

    return AnomalyReport(similar, generalComparison, deltaFaces, faceComparisons);
}