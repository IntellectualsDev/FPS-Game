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
    Transmitter(const string& temp_address, int port, PacketBuffer& transmitBuffer,mutex& consoleMutex);
    void start();
    void shutdown();
    PacketBuffer& getPacketBuffer();

private:
    ENetPeer* server;
    flatbuffers::FlatBufferBuilder builder;
    bool connect(const string& serverIP,int port);
    void transmitLoop();
    void transmitPacket(unique_ptr<ENetPacket> packet);
    ENetHost* client;
    ENetAddress address;
    int port;
    std::thread transmitThread;
    std::atomic<bool> shutdownFlag;
    PacketBuffer& transmitBuffer;
    mutex &consoleMutex;
};


#endif //FPS_GAME_TRANSMITTER_H
