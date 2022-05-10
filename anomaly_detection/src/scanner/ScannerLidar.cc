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
#include <mutex>
#include <condition_variable>

#include "livox_sdk.h"

#include "scanner/ScannerLidar.hh"
#include "models/Point.hh"
#include "models/Timestamp.hh"
#include "models/CLICommand.hh"

#include "logging/debug.hh"

std::mutex mtx;                          // Mutex de bloqueo
std::unique_lock<std::mutex> lock(mtx);  // Candado de sincronización
std::condition_variable cv;              // Gestor de sincronización

// Obtiene los datos del punto enviado por el sensor
void getLidarData(uint8_t handle, LivoxEthPacket *data, uint32_t data_num, void *client_data) {
    if (ScannerLidar::getInstance()->lidar.device_state == kDeviceStateSampling) {
        // Obtenemos datos
        if (data && data->data_type == kExtendCartesian) {
            LivoxExtendRawPoint *p_data = (LivoxExtendRawPoint *)data->data;

            DEBUG_POINT_STDOUT("Obtenido paquete de datos de tipo [" << std::to_string(data->data_type) << "]");

            for (uint32_t i = 0; !ScannerLidar::getInstance()->scanning && i < data_num; ++i)
                if (ScannerLidar::getInstance()->callback) {
                    Point p = {Timestamp(data->timestamp), p_data[i].reflectivity, p_data[i].x, p_data[i].y, p_data[i].z};
                    ScannerLidar::getInstance()->callback(p);
                }
        }
        // Dato de tipo incorrecto
        else {
            DEBUG_POINT_STDOUT("Paquete de datos de tipo [" << std::to_string(data->data_type) << "] no tratado");
        }
    }
}

// Callback para recepción de información del sensor
void onDeviceBroadcast(const BroadcastDeviceInfo *info) {
    // Solo aceptamos conexiones de un sensor LiDAR
    if (info == NULL || info->dev_type == kDeviceTypeHub) {
        CLI_STDERR("El sensor no es de un tipo válido.");
        return;
    }

    DEBUG_STDOUT("Recibido código de broadcast " << std::string(ScannerLidar::getInstance()->lidar.info.broadcast_code));

    if (strncmp(info->broadcast_code, ScannerLidar::getInstance()->lidar.info.broadcast_code, kBroadcastCodeSize) != 0) {
        CLI_STDERR("El código de retransmision no coincide con el almacenado.");
        return;
    }

    // Conectamos LiDAR
    if (AddLidarToConnect(info->broadcast_code, &ScannerLidar::getInstance()->lidar.handle) == kStatusSuccess) {
        /** Set the point cloud data for a specific Livox LiDAR. */
        SetDataCallback(ScannerLidar::getInstance()->lidar.handle, getLidarData, NULL);

    } else {
        CLI_STDERR("No se ha podido conectar el sensor.");
    }
}

// Recepción de mensajes de error
void onLidarErrorStatusCallback(livox_status status, uint8_t handle, ErrorMessage *message) {
    if (message != NULL) {
        CLI_STDERR("----- START ERROR DATA DUMP -----");
        CLI_STDERR("handle:            " << handle);
        CLI_STDERR("temp_status :      " << message->lidar_error_code.temp_status);
        CLI_STDERR("volt_status :      " << message->lidar_error_code.volt_status);
        CLI_STDERR("motor_status :     " << message->lidar_error_code.motor_status);
        CLI_STDERR("dirty_warn :       " << message->lidar_error_code.dirty_warn);
        CLI_STDERR("firmware_err :     " << message->lidar_error_code.firmware_err);
        CLI_STDERR("pps_status :       " << message->lidar_error_code.device_status);
        CLI_STDERR("fan_status :       " << message->lidar_error_code.fan_status);
        CLI_STDERR("self_heating :     " << message->lidar_error_code.self_heating);
        CLI_STDERR("ptp_status :       " << message->lidar_error_code.ptp_status);
        CLI_STDERR("time_sync_status : " << message->lidar_error_code.time_sync_status);
        CLI_STDERR("system_status :    " << message->lidar_error_code.system_status);
        CLI_STDERR("------ END ERROR DATA DUMP ------");
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
        CLI_STDERR("La información del sensor es nula");
        return;
    }

    // Comprobamos que el handler sea el correcto
    if (ScannerLidar::getInstance()->lidar.handle != info->handle) {
        CLI_STDERR("El handler no se corresponde con el del sensor");
        return;
    }

    if (type == kEventConnect) {
        lidarConnect(info);

        DEBUG_STDOUT("Lidar " << std::string(info->broadcast_code) << " conectado.");
    }

    else if (type == kEventDisconnect) {
        lidarDisConnect(info);

        DEBUG_STDOUT("Lidar " << std::string(info->broadcast_code) << " desconectado.");
    }

    else if (type == kEventStateChange) {
        lidarStateChange(info);

        DEBUG_STDOUT("Modificado estado del LiDAR " << std::string(info->broadcast_code) + ". Conexión: [" +
                                                           std::to_string(ScannerLidar::getInstance()->lidar.device_state) + "], Estado: [" + std::to_string(ScannerLidar::getInstance()->lidar.info.state) +
                                                           "], Funcionalidad: [" + std::to_string(ScannerLidar::getInstance()->lidar.info.feature) + "]");
    }

    if (ScannerLidar::getInstance()->lidar.device_state == kDeviceStateConnect) {
        SetErrorMessageCallback(ScannerLidar::getInstance()->lidar.handle, onLidarErrorStatusCallback);
    }
}

// Se ejecuta con el comienzo del escaneo de puntos
void onSampleCallback(livox_status status, uint8_t handle, uint8_t response, void *data) {
    DEBUG_STDOUT("Comienzo del escaneo. Status: [" << std::to_string(status) << "], Response: [" << std::to_string(response) << "]");

    // Bloqueamos el mutex
    lock.lock();

    // Inicio correcto
    if (status == kStatusSuccess) {
        ScannerLidar::getInstance()->lidar.device_state = kDeviceStateSampling;

    }
    // Fallo
    else {
        CLI_STDERR("Fallo al intentar iniciar el escaneo de puntos.");
        ScannerLidar::getInstance()->lidar.device_state = kDeviceStateDisconnect;

        // Debloqueamos mutex por fallo
        lock.unlock();
        cv.notify_all();
    }
}

// Se ejecuta con la finalización del escaneo de puntos
void onStopSampleCallback(livox_status status, uint8_t handle, uint8_t response, void *data) {
    DEBUG_STDOUT("Finalización del escaneo. Status: [" << std::to_string(status) << "], Response: [" << std::to_string(response) << "]");

    // Pausa correcta
    if (status == kStatusSuccess) {
        ScannerLidar::getInstance()->lidar.device_state = kDeviceStateConnect;

    }
    // Fallo
    else {
        CLI_STDERR("Fallo al finalizar el escaneo de puntos.");
        ScannerLidar::getInstance()->lidar.device_state = kDeviceStateDisconnect;
    }

    ScannerLidar::getInstance()->scanning = false;

    // Desbloqueamos el mutex
    lock.unlock();
    cv.notify_all();
}

// Se ejecuta al realizar la peticion de desestimiento de los datos IMU
void onStopIMURecepcionCallback(livox_status status, uint8_t handle, uint8_t response, void *data) {
    // Renegado correctamente
    if (status == kStatusSuccess) {
        DEBUG_STDOUT("Renegada la recepción de datos IMU.");
    }
    // Fallo
    else {
        CLI_STDERR("Fallo al renegar de la emisión de datos IMU.");
    }
}

// Callback para el establecimiento de las coordenadas cartesianas
void onSetCartesianCoordinateCallback(livox_status status, uint8_t handle, uint8_t response, void *data) {
    // Establecido correctamente
    if (status == kStatusSuccess) {
        DEBUG_STDOUT("Establecidas coordenadas cartesianas.");
    }
    // Fallo
    else {
        CLI_STDERR("Fallo al establecer las coordenadas cartesianas.");
    }
}

bool ScannerLidar::init() {
    DEBUG_STDOUT("Inicializando Livox SDK.");

    // Inicialización de Livox SDK
    if (!Init()) {
        CLI_STDERR("Fallo al inicializar Livox SDK.");
        return false;
    }
    // Desactivamos logs
    DisableConsoleLogger();

    // Callback para recibir mensajes de broadcast del Livox LiDAR
    SetBroadcastCallback(onDeviceBroadcast);

    // Callback para cuando se recibe un cambio de estado (conexión/desconexión/cambio de estado)
    SetDeviceStateUpdateCallback(onDeviceInfoChange);

    if (!Start()) {
        CLI_STDERR("Fallo al iniciar Livox SDK.");
        return false;
    }

    DEBUG_STDOUT("Livox SDK inicializado correctamente.");

    return true;
}

ScanCode ScannerLidar::scan() {
    if (!scanning) {
        DEBUG_STDOUT("Iniciando el escaneo de puntos.");

        scanning = true;

        /* Esperamos a que el sensor esté listo */
        while (ScannerLidar::getInstance()->lidar.info.state != kLidarStateNormal) {}

        DEBUG_STDOUT("Sensor detectado.");

        /* Renegamos de la obtención de datos IMU */
        LidarSetImuPushFrequency(ScannerLidar::getInstance()->lidar.handle, kImuFreq0Hz, onStopIMURecepcionCallback, NULL);

        /* Establecemos las cordenadas cartesianas por defecto */
        SetCartesianCoordinate(ScannerLidar::getInstance()->lidar.handle, onSetCartesianCoordinateCallback, NULL);

        /* Comenzamos el muestreo */
        if (kStatusSuccess == LidarStartSampling(ScannerLidar::getInstance()->lidar.handle, onSampleCallback, NULL)) {
            cv.wait(lock, [this] { return !this->isScanning(); });  // Esperamos a que finalize de escanear

            return ScanCode::kScanOk;
        } else {
            CLI_STDERR("El sensor no está listo para iniciar el escaneo de puntos.");
            return ScanCode::kScanError;
        }

    } else {
        CLI_STDERR("El sensor ya está escaneando.");
        return ScanCode::kScanError;
    }
}

void ScannerLidar::pause() {
    LidarStopSampling(lidar.handle, onStopSampleCallback, NULL);

    ScannerLidar::getInstance()->lidar.device_state = kDeviceStateConnect;
}

bool ScannerLidar::setCallback(const std::function<void(const Point &p)> func) {
    DEBUG_STDOUT("Estableciendo el callback.");

    callback = func;
    return ((bool)callback);
}

void ScannerLidar::stop() {
    DEBUG_STDOUT("Finalizando Livox SDK.");

    LidarStopSampling(lidar.handle, onStopSampleCallback, NULL);
    Uninit();

    DEBUG_STDOUT("Finalizado Livox SDK.");
}
