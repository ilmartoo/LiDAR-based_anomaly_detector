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

#include "livox_sdk.h"

#include "debug_lbad.hh"
#include "scanner/ScannerLidar.hh"
#include "models/Point.hh"

// Obtiene los datos del punto enviado por el sensor
void ScannerLidar::getLidarData(uint8_t handle, LivoxEthPacket *data, uint32_t data_num, void *client_data) {
    // Obtenemos datos
    if (data && data->data_type == kExtendCartesian) {
        LivoxExtendRawPoint *p_data = (LivoxExtendRawPoint *)data->data;

        if (callback) {
            callback(Point((uint64_t)data->timestamp, p_data->reflectivity, p_data->x, p_data->y, p_data->z));
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

    printDebug("Recibido código de broadcast " + std::string(info->broadcast_code));  // debug

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

// Se ejecuta con el comienzo del escaneo de puntos
void ScannerLidar::onSampleCallback(livox_status status, uint8_t handle, uint8_t response, void *data) {
    printDebug("Comienzo del escaneo: " + std::to_string(status) + "statue" + std::to_string(response) +
               "response");  // debug

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
    printDebug("Finalización del escaneo: " + std::to_string(status) + "statue" + std::to_string(response) +
               "response");  // debug

    lidar.device_state = kDeviceStateDisconnect;
}

// Conecta el sensor
void ScannerLidar::lidarConnect(const DeviceInfo *info) {
    if (lidar.device_state == kDeviceStateDisconnect) {
        lidar.device_state = kDeviceStateConnect;
        lidar.info = *info;
    }
}

// Desconecta el sensor
void ScannerLidar::lidarDisConnect(const DeviceInfo *info) {
    lidar.device_state = kDeviceStateDisconnect;
}

// Cambia el la información de estado del sensor
void ScannerLidar::lidarStateChange(const DeviceInfo *info) {
    lidar.info = *info;
}

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

        printDebug("Lidar " + std::string(info->broadcast_code) + " conectado");  // debug
    }

    else if (type == kEventDisconnect) {
        lidarDisConnect(info);

        printDebug("Lidar " + std::string(info->broadcast_code) + " desconectado");  // debug
    }

    else if (type == kEventStateChange) {
        lidarStateChange(info);

        printDebug("Modificado estado del lidar " + std::string(info->broadcast_code));  // debug
    }

    if (lidar.device_state == kDeviceStateConnect) {
        printDebug("Estado del sensor: " + std::to_string(lidar.info.state));           // debug
        printDebug("Funcionalidad del sensor: " + std::to_string(lidar.info.feature));  // debug

        SetErrorMessageCallback(lidar.handle, (ErrorMessageCallback)&ScannerLidar::onLidarErrorStatusCallback);
    }
}

// Inicialización del escaner
bool ScannerLidar::initScanner() {
    printDebug("Inicializando Livox SDK.");  // debug

    /** Set the callback function receiving broadcast message from Livox LiDAR. */
    SetBroadcastCallback((DeviceBroadcastCallback)&ScannerLidar::onDeviceBroadcast);

    /** Set the callback function called when device state change,
     * which means connection/disconnection and changing of LiDAR state.
     */
    SetDeviceStateUpdateCallback((DeviceStateUpdateCallback)&ScannerLidar::onDeviceInfoChange);

    // Inicialización de Livox SDK
    if (!Init()) {
        std::cerr << "Fallo al inicializar Livox SDK." << std::endl;
        return false;
    }

    printDebug("Livox SDK inicializado correctamente.");  // debug

    return true;
}

// Comienza la obtención de puntos
bool ScannerLidar::startScanner() {
    if (lidar.info.state == kLidarStateNormal &&
        kStatusSuccess ==
            LidarStartSampling(lidar.handle, (CommonCommandCallback)&ScannerLidar::onSampleCallback, NULL)) {
        lidar.device_state = kDeviceStateSampling;

        return true;
    }

    else {
        std::cerr << "El sensor no está listo para iniciar el escaneo de puntos" << std::endl;
        return false;
    }
}

// Establece la función especificada como función de callback a la que se llamará cada vez que
// se escanee un nuevo punto
bool ScannerLidar::setCallback(const std::function<void(Point)> func) {
    printDebug("Estableciendo el callback.");  // debug

    callback = func;
    return ((bool)callback);
}

// Finaliza el escaner
void ScannerLidar::closeScanner() {
    printDebug("Finalizando Livox SDK.");  // debug

    LidarStopSampling(lidar.handle, (CommonCommandCallback)&ScannerLidar::onStopSampleCallback, NULL);
    Uninit();

    printDebug("Finalizado Livox SDK.");  // debug
}