/**
 * @file ScannerLVX.cpp
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 03/04/2022
 *
 * Implementación del objeto ScannerLVX
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <thread>

#include "read_lvx/lvx_file.h"
#include "read_lvx/lds.h"

#include "scanner/ScannerLVX.hh"
#include "models/Point.hh"
#include "models/Timestamp.hh"

#include "logging/debug.hh"
#include "logging/logging.hh"

// Inicialización del escaner
bool ScannerLVX::init() {
    DEBUG_STDOUT("Inicializando el escaner de archivos lvx.");

    // Abrimos archivo
    if (lvx_file.Open(filename.c_str(), std::ios::in) != livox_ros::kLvxFileOk) {
        LOG_ERROR("Fallo al inicializar el escaner de archivos lvx.");

        return false;
    }

    // Reservamos memoria para el paquete
    const uint32_t kMaxPacketsNumOfFrame = 8192;
    packets_of_frame.buffer_capacity = kMaxPacketsNumOfFrame * sizeof(livox_ros::LvxFilePacket);
    packets_of_frame.packet = new uint8_t[kMaxPacketsNumOfFrame * sizeof(livox_ros::LvxFilePacket)];

    LOG_INFO("Escaner de archivos lvx inicializado correctamente.");

    return true;
}

// Comienza la obtención de puntos
bool ScannerLVX::start() {
    LOG_INFO("Inicio del escaneo de puntos.");

    packets_of_frame.data_size = 0;  // Borramos total de puntos guardados

    if (lvx_file.GetFileState() == livox_ros::kLvxFileOk) {
        exit = false;                                                    // Permitimos la ejecución del hilo
        executionThread = new std::thread(&ScannerLVX::readData, this);  // Creamos hilo
    }
    // Fallo de apertura
    else {
        LOG_ERROR("Fallo de apertura del archivo de puntos.");
        return false;
    }

    return true;
};

// Establece la función especificada como función de callback a la que se llamará cada vez que
// se escanee un nuevo punto
bool ScannerLVX::setCallback(const std::function<void(const Point &p)> func) {
    DEBUG_STDOUT("Estableciendo el callback.");

    callback = func;
    return ((bool)callback);
}

// Finaliza el escaner
void ScannerLVX::stop() {
    DEBUG_STDOUT("Finalizando el escaneo de puntos.");

    exit = true;              // Comunicamos al hilo que finalice la ejecución
    executionThread->join();  // Realizamos unión del hilo de lectura

    lvx_file.CloseLvxFile();  // Cerramos stream del archivo

    delete packets_of_frame.packet;  // Array de paquetes

    LOG_INFO("Finalizado el escaneo de puntos.");
}

// Non-busy wait hasta la finalización del escaner
void ScannerLVX::wait() {
    DEBUG_STDOUT("Esperamos hasta la finalización del escaneo de puntos.");

    executionThread->join();  // Realizamos unión del hilo de lectura

    lvx_file.CloseLvxFile();  // Cerramos stream del archivo

    delete packets_of_frame.packet;  // Array de paquetes

    LOG_INFO("Finalizado el escaneo de puntos.");
}

// Lectura de puntos del archivo
void ScannerLVX::readData() {
    int r = livox_ros::kLvxFileOk;  // Estado del archivo

    r = lvx_file.GetPacketsOfFrame(&packets_of_frame);  // Recuperamos paquetes del archivo

    uint8_t *packet_base;
    uint32_t data_offset = 0;
    LivoxEthPacket *eth_packet;
    livox_ros::LvxFilePacket *detail_packet;
    uint8_t data_type;

    // Recorremos el archivo
    while (!exit && r == livox_ros::kLvxFileOk) {
        packet_base = packets_of_frame.packet;  // Data array pointer
        data_offset = 0;                        // Data offset in array

        // Recorremos todo el array de paquetes
        while (!exit && data_offset < packets_of_frame.data_size) {
            if (lvx_file.GetFileVersion() != 0) {
                detail_packet = (livox_ros::LvxFilePacket *)&packet_base[data_offset];  // Point packet
                eth_packet = (LivoxEthPacket *)(&detail_packet->version);               // Ethernet packet

            } else {
                livox_ros::LvxFilePacketV0 *detail_packet = (livox_ros::LvxFilePacketV0 *)&packet_base[data_offset];  // V0 Point packet
                eth_packet = (LivoxEthPacket *)(&detail_packet->version);                                             // Ethernet packet
            }

            data_type = eth_packet->data_type;  // Packet data type
            if (data_type == kExtendCartesian) {
                const int points_in_packet = livox_ros::GetPointsPerPacket(eth_packet->data_type);
                size_t packet_offset = 0;
                for (int i = 0; !exit && i < points_in_packet; i++) {
                    LivoxExtendRawPoint *point = reinterpret_cast<LivoxExtendRawPoint *>(eth_packet->data + packet_offset);

                    // Llamada al callback
                    if (this->callback) {
                        Point p = {Timestamp(eth_packet->timestamp), point->reflectivity, point->x, point->y, point->z};
                        this->callback(p);
                    }

                    packet_offset += sizeof(LivoxExtendRawPoint);
                }
            }
            data_offset += (livox_ros::GetEthPacketLen(data_type) + 1);
        }

        r = lvx_file.GetPacketsOfFrame(&packets_of_frame);  // Recuperamos paquetes del archivo
    }

    exit = true;  // Finalización
}