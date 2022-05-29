/**
 * @file Config.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 29/05/2022
 *
 * Clase almacén de parámetros utilizados en la ejecución de la aplicación
 *
 */

#ifndef CONFIG_DEFINITIONS_H
#define CONFIG_DEFINITIONS_H

#include <cmath>

/* Paralelización */
#define PARALELIZATION_NUM_THREADS 6  ///< Número de hilos utilizados en la paralelización

/* Geometría */
#define RAD_PER_DEG                (M_PI / 180.)  ///< Total de radianes correspondientes a un grado

/* Caracterización de objetos */
#define MIN_CLUSTER_POINTS         20               ///< Número mínimo de puntos que debe tener un cluster inicial para ser considerado
#define MIN_FACE_POINTS            15               ///< Número mínimo de puntos que debe tener una cara inicial para ser considerada
#define CLUSTER_POINT_PROXIMITY    20               ///< Proximidad máxima (mm) de un punto hacia uno origen para pertenecer al mismo cluster
#define FACE_POINT_PROXIMITY       30               ///< Proximidad máxima (mm) de un punto hacia uno origen para pertenecer a la misma cara
#define MAX_NORMAL_VECT_ANGLE_OC   3 * RAD_PER_DEG  ///< Radianes máximos de separación angular entre normales para pertenecer a la misma cara

/* Detección de anomalías */
#define MAX_DIMENSION_DELTA        40                 ///< Máxima diferencia (mm) entre medidas de una bounding box en la misma dimensión
#define MAX_NORMAL_VECT_ANGLE_AD   1.5 * RAD_PER_DEG  ///< Radianes máximos de separación angular entre normales de las caras para considerarse similares

#endif  // CONFIG_DEFINITIONS_H