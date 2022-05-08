/**
 * @file ScannerLidar.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 23/03/2022
 *
 * Definicion del objeto ScannerLidar
 *
 */

#ifndef SCANNERLIDAR_CLASS_H
#define SCANNERLIDAR_CLASS_H

#include <iostream>
#include <string>
#include <fstream>
#include <functional>
#include <string.h>

#include "livox_sdk.h"

#include "scanner/IScanner.hh"
#include "models/Point.hh"

#include "logging/debug.hh"

/**
 * Estados en los que puede estar el escaner LiDAR
 */
typedef enum {
    kDeviceStateDisconnect = 0,  ///< Desconectado
    kDeviceStateConnect = 1,     ///< Conectado
    kDeviceStateSampling = 2,    ///< Retransmitiendo puntos
} DeviceState;

/**
 * Datos del sensor LiDAR
 */
struct DeviceItem {
    DeviceItem(const char broadcast_code[kBroadcastCodeSize]) { strncpy(info.broadcast_code, broadcast_code, kBroadcastCodeSize - 1); }

    uint8_t handle;            ///< Handler
    DeviceState device_state;  ///< Estado del sensor
    DeviceInfo info;           ///< Propiedades del sensor
};

/**
 * Implementación de la interfaz IScanner para la lectura de puntos del modelo de LiDAR Livox Horizon
 */
class ScannerLidar : public IScanner {
   private:
    DeviceItem lidar;  ///< Datos del sensor LiDAR

   public:
    /**
     * Devuelve la instancia única creada del escaner
     * @return Instancia única del escaner
     */
    static ScannerLidar *getInstance() { return (ScannerLidar *)instance; }

    /**
     * Crea una instancia unica del escaner si no existe
     * @param code Broadcast del sensor
     * @return Instancia única del escaner
     */
    static IScanner *create(const char code[kBroadcastCodeSize]) {
        static ScannerLidar scanner = {code};
        instance = (IScanner *)&scanner;
        return instance;
    }

    /**
     * Inicialización del escaner
     * @return Se devolverá true si se ha establecido el escaner correctamente
     */
    bool init();

    /**
     * Comienza a escanear puntos.
     * Si no se quiere escanear hasta el final del archivo será responsabilidad del programador
     * hacer una llamada a la función pause() cuando se requiera parar el escaneo.
     * @return Se devolverá un ScanCode respecto a como ha finalizado el escaneo
     */
    ScanCode scan();

    /**
     * Pausa el escaneo de puntos
     */
    void pause();

    /**
     * Establece la función especificada como función de callback a la que se llamará cada vez que
     * se escanee un nuevo punto
     * @param func Función de callback a ser llamada por el sensor
     * @return Se devolverá true si se ha establecido el callback correctamente
     */
    bool setCallback(const std::function<void(const Point &p)> func);

    /**
     * Finaliza el escaner
     */
    void stop();

   private:
    /**
     * Constructor del objeto ScannerLidar
     * @param broadcast_code Codigo de broadcast del sensor
     */
    ScannerLidar(const char code[kBroadcastCodeSize]) : lidar(code) {}
    /**
     * Destructor del scanner
     */
    ~ScannerLidar() {}

   public:
    /**
     * Obtiene los datos del punto enviado por el sensor
     * @param data Paquete contenedor de datos
     * @param data_num Número de paquete
     * @param client_data Cliente al que pertenece
     */
    friend void getLidarData(uint8_t handle, LivoxEthPacket *data, uint32_t data_num, void *client_data);

    /**
     * Callback para recepción de información del sensor
     * @param info Información del sensor de broadcast
     */
    friend void onDeviceBroadcast(const BroadcastDeviceInfo *info);

    /**
     * Recepción de mensajes de error
     * @param status Estatus del sensor
     * @param handle Handler del sensor
     * @param message Mensaje de error
     */
    friend void onLidarErrorStatusCallback(livox_status status, uint8_t handle, ErrorMessage *message);

    /**
     * Se ejecuta con el comienzo del escaneo de puntos
     * @param status Estatus del sensor
     * @param handle Handler del sensor
     * @param response Respuesta del sensor
     * @param data Datos enviador por el sensor
     */
    friend void onSampleCallback(livox_status status, uint8_t handle, uint8_t response, void *data);

    /**
     * Se ejecuta con la finalización del escaneo de puntos
     * @param status Estatus del sensor
     * @param handle Handler del sensor
     * @param response Respuesta del sensor
     * @param data Datos enviador por el sensor
     */
    friend void onStopSampleCallback(livox_status status, uint8_t handle, uint8_t response, void *data);

    /**
     * Conecta el sensor
     * @param info Información del sensor
     */
    friend void lidarConnect(const DeviceInfo *info);

    /**
     * Desconecta el sensor
     * @param info Información del sensor
     */
    friend void lidarDisConnect(const DeviceInfo *info);

    /**
     * Cambia el la información de estado del sensor
     * @param info Información del sensor
     */
    friend void lidarStateChange(const DeviceInfo *info);

    /**
     * Callback para cambiar el estado del sensor
     * @param info Información del sensor
     * @param type Tipo de dispositivo
     */
    friend void onDeviceInfoChange(const DeviceInfo *info, DeviceEvent type);
};

#endif  // SCANNERLIDAR_CLASS_H