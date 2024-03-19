//
// Created by Joseph on 3/18/2024.
//

#ifndef FPS_GAME_TRANSMITTER_H
#define FPS_GAME_TRANSMITTER_H
#include <thread>
#include <map>
#include <enet/enet.h>
#include <atomic>
#include "PacketBuffer.h"
#include "DevEnv/flatbuffers/include/flatbuffers/flatbuffer_builder.h"
class Transmitter {
public:
    Transmitter(ENetHost* server, ENetAddress address, int port, PacketBuffer& transmitBuffer);
    void start();
    void shutdown();
    PacketBuffer& getPacketBuffer();

private:
    flatbuffers::FlatBufferBuilder builder;
    void transmitLoop();
    void transmitPacket(unique_ptr<OD_Packet> packet);
    ENetHost* client;
    ENetAddress address;
    int port;
    std::thread transmitThread;
    std::atomic<bool> shutdownFlag;
    PacketBuffer& transmitBuffer;
};


#endif //FPS_GAME_TRANSMITTER_H
