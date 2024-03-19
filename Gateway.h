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
    Gateway();
    ~Gateway();
    void start();
    void shutdown();
    const ENetAddress &getAddress() const;
    int getPort() const;
    const atomic<bool> &getShutdownFlag() const;
    const map<string, pair<string, ENetPeer *>> &getClientList() const;
private:
    void networkLoop();
    ENetHost* server;
    ENetAddress address;
    int port;

    thread networkThead;
    mutex networkMutex;
    condition_variable networkCV;
    atomic<bool> shutdownFlag;
    PacketBuffer& receiveBuffer;
    map<string,pair<string,ENetPeer*>> serverList;//implement ttl
};


#endif //FPS_GAME_GATEWAY_H
