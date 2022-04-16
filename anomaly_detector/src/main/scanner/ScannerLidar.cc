/**
 * @file ScannerLidar.cpp
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 23/03/2022
 *
 * Implementación del objeto ScannerLidar
 *
 */

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <string>
#include <functional>

#include "livox_sdk/livox_sdk.h"

#include "scanner/ScannerLidar.hh"
#include "models/Point.hh"
#include "models/Timestamp.hh"

#include "debug.hh"

// Obtiene los datos del punto enviado por el sensor
void ScannerLidar::getLidarData(uint8_t handle, LivoxEthPacket *data, uint32_t data_num, void *client_data) {
    // Obtenemos datos
    if (data && data->data_type == kExtendCartesian) {
        LivoxExtendRawPoint *p_data = (LivoxExtendRawPoint *)data->data;

        if (callback) {
            Point p = {Timestamp(data->timestamp), p_data->reflectivity, p_data->x, p_data->y, p_data->z};
            callback(p);
        }
    }
}

// Callback para recepción de información del sensor
void ScannerLidar::onDeviceBroadcast(const BroadcastDeviceInfo *info) {
    // Solo aceptamos conexiones de un sensor LiDAR
    if (info == NULL || info->dev_type == kDeviceTypeHub) {
        std::cerr << "El sensor no es de un tipo válido." << std::endl;
        return;
    }

    DEBUG_STDOUT("Recibido código de broadcast " + std::string(info->broadcast_code));

    if (info->broadcast_code != lidar.info.broadcast_code) {
        std::cerr << "El código de retransmision no coincide con el almacenado." << std::endl;
        return;
    }

    // Conectamos LiDAR
    if (AddLidarToConnect(info->broadcast_code, &lidar.handle) == kStatusSuccess) {
        /** Set the point cloud data for a specific Livox LiDAR. */
        SetDataCallback(lidar.handle, (DataCallback)&ScannerLidar::getLidarData, NULL);

    } else {
        std::cerr << "No se ha podido conectar el sensor." << std::endl;
    }
}

// Recepción de mensajes de error
void ScannerLidar::onLidarErrorStatusCallback(livox_status status, uint8_t handle, ErrorMessage *message) {
    if (message != NULL) {
        std::cerr << "----- START ERROR DATA DUMP -----" << std::endl;
        std::cerr << "handle:            " << handle << std::endl;
        std::cerr << "temp_status :      " << message->lidar_error_code.temp_status << std::endl;
        std::cerr << "volt_status :      " << message->lidar_error_code.volt_status << std::endl;
        std::cerr << "motor_status :     " << message->lidar_error_code.motor_status << std::endl;
        std::cerr << "dirty_warn :       " << message->lidar_error_code.dirty_warn << std::endl;
        std::cerr << "firmware_err :     " << message->lidar_error_code.firmware_err << std::endl;
        std::cerr << "pps_status :       " << message->lidar_error_code.device_status << std::endl;
        std::cerr << "fan_status :       " << message->lidar_error_code.fan_status << std::endl;
        std::cerr << "self_heating :     " << message->lidar_error_code.self_heating << std::endl;
        std::cerr << "ptp_status :       " << message->lidar_error_code.ptp_status << std::endl;
        std::cerr << "time_sync_status : " << message->lidar_error_code.time_sync_status << std::endl;
        std::cerr << "system_status :    " << message->lidar_error_code.system_status << std::endl;
        std::cerr << "------ END ERROR DATA DUMP ------" << std::endl;
    }
}

// Conecta el sensor
void ScannerLidar::lidarConnect(const DeviceInfo *info) {
    if (lidar.device_state == kDeviceStateDisconnect) {
        lidar.device_state = kDeviceStateConnect;
        lidar.info = *info;
    }
}

// Desconecta el sensor
void ScannerLidar::lidarDisConnect(const DeviceInfo *info) { lidar.device_state = kDeviceStateDisconnect; }

// Cambia el la información de estado del sensor
void ScannerLidar::lidarStateChange(const DeviceInfo *info) { lidar.info = *info; }

// Callback para cambiar el estado del sensor
void ScannerLidar::onDeviceInfoChange(const DeviceInfo *info, DeviceEvent type) {
    if (info == NULL) {
        std::cerr << "La información del sensor es nula" << std::endl;
        return;
    }

    // Comprobamos que el handler sea el correcto
    if (this->lidar.handle != info->handle) {
        std::cerr << "El handler no se corresponde con el del sensor" << std::endl;
        return;
    }

    if (type == kEventConnect) {
        lidarConnect(info);

        DEBUG_STDOUT("Lidar " + std::string(info->broadcast_code) + " conectado");
    }

    else if (type == kEventDisconnect) {
        lidarDisConnect(info);

        DEBUG_STDOUT("Lidar " + std::string(info->broadcast_code) + " desconectado");
    }

    else if (type == kEventStateChange) {
        lidarStateChange(info);

        DEBUG_STDOUT("Modificado estado del lidar " + std::string(info->broadcast_code));
    }

    if (lidar.device_state == kDeviceStateConnect) {
        DEBUG_STDOUT("Estado del sensor: " + std::to_string(lidar.info.state));
        DEBUG_STDOUT("Funcionalidad del sensor: " + std::to_string(lidar.info.feature));

        SetErrorMessageCallback(lidar.handle, (ErrorMessageCallback)&ScannerLidar::onLidarErrorStatusCallback);
    }
}

// Se ejecuta con el comienzo del escaneo de puntos
void ScannerLidar::onSampleCallback(livox_status status, uint8_t handle, uint8_t response, void *data) {
    DEBUG_STDOUT("Comienzo del escaneo: " + std::to_string(status) + "statue" + std::to_string(response) + "response");

    if (status == kStatusSuccess) {
        if (response != 0) {
            lidar.device_state = kDeviceStateConnect;
        }
    } else if (status == kStatusTimeout) {
        std::cerr << "Timeout del sensor" << std::endl;
        lidar.device_state = kDeviceStateDisconnect;
    }
}

// Se ejecuta con la finalización del escaneo de puntos
void ScannerLidar::onStopSampleCallback(livox_status status, uint8_t handle, uint8_t response, void *data) {
    DEBUG_STDOUT("Finalización del escaneo: " + std::to_string(status) + "statue" + std::to_string(response) + "response");

    lidar.device_state = kDeviceStateDisconnect;
}

// Inicialización del escaner
bool ScannerLidar::init() {
    std::cout << "Inicializando Livox SDK." << std::endl;

    // Callback para recibir mensajes de broadcast del Livox LiDAR
    SetBroadcastCallback((DeviceBroadcastCallback)&ScannerLidar::onDeviceBroadcast);

    // Callback para cuando se recibe un cambio de estado (conexión/desconexión/cambio de estado)
    SetDeviceStateUpdateCallback((DeviceStateUpdateCallback)&ScannerLidar::onDeviceInfoChange);

    // Inicialización de Livox SDK
    if (!Init()) {
        std::cerr << "Fallo al inicializar Livox SDK." << std::endl;
        return false;
    }

    DEBUG_STDOUT("Livox SDK inicializado correctamente.");

    return true;
}

// Comienza la obtención de puntos
bool ScannerLidar::start() {
    while (lidar.info.state != kLidarStateNormal) { /* Esperamos a que el sensor esté listo */
    }

    if (kStatusSuccess == LidarStartSampling(lidar.handle, (CommonCommandCallback)&ScannerLidar::onSampleCallback, NULL)) {
        lidar.device_state = kDeviceStateSampling;

        return true;

    } else {
        std::cerr << "El sensor no está listo para iniciar el escaneo de puntos" << std::endl;
        return false;
    }
}

// Establece la función especificada como función de callback a la que se llamará cada vez que
// se escanee un nuevo punto
bool ScannerLidar::setCallback(const std::function<void(const Point &p)> func) {
    DEBUG_STDOUT("Estableciendo el callback.");

    callback = func;
    return ((bool)callback);
}

// Finaliza el escaner
void ScannerLidar::stop() {
    DEBUG_STDOUT("Finalizando Livox SDK.");

    LidarStopSampling(lidar.handle, (CommonCommandCallback)&ScannerLidar::onStopSampleCallback, NULL);
    Uninit();

    std::cout << "Finalizado Livox SDK." << std::endl;
}