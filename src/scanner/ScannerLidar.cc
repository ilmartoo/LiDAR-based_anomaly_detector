/**
 * @file ScannerLidar.cc
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

#include "livox_sdk.h"

#include "scanner/ScannerLidar.hh"
#include "models/LidarPoint.hh"
#include "models/Timestamp.hh"

#include "logging/debug.hh"

#pragma GCC push_options
#pragma GCC optimize("O0")

// Obtiene los datos del punto enviado por el sensor
void getLidarData(uint8_t handle, LivoxEthPacket *data, uint32_t data_num, void *client_data) {
    if (ScannerLidar::getInstance()->lidar.device_state == kDeviceStateSampling) {
        // Obtenemos datos
        if (data && data->data_type == kExtendCartesian) {
            LivoxExtendRawPoint *p_data = (LivoxExtendRawPoint *)data->data;

            DEBUG_POINT_STDOUT("Point packet of type " << std::to_string(data->data_type) << " retrieved");

            for (uint32_t i = 0; ScannerLidar::getInstance()->scanning && i < data_num; ++i)
                if (ScannerLidar::getInstance()->callback) {
                    ScannerLidar::getInstance()->callback({Timestamp(data->timestamp), p_data[i].reflectivity, p_data[i].x, p_data[i].y, p_data[i].z});
                }
        }
        // Dato de tipo incorrecto
        else {
            DEBUG_POINT_STDOUT("Point packet of type  " << std::to_string(data->data_type) << " untreated");
        }
    }
}

// Callback para recepción de información del sensor
void onDeviceBroadcast(const BroadcastDeviceInfo *info) {
    // Solo aceptamos conexiones de un sensor LiDAR
    if (info == NULL || info->dev_type == kDeviceTypeHub) {
        DEBUG_STDERR("LiDAR scanner is not from a valid type");
        return;
    }

    DEBUG_STDOUT("Retrieved broadcast code " << std::string(ScannerLidar::getInstance()->lidar.info.broadcast_code));

    if (strncmp(info->broadcast_code, ScannerLidar::getInstance()->lidar.info.broadcast_code, kBroadcastCodeSize) != 0) {
        DEBUG_STDERR("Retrieved broadcast code do not match stored code");
        return;
    }

    // Conectamos LiDAR
    if (AddLidarToConnect(info->broadcast_code, &ScannerLidar::getInstance()->lidar.handle) == kStatusSuccess) {
        /** Set the point cloud data for a specific Livox LiDAR. */
        SetDataCallback(ScannerLidar::getInstance()->lidar.handle, getLidarData, NULL);

    } else {
        DEBUG_STDERR("Unable to connect with LiDAR scanner");
    }
}

// Recepción de mensajes de error
void onLidarErrorStatusCallback(livox_status status, uint8_t handle, ErrorMessage *message) {
    if (message != NULL) {
        DEBUG_STDERR("----- START ERROR DATA DUMP -----");
        DEBUG_STDERR("handle:            " << handle);
        DEBUG_STDERR("temp_status :      " << message->lidar_error_code.temp_status);
        DEBUG_STDERR("volt_status :      " << message->lidar_error_code.volt_status);
        DEBUG_STDERR("motor_status :     " << message->lidar_error_code.motor_status);
        DEBUG_STDERR("dirty_warn :       " << message->lidar_error_code.dirty_warn);
        DEBUG_STDERR("firmware_err :     " << message->lidar_error_code.firmware_err);
        DEBUG_STDERR("pps_status :       " << message->lidar_error_code.device_status);
        DEBUG_STDERR("fan_status :       " << message->lidar_error_code.fan_status);
        DEBUG_STDERR("self_heating :     " << message->lidar_error_code.self_heating);
        DEBUG_STDERR("ptp_status :       " << message->lidar_error_code.ptp_status);
        DEBUG_STDERR("time_sync_status : " << message->lidar_error_code.time_sync_status);
        DEBUG_STDERR("system_status :    " << message->lidar_error_code.system_status);
        DEBUG_STDERR("------ END ERROR DATA DUMP ------");
    }
}

// Conecta el sensor
void lidarConnect(const DeviceInfo *info) {
    if (ScannerLidar::getInstance()->lidar.device_state == kDeviceStateDisconnect) {
        ScannerLidar::getInstance()->lidar.device_state = kDeviceStateConnect;
        ScannerLidar::getInstance()->lidar.info = *info;
    }
}

// Desconecta el sensor
void lidarDisConnect(const DeviceInfo *info) { ScannerLidar::getInstance()->lidar.device_state = kDeviceStateDisconnect; }

// Cambia el la información de estado del sensor
void lidarStateChange(const DeviceInfo *info) { ScannerLidar::getInstance()->lidar.info = *info; }

// Callback para cambiar el estado del sensor
void onDeviceInfoChange(const DeviceInfo *info, DeviceEvent type) {
    if (info == NULL) {
        DEBUG_STDERR("LiDAR scanner info is NULL");
        return;
    }

    // Comprobamos que el handler sea el correcto
    if (ScannerLidar::getInstance()->lidar.handle != info->handle) {
        DEBUG_STDERR("Handler mismatch");
        return;
    }

    if (type == kEventConnect) {
        lidarConnect(info);

        DEBUG_STDOUT("LiDAR scanner " << std::string(info->broadcast_code) << " connected");
    }

    else if (type == kEventDisconnect) {
        lidarDisConnect(info);

        DEBUG_STDOUT("LiDAR scanner " << std::string(info->broadcast_code) << " disconnected");
    }

    else if (type == kEventStateChange) {
        lidarStateChange(info);

        DEBUG_STDOUT("Updated LiDAR scanner state [connection: " + std::to_string(ScannerLidar::getInstance()->lidar.device_state) +
                     "] [state: " + std::to_string(ScannerLidar::getInstance()->lidar.info.state) +
                     "] [feature: " + std::to_string(ScannerLidar::getInstance()->lidar.info.feature) + "]");
    }

    if (ScannerLidar::getInstance()->lidar.device_state == kDeviceStateConnect) {
        SetErrorMessageCallback(ScannerLidar::getInstance()->lidar.handle, onLidarErrorStatusCallback);
    }
}

// Se ejecuta con el comienzo del escaneo de puntos
void onSampleCallback(livox_status status, uint8_t handle, uint8_t response, void *data) {
    DEBUG_STDOUT("Point scanning start [status: " << std::to_string(status) << "] [response: " << std::to_string(response) << "]");

    // Inicio correcto
    if (status == kStatusSuccess) {
        ScannerLidar::getInstance()->lidar.device_state = kDeviceStateSampling;

    }
    // Fallo
    else {
        DEBUG_STDERR("Error while starting point scanning");
        ScannerLidar::getInstance()->lidar.device_state = kDeviceStateDisconnect;

        ScannerLidar::getInstance()->scanning = false;
    }
}

// Se ejecuta con la finalización del escaneo de puntos
void onStopSampleCallback(livox_status status, uint8_t handle, uint8_t response, void *data) {
    DEBUG_STDOUT("Ended point scanning [status: " << std::to_string(status) << "] [response: " << std::to_string(response) << "]");

    // Pausa correcta
    if (status == kStatusSuccess) {
        ScannerLidar::getInstance()->lidar.device_state = kDeviceStateConnect;

    }
    // Fallo
    else {
        DEBUG_STDERR("Error while ending point scanning");
        ScannerLidar::getInstance()->lidar.device_state = kDeviceStateDisconnect;
    }

    ScannerLidar::getInstance()->scanning = false;
}

// Se ejecuta al realizar la peticion de desestimiento de los datos IMU
void onStopIMURecepcionCallback(livox_status status, uint8_t handle, uint8_t response, void *data) {
    // Renegado correctamente
    if (status == kStatusSuccess) {
        DEBUG_STDOUT("Stopped IMU point retrieval");
    }
    // Fallo
    else {
        DEBUG_STDERR("Error while stopping IMU point retrieval");
    }
}

// Callback para el establecimiento de las coordenadas cartesianas
void onSetCartesianCoordinateCallback(livox_status status, uint8_t handle, uint8_t response, void *data) {
    // Establecido correctamente
    if (status == kStatusSuccess) {
        DEBUG_STDOUT("Set cartesian coordinates as default point type");
    }
    // Fallo
    else {
        DEBUG_STDERR("Error while setting up cartesian coordinates as default");
    }
}

bool ScannerLidar::init() {
    DEBUG_STDOUT("Initializing LIVOX SDK");

    // Inicialización de Livox SDK
    if (!Init()) {
        DEBUG_STDERR("Error while initializing LIVOX SDK");
        return false;
    }
    // Desactivamos logs
    DisableConsoleLogger();

    // Callback para recibir mensajes de broadcast del Livox LiDAR
    SetBroadcastCallback(onDeviceBroadcast);

    // Callback para cuando se recibe un cambio de estado (conexión/desconexión/cambio de estado)
    SetDeviceStateUpdateCallback(onDeviceInfoChange);

    if (!Start()) {
        DEBUG_STDERR("Error while starting LIVOX SDK");
        return false;
    }

    DEBUG_STDOUT("Initialized LIVOX SDK");

    return true;
}

ScanCode ScannerLidar::scan() {
    if (!scanning) {
        DEBUG_STDOUT("Starting point scanning");

        scanning = true;

        /* Esperamos a que el sensor esté listo */
        while (ScannerLidar::getInstance()->lidar.info.state != kLidarStateNormal) {}

        DEBUG_STDOUT("LiDAR scanner ready");

        /* Renegamos de la obtención de datos IMU */
        LidarSetImuPushFrequency(ScannerLidar::getInstance()->lidar.handle, kImuFreq0Hz, onStopIMURecepcionCallback, NULL);

        /* Establecemos las cordenadas cartesianas por defecto */
        SetCartesianCoordinate(ScannerLidar::getInstance()->lidar.handle, onSetCartesianCoordinateCallback, NULL);

        /* Comenzamos el muestreo */
        if (kStatusSuccess == LidarStartSampling(ScannerLidar::getInstance()->lidar.handle, onSampleCallback, NULL)) {
            while (this->isScanning()) {}  // Esperamos a que finalize de escanear

            return ScanCode::kScanOk;
        } else {
            DEBUG_STDERR("Scanner failed to start scanning");
            return ScanCode::kScanError;
            
            scanning = false;
        }

    } else {
        DEBUG_STDERR("Scanner already in use");
        return ScanCode::kScanError;
    }
}

void ScannerLidar::pause() {
    LidarStopSampling(lidar.handle, onStopSampleCallback, NULL);

    ScannerLidar::getInstance()->lidar.device_state = kDeviceStateConnect;
}

bool ScannerLidar::setCallback(const std::function<void(const LidarPoint &p)> func) {
    DEBUG_STDOUT("Setting up callback");

    callback = func;
    return ((bool)callback);
}

void ScannerLidar::stop() {
    DEBUG_STDOUT("Closing LIVOX SDK");

    LidarStopSampling(lidar.handle, onStopSampleCallback, NULL);
    Uninit();

    DEBUG_STDOUT("LIVOX SDK closed");
}

#pragma GCC pop_options
