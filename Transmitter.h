//
// Created by Anshul Gowda on 2/24/24.
//

#ifndef ODYSSEYGAMESERVER_TRANSMITTER_H
#define ODYSSEYGAMESERVER_TRANSMITTER_H

#include <thread>
#include <map>

#include "PacketBuffer.h"
#include "flatbuffers/buffer.h"


class Transmitter {
public:
    Transmitter(string gatewayIP,
                int port,
                PacketBuffer *transmitBuffer,
                mutex& consoleMutex,string serverIP,int serverPort);

    void start();

    void shutdown();




private:
    // Pass down in GatewayServer constructor
    void transmitLoop();
    ENetPeer * connect(const string& serverIP, int port);
    bool disconnect(const string& server);
    void transmitPacket(unique_ptr<ENetPacket> packet);
    string serverIP;
    int serverPort;
    ENetPeer * server;
    ENetHost* client;
    ENetAddress address;
//    vector<ENetPeer *> peers;
    int port;

    thread transmitThread;
    atomic<bool> shutdownFlag;

    PacketBuffer* transmitBuffer;

    std::mutex& consoleMutex;
};


#endif //ODYSSEYGAMESERVER_TRANSMITTER_H