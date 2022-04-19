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
#include <chrono>

#include "livox_sdk/livox_sdk.h"

#include "scanner/ScannerLidar.hh"
#include "models/Point.hh"
#include "models/Timestamp.hh"

#include "logging/debug.hh"
#include "logging/logging.hh"

/* Guarrería muy fea que no se como solucionar, pero mientras que esté así funciona que no veas */
ScannerLidar *_sl;

// Obtiene los datos del punto enviado por el sensor
void getLidarData(uint8_t handle, LivoxEthPacket *data, uint32_t data_num, void *client_data) {
    if (_sl->lidar.device_state == kDeviceStateSampling) {
        // Obtenemos datos
        if (data && data->data_type == kExtendCartesian) {
            LivoxExtendRawPoint *p_data = (LivoxExtendRawPoint *)data->data;

            DEBUG_POINT_STDOUT("Obtenido paquete de datos de tipo [" + std::to_string(data->data_type) + "]");

            for (uint32_t i = 0; !_sl->exit && i < data_num; ++i)
                if (_sl->callback) {
                    Point p = {Timestamp(data->timestamp), p_data[i].reflectivity, p_data[i].x, p_data[i].y, p_data[i].z};
                    _sl->callback(p);
                }
        }
        // Dato de tipo incorrecto
        else {
            DEBUG_POINT_STDOUT("Paquete de datos de tipo [" + std::to_string(data->data_type) + "] no tratado");
        }
    }
}

// Callback para recepción de información del sensor
void onDeviceBroadcast(const BroadcastDeviceInfo *info) {
    // Solo aceptamos conexiones de un sensor LiDAR
    if (info == NULL || info->dev_type == kDeviceTypeHub) {
        LOG_ERROR("El sensor no es de un tipo válido.");
        return;
    }

    DEBUG_STDOUT("Recibido código de broadcast " + std::string(_sl->lidar.info.broadcast_code));

    if (strncmp(info->broadcast_code, _sl->lidar.info.broadcast_code, kBroadcastCodeSize) != 0) {
        LOG_ERROR("El código de retransmision no coincide con el almacenado.");
        return;
    }

    // Conectamos LiDAR
    if (AddLidarToConnect(info->broadcast_code, &_sl->lidar.handle) == kStatusSuccess) {
        /** Set the point cloud data for a specific Livox LiDAR. */
        SetDataCallback(_sl->lidar.handle, getLidarData, NULL);

    } else {
        LOG_ERROR("No se ha podido conectar el sensor.");
    }
}

// Recepción de mensajes de error
void onLidarErrorStatusCallback(livox_status status, uint8_t handle, ErrorMessage *message) {
    if (message != NULL) {
        LOG_ERROR("----- START ERROR DATA DUMP -----");
        LOG_ERROR("handle:            " << handle);
        LOG_ERROR("temp_status :      " << message->lidar_error_code.temp_status);
        LOG_ERROR("volt_status :      " << message->lidar_error_code.volt_status);
        LOG_ERROR("motor_status :     " << message->lidar_error_code.motor_status);
        LOG_ERROR("dirty_warn :       " << message->lidar_error_code.dirty_warn);
        LOG_ERROR("firmware_err :     " << message->lidar_error_code.firmware_err);
        LOG_ERROR("pps_status :       " << message->lidar_error_code.device_status);
        LOG_ERROR("fan_status :       " << message->lidar_error_code.fan_status);
        LOG_ERROR("self_heating :     " << message->lidar_error_code.self_heating);
        LOG_ERROR("ptp_status :       " << message->lidar_error_code.ptp_status);
        LOG_ERROR("time_sync_status : " << message->lidar_error_code.time_sync_status);
        LOG_ERROR("system_status :    " << message->lidar_error_code.system_status);
        LOG_ERROR("------ END ERROR DATA DUMP ------");
    }
}

// Conecta el sensor
void lidarConnect(const DeviceInfo *info) {
    if (_sl->lidar.device_state == kDeviceStateDisconnect) {
        _sl->lidar.device_state = kDeviceStateConnect;
        _sl->lidar.info = *info;
    }
}

// Desconecta el sensor
void lidarDisConnect(const DeviceInfo *info) { _sl->lidar.device_state = kDeviceStateDisconnect; }

// Cambia el la información de estado del sensor
void lidarStateChange(const DeviceInfo *info) { _sl->lidar.info = *info; }

// Callback para cambiar el estado del sensor
void onDeviceInfoChange(const DeviceInfo *info, DeviceEvent type) {
    if (info == NULL) {
        LOG_ERROR("La información del sensor es nula");
        return;
    }

    // Comprobamos que el handler sea el correcto
    if (_sl->lidar.handle != info->handle) {
        LOG_ERROR("El handler no se corresponde con el del sensor");
        return;
    }

    if (type == kEventConnect) {
        lidarConnect(info);

        DEBUG_STDOUT("Lidar " + std::string(info->broadcast_code) + " conectado.");
    }

    else if (type == kEventDisconnect) {
        lidarDisConnect(info);

        DEBUG_STDOUT("Lidar " + std::string(info->broadcast_code) + " desconectado.");
    }

    else if (type == kEventStateChange) {
        lidarStateChange(info);

        DEBUG_STDOUT("Modificado estado del LiDAR " + std::string(info->broadcast_code) + ". Conexión: [" +
                     std::to_string(_sl->lidar.device_state) + "], Estado: [" + std::to_string(_sl->lidar.info.state) +
                     "], Funcionalidad: [" + std::to_string(_sl->lidar.info.feature) + "]");
    }

    if (_sl->lidar.device_state == kDeviceStateConnect) {
        SetErrorMessageCallback(_sl->lidar.handle, onLidarErrorStatusCallback);
    }
}

// Se ejecuta con el comienzo del escaneo de puntos
void onSampleCallback(livox_status status, uint8_t handle, uint8_t response, void *data) {
    DEBUG_STDOUT("Comienzo del escaneo. Status: [" + std::to_string(status) + "], Response: [" + std::to_string(response) + "]");

    // Inicio correcto
    if (status == kStatusSuccess) {
        _sl->lidar.device_state = kDeviceStateConnect;
        _sl->lidar.device_state = kDeviceStateSampling;

    }
    // Fallo
    else {
        LOG_ERROR("Fallo al intentar iniciar el escaneo de puntos.");
        _sl->lidar.device_state = kDeviceStateDisconnect;
    }
}

// Se ejecuta con la finalización del escaneo de puntos
void onStopSampleCallback(livox_status status, uint8_t handle, uint8_t response, void *data) {
    DEBUG_STDOUT("Finalización del escaneo. Status: [" + std::to_string(status) + "], Response: [" + std::to_string(response) + "]");

    _sl->lidar.device_state = kDeviceStateDisconnect;
}

// Se ejecuta al realizar la peticion de desestimiento de los datos IMU
void onStopIMURecepcionCallback(livox_status status, uint8_t handle, uint8_t response, void *data) {
    // Renegado correctamente
    if (status == kStatusSuccess) {
        DEBUG_STDOUT("Renegada la recepción de datos IMU.");
    }
    // Fallo
    else {
        LOG_ERROR("Fallo al renegar de la emisión de datos IMU.");
    }
}

// Inicialización del escaner
bool ScannerLidar::init() {
    LOG_INFO("Inicializando Livox SDK.");

    // Callback para recibir mensajes de broadcast del Livox LiDAR
    SetBroadcastCallback(onDeviceBroadcast);

    // Callback para cuando se recibe un cambio de estado (conexión/desconexión/cambio de estado)
    SetDeviceStateUpdateCallback(onDeviceInfoChange);

    // Inicialización de Livox SDK
    if (!Init()) {
        LOG_ERROR("Fallo al inicializar Livox SDK.");
        return false;
    }

    _sl = this;  // Guardamos puntero a la clase actual

    DEBUG_STDOUT("Livox SDK inicializado correctamente.");

    return true;
}

// Comienza la obtención de puntos
bool ScannerLidar::start() {
    LOG_INFO("Iniciando el escaneo de puntos.");

    if (!Start()) {
        LOG_ERROR("Fallo al iniciar la rutina de escaneo de puntos.");
        return false;
    }

    exit = false;

    /* Esperamos a que el sensor esté listo */
    while (_sl->lidar.info.state != kLidarStateNormal) {}  // Busy wait -- Posible mejora

    LOG_INFO("Sensor detectado.");

    /* Renegamos de la obtención de datos IMU */
    LidarSetImuPushFrequency(_sl->lidar.handle, kImuFreq0Hz, onStopIMURecepcionCallback, NULL);

    /* Comenzamos el muestreo */
    if (kStatusSuccess == LidarStartSampling(_sl->lidar.handle, onSampleCallback, NULL)) {
        // lidar.device_state = kDeviceStateSampling;

        return true;

    } else {
        LOG_ERROR("El sensor no está listo para iniciar el escaneo de puntos.");
        return false;
    }
}

// Establece la función especificada como función de callback a la que se llamará cada vez que se escanee un nuevo punto
bool ScannerLidar::setCallback(const std::function<void(const Point &p)> func) {
    DEBUG_STDOUT("Estableciendo el callback.");

    callback = func;
    return ((bool)callback);
}

// Finaliza el escaner
void ScannerLidar::stop() {
    DEBUG_STDOUT("Finalizando Livox SDK.");

    exit = true;

    LidarStopSampling(lidar.handle, onStopSampleCallback, NULL);
    Uninit();

    _sl = nullptr;  // Quitamos referencia

    LOG_INFO("Finalizado Livox SDK.");
}
