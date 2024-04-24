//
// Created by Joseph on 3/19/2024.
//

#ifndef FPS_GAME_GATEWAY_H
#define FPS_GAME_GATEWAY_H
#include <string>
#include <map>

#include "PacketBuffer.h"
class Gateway {
public:
    Gateway(string gatewayIP, int gatewayPort, PacketBuffer& buffer);
    ~Gateway();
    void start();
    void shutdown();
    const ENetAddress &getAddress() const;
    int getPort() const;
    const atomic<bool> &getShutdownFlag() const;
    const map<string, pair<string, ENetPeer *>> &getServerList() const;
private:
    void networkLoop();
    ENetHost* client;
    ENetAddress address;
    int port;

    thread networkThread;
    mutex networkMutex;
    condition_variable networkCV;
    atomic<bool> shutdownFlag;
    PacketBuffer& receiveBuffer;
    map<string,pair<string,ENetPeer*>> serverList;//implement ttl
};


#endif //FPS_GAME_GATEWAY_H