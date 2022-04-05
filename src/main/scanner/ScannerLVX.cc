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
#include "debug_lbad.hh"

// Inicialización del escaner
bool ScannerLVX::init() {
    printDebug("Inicializando el escaner de archivos lvx.");  // debug

    // Abrimos archivo
    if (lvx_file.Open(filename.c_str(), std::ios::in) != livox_ros::kLvxFileOk) {
        std::cerr << "Fallo al inicializar el escaner" << std::endl;

        return false;
    }

    // Reservamos memoria para el paquete
    const uint32_t kMaxPacketsNumOfFrame = 8192;
    packets_of_frame.buffer_capacity = kMaxPacketsNumOfFrame * sizeof(livox_ros::LvxFilePacket);
    packets_of_frame.packet = new uint8_t[kMaxPacketsNumOfFrame * sizeof(livox_ros::LvxFilePacket)];

    std::cout << "Escaner de archivos lvx inicializado correctamente." << std::endl;

    return true;
}

// Comienza la obtención de puntos
bool ScannerLVX::start() {
    std::cout << "Inicio del escaneo de puntos." << std::endl;

    packets_of_frame.data_size = 0;  // Borramos total de puntos guardados

    if (lvx_file.GetFileState() == livox_ros::kLvxFileOk) {
        exit = false;                                                    // Permitimos la ejecución del hilo
        executionThread = new std::thread(&ScannerLVX::readData, this);  // Creamos hilo
    }
    // Fallo de apertura
    else {
        std::cerr << "Fallo de apertura del archivo de puntos." << std::endl;
        return false;
    }

    return true;
};

// Establece la función especificada como función de callback a la que se llamará cada vez que
// se escanee un nuevo punto
bool ScannerLVX::setCallback(const std::function<void(const Point &p)> func) {
    printDebug("Estableciendo el callback.");  // debug

    callback = func;
    return ((bool)callback);
}

// Finaliza el escaner
void ScannerLVX::stop() {
    printDebug("Finalizando el escaneo de puntos.");

    exit = true;              // Comunicamos al hilo que finalice la ejecución
    executionThread->join();  // Realizamos unión del hilo de lectura

    lvx_file.CloseLvxFile();  // Cerramos stream del archivo

    delete packets_of_frame.packet;  // Array de paquetes

    std::cout << "Finalizado el escaneo de puntos." << std::endl;
}

// Lectura de puntos del archivo
void ScannerLVX::readData() {
    int r = livox_ros::kLvxFileOk;  // Estado del archivo

    r = lvx_file.GetPacketsOfFrame(&packets_of_frame);  // Recuperamos paquetes del archivo

    uint8_t *packet_base;
    uint32_t data_offset = 0;
    LivoxEthPacket *eth_packet;
    livox_ros::LvxFilePacket *detail_packet;
    int32_t handle;
    uint8_t data_type;

    while (r == livox_ros::kLvxFileOk) {
        packet_base = packets_of_frame.packet;  // Data array pointer
        data_offset = 0;                        // Data offset in array

        // Recorremos todo el array
        while (data_offset < packets_of_frame.data_size) {
            if (lvx_file.GetFileVersion() != 0) {
                detail_packet = (livox_ros::LvxFilePacket *)&packet_base[data_offset];  // Point packet
                eth_packet = (LivoxEthPacket *)(&detail_packet->version);               // Ethernet packet
                handle = detail_packet->device_index;                                   // Packet handle

            } else {
                livox_ros::LvxFilePacketV0 *detail_packet =
                    (livox_ros::LvxFilePacketV0 *)&packet_base[data_offset];  // V0 Point packet
                eth_packet = (LivoxEthPacket *)(&detail_packet->version);     // Ethernet packet
                handle = detail_packet->device_index;                         // Packet handle
            }

            data_type = eth_packet->data_type;  // Packet data type
            if (data_type == kExtendCartesian) {
                const int points_in_packet = livox_ros::GetPointsPerPacket(eth_packet->data_type);
                size_t packet_offset = 0;
                for (int i = 0; i < points_in_packet; i++) {
                    LivoxExtendRawPoint *point =
                        reinterpret_cast<LivoxExtendRawPoint *>(eth_packet->data + packet_offset);

                    // Llamada al callback
                    if (this->callback) {
                        this->callback(
                            Point(Timestamp(eth_packet->timestamp), point->reflectivity, point->x, point->y, point->z));
                    }

                    packet_offset += sizeof(LivoxExtendRawPoint);
                }
            }
            data_offset += (livox_ros::GetEthPacketLen(data_type) + 1);
        }

        r = lvx_file.GetPacketsOfFrame(&packets_of_frame);  // Recuperamos paquetes del archivo
    }
}