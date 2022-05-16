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
#include <stdint.h>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "lvx_file.h"
#include "lds.h"

#include "scanner/ScannerLVX.hh"
#include "models/LidarPoint.hh"
#include "models/Timestamp.hh"
#include "app/CLICommand.hh"

#include "logging/debug.hh"

bool ScannerLVX::init() {
    DEBUG_STDOUT("Initializing lvx file scanner");

    if (lvx_file.Open(filename.c_str(), std::ios::in) != livox_ros::kLvxFileOk) {
        CLI_STDERR("Error while initializing lvx file scanner");

        return false;
    }

    const uint32_t kMaxPacketsNumOfFrame = 8192;
    packets_of_frame.buffer_capacity = kMaxPacketsNumOfFrame * sizeof(livox_ros::LvxFilePacket);
    packets_of_frame.packet = new uint8_t[kMaxPacketsNumOfFrame * sizeof(livox_ros::LvxFilePacket)];

    frameOffset = 0;
    packetOffset = 0;

    DEBUG_STDOUT("Initialized lvx file scanner");

    return true;
}

ScanCode ScannerLVX::scan() {
    DEBUG_STDOUT("Starting point scanning");

    // Reabre el archivo si no está abierto
    if (frameOffset == 0 && packetOffset == 0 && lvx_file.GetFileState() == livox_ros::kLvxFileAtEnd) {
        lvx_file.CloseLvxFile();
        lvx_file.Open(filename.c_str(), std::ios::in);
    }

    if (lvx_file.GetFileState() == livox_ros::kLvxFileOk) {
        if (!scanning) {
            scanning = true;
            return readData();

        } else {
            CLI_STDERR("Scanner already in use");
            return ScanCode::kScanError;
        }
    }
    // Fallo de apertura
    else {
        CLI_STDERR("Error while opening lvx file");
        return ScanCode::kScanError;
    }
}

void ScannerLVX::pause() {
    scanning = false;
}

bool ScannerLVX::setCallback(const std::function<void(const LidarPoint &p)> func) {
    DEBUG_STDOUT("Setting up callback");

    callback = func;
    return ((bool)callback);
}

void ScannerLVX::wait() {
    DEBUG_STDOUT("Waiting for the scanner to end");

    readData();

    delete packets_of_frame.packet;

    DEBUG_STDOUT("Ended point scanning");
}

void ScannerLVX::stop() {
    DEBUG_STDOUT("Closing scanner");

    lvx_file.CloseLvxFile();

    delete packets_of_frame.packet;

    DEBUG_STDOUT("Scanner closed");
}

ScanCode ScannerLVX::readData() {
    int fileState = livox_ros::kLvxFileOk;  // Estado del archivo

    uint8_t *packet_base;        // Array de paquetes ethernet
    LivoxEthPacket *eth_packet;  // Puntero al paquete de datos
    LivoxExtendRawPoint *point;  // Puntero de punto de datos

    if (packetOffset == 0 && frameOffset == 0) {
        fileState = lvx_file.GetPacketsOfFrame(&packets_of_frame);
    }
    while (fileState == livox_ros::kLvxFileOk) {
        packet_base = packets_of_frame.packet;
        while (frameOffset < packets_of_frame.data_size) {
            // V1 Point packet
            if (lvx_file.GetFileVersion() != 0) {
                eth_packet = (LivoxEthPacket *)(&((livox_ros::LvxFilePacket *)&packet_base[frameOffset])->version);
            }
            // V0 Point packet
            else {
                eth_packet = (LivoxEthPacket *)(&((livox_ros::LvxFilePacketV0 *)&packet_base[frameOffset])->version);
            }

            if (eth_packet->data_type == kExtendCartesian) {
                const uint32_t points_in_packet = livox_ros::GetPointsPerPacket(eth_packet->data_type);
                uint32_t i = packetOffset / sizeof(LivoxExtendRawPoint);
                while (i < points_in_packet) {
                    point = reinterpret_cast<LivoxExtendRawPoint *>(eth_packet->data + packetOffset);

                    // Llamada al callback
                    if (this->callback) {
                        this->callback({Timestamp(eth_packet->timestamp), point->reflectivity, point->x, point->y, point->z});
                    }

                    if (scanning) {
                        ++i;
                        packetOffset += sizeof(LivoxExtendRawPoint);
                    } else {
                        break;
                    }
                }
            }

            if (scanning) {
                packetOffset = 0;
                frameOffset += (livox_ros::GetEthPacketLen(eth_packet->data_type) + 1);
            } else {
                break;
            }
        }

        if (scanning) {
            frameOffset = 0;
            fileState = lvx_file.GetPacketsOfFrame(&packets_of_frame);
        } else {
            break;
        }
    }

    if (frameOffset == 0 && packetOffset == 0 && lvx_file.GetFileState() == livox_ros::kLvxFileAtEnd) {
        CLI_STDERR("EOF reached");

        DEBUG_STDOUT("FO: " << frameOffset << " PO: " << packetOffset);

        scanning = false;
        return ScanCode::kScanEof;
    }

    return ScanCode::kScanOk;
}