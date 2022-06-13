/**
 * @file Config.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 29/05/2022
 *
 * @brief Clase almacén de parámetros utilizados en la ejecución de la aplicación
 *
 */

#ifndef CONFIG_DEFINITIONS_H
#define CONFIG_DEFINITIONS_H

#include <cmath>

/* Default parameters */
#define DEFAULT_BROADCAST_CODE      "3WEDH7600101621"  ///< Código por defecto para el LiDAR (Código del LIVOX Horizon utilizado)
#define DEFAULT_TIMER_MODE          kNoChrono          ///< Cronómetro por defecto
#define DEFAULT_OBJECT_FRAME_T      1500               ///< Tiempo de escaneo del objeto (ms) por defecto
#define DEFAULT_BACKGROUND_FRAME_T  5000               ///< Tiempo de escaneo del fondo (ms) por defecto
#define DEFAULT_MIN_RELECTIVITY     0.0f               ///< Umbral de reflectividad de los puntos
#define DEFAULT_BACKGROUND_DISTANCE 0.01f              ///< Umbral de distancia al fondo (m)
#define DEFAULT_NUM_THREADS         4                  ///< Número de hilos utilizados en la paralelización por defecto

/* Tipos de cronometros */
enum ChronoMode {
    kNoChrono = 0,                   ///< Ejecución sin medida de tiempo
    kChronoCharacterization = 0b01,  ///< Ejecución con medida de tiempo en la caracterización de objetos
    kChronoAnomalyDetection = 0b10,  ///< Ejecución con medida de tiempo en la detección de anomalías
    kChronoAll = 0b11,               ///< Ejecución con medida de tiempo en todo el programa
};

/* OpenMP **/
#define OMP_SCHEDULE_TYPE           guided  ///< Tipo de distribución para los bucles for
#define OMP_CHUNK_SIZE              1       ///< Tipo de distribución para los bucles for

/* Geometría */
#define RAD_PER_DEG                 (M_PI / 180.)  ///< Total de radianes correspondientes a un grado

/* Caracterización de objetos */
#define MIN_CLUSTER_POINTS          20                  ///< Número mínimo de puntos que debe tener un cluster inicial para ser considerado
#define CLUSTER_POINT_PROXIMITY     20                  ///< Proximidad máxima (mm) de un punto hacia uno origen para pertenecer al mismo cluster
#define MIN_FACE_POINTS             20                  ///< Número mínimo de puntos que debe tener una cara inicial para ser considerada
#define NORMAL_CALC_POINT_PROXIMITY 60                  ///< Proximidad máxima (mm) de los puntos vecinos que se usarán para calcular la normal de puntos
#define FACE_POINT_PROXIMITY        30                  ///< Proximidad máxima (mm) de un punto hacia uno origen para pertenecer a la misma cara
#define MAX_NORMAL_VECT_ANGLE       5 * RAD_PER_DEG     ///< (Parcial 1/2) Radianes máximos de separación angular entre normales para pertenecer a la misma cara
#define MAX_MEAN_VECT_ANGLE         45 * RAD_PER_DEG  ///< (Parcial 2/2) Radianes máximos de separación angular entre una normal y la normal media del cluster para pertenecer a la misma cara
#define MAX_MEAN_VECT_ANGLE_SINGLE  25 * RAD_PER_DEG    ///< Radianes máximos de separación angular entre una normal y la normal media del cluster para pertenecer a la misma cara

/* Detección de anomalías */
#define MAX_DIMENSION_DELTA         40                 ///< Máxima diferencia (mm) entre medidas de una bounding box en la misma dimensión
#define MAX_NORMAL_VECT_ANGLE_AD    1.5 * RAD_PER_DEG  ///< Radianes máximos de separación angular entre normales de las caras para considerarse similares

#endif  // CONFIG_DEFINITIONS_H