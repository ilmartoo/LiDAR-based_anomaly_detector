/**
 * @file ScannerLVX.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 03/04/2022
 *
 * Implementación del objeto ScannerLVX
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "lvx_file.h"
#include "lds.h"

#include "scanner/ScannerLVX.hh"
#include "models/Point.hh"
#include "models/Timestamp.hh"

#include "logging/debug.hh"
#include "logging/logging.hh"

bool ScannerLVX::init() {
    DEBUG_STDOUT("Inicializando el escaner de archivos lvx.");

    if (lvx_file.Open(filename.c_str(), std::ios::in) != livox_ros::kLvxFileOk) {
        LOG_ERROR("Fallo al inicializar el escaner de archivos lvx.");

        return false;
    }

    const uint32_t kMaxPacketsNumOfFrame = 8192;
    packets_of_frame.buffer_capacity = kMaxPacketsNumOfFrame * sizeof(livox_ros::LvxFilePacket);
    packets_of_frame.packet = new uint8_t[kMaxPacketsNumOfFrame * sizeof(livox_ros::LvxFilePacket)];

    fileOffset = 0;
    frameOffset = 0;
    packetOffset = 0;

    DEBUG_STDOUT("Escaner de archivos lvx inicializado correctamente.");

    return true;
}

bool ScannerLVX::scan(std::condition_variable &cv, std::mutex &mutex) {
    DEBUG_STDOUT("Inicio del escaneo de puntos.");

    // Reabre el archivo si no está abierto
    if (lvx_file.GetLvxFileReadProgress() == 100) {
        lvx_file.CloseLvxFile();
        if (lvx_file.Open(filename.c_str(), std::ios::in) != livox_ros::kLvxFileOk) {
            LOG_ERROR("Fallo al abrir el escaner de archivos lvx.");

            return false;
        }

        fileOffset = 0;
        frameOffset = 0;
        packetOffset = 0;
    }

    if (lvx_file.GetFileState() == livox_ros::kLvxFileOk) {
        scanning = true;
        std::thread([this, &cv, &mutex]() { this->readData(cv, mutex); }).detach();

        return true;
    }
    // Fallo de apertura
    else {
        LOG_ERROR("Fallo de apertura del archivo de puntos.");
        return false;
    }
}

void ScannerLVX::pause() {
    scanning = false;
}

bool ScannerLVX::setCallback(const std::function<void(const Point &p)> func) {
    DEBUG_STDOUT("Estableciendo el callback.");

    callback = func;
    return ((bool)callback);
}

void ScannerLVX::wait() {
    DEBUG_STDOUT("Esperando a la finalización del escaneo de puntos.");

    std::mutex mtx;
    std::condition_variable cv;
    readData(cv, mtx);

    lvx_file.CloseLvxFile();

    delete packets_of_frame.packet;

    DEBUG_STDOUT("Finalizado el escaneo de puntos.");
}

void ScannerLVX::stop() {
    DEBUG_STDOUT("Finalizando el escaneo de puntos.");

    lvx_file.CloseLvxFile();

    delete packets_of_frame.packet;

    DEBUG_STDOUT("Finalizado el escaneo de puntos.");
}

void ScannerLVX::readData(std::condition_variable &cv, std::mutex &mutex) {
    {
        std::lock_guard<std::mutex> lock(mutex);  // Bloqueamos el mutex durante la funcion

        int fileState = livox_ros::kLvxFileOk;  // Estado del archivo

        uint8_t *packet_base;        // Array de paquetes ethernet
        LivoxEthPacket *eth_packet;  // Puntero al paquete de datos

        if (packetOffset == 0 && frameOffset == 0) {
            fileState = lvx_file.GetPacketsOfFrame(&packets_of_frame);
        }
        for (; scanning && fileState == livox_ros::kLvxFileOk; ++fileOffset, fileState = lvx_file.GetPacketsOfFrame(&packets_of_frame)) {
            for (packet_base = packets_of_frame.packet; scanning && frameOffset < packets_of_frame.data_size; frameOffset += (livox_ros::GetEthPacketLen(eth_packet->data_type) + 1)) {
                // V1 Point packet
                if (lvx_file.GetFileVersion() != 0) {
                    eth_packet = (LivoxEthPacket *)(&((livox_ros::LvxFilePacket *)&packet_base[frameOffset])->version);
                }
                // V0 Point packet
                else {
                    eth_packet = (LivoxEthPacket *)(&((livox_ros::LvxFilePacketV0 *)&packet_base[frameOffset])->version);
                }

                if (eth_packet->data_type == kExtendCartesian) {
                    const int points_in_packet = livox_ros::GetPointsPerPacket(eth_packet->data_type);
                    int i;
                    for (i = packetOffset / sizeof(LivoxExtendRawPoint); scanning && i < points_in_packet; ++i, packetOffset += sizeof(LivoxExtendRawPoint)) {
                        LivoxExtendRawPoint *point = reinterpret_cast<LivoxExtendRawPoint *>(eth_packet->data + packetOffset);

                        // Llamada al callback
                        if (this->callback) {
                            Point p = {Timestamp(eth_packet->timestamp), point->reflectivity, point->x, point->y, point->z};
                            this->callback(p);
                        }
                    }
                    packetOffset = i == points_in_packet ? 0 : packetOffset;
                }
            }
            frameOffset = frameOffset == packets_of_frame.data_size ? 0 : frameOffset;
        }

        if (lvx_file.GetLvxFileReadProgress() == 100) {
            DEBUG_STDOUT("Se ha llegado al final del paquete de puntos LVX");
        }

        scanning = false;
    }
    cv.notify_one();
}