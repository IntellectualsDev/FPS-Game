//
// Created by Anshul Gowda on 2/14/24.
//

#ifndef ODYSSEYGAMESERVER_DUMMYCLIENTF_H
#define ODYSSEYGAMESERVER_DUMMYCLIENTF_H

#include <enet/enet.h>
#include <cstdio>
#include <string>
#include <cstring>

using namespace std;


class DummyClient {
private:
    ENetHost * client;
    ENetPeer * server;
    string serverIP;
    int serverPort;
    int numConnections;
    int numChannels;
    int incomingBandwith;
    int outgoingBandwith;
public:
    DummyClient(int numConnections = 2, int numChannels = 2, int incomingBandwith = 0, int outgoingBandwith = 0);
    bool connect(const string& serverIP, int port);

    bool disconnect();

    void listenForPackets();

    bool sendPacket(string message);
};


#endif //ODYSSEYGAMESERVER_DUMMYCLIENTF_H
