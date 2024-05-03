//
// Created by Joseph on 3/19/2024.
//

#include "Gateway.h"
#include "BufferHandler.h"

Gateway::Gateway(string gatewayIP, int gatewayPort, PacketBuffer* buffer): receiveBuffer(buffer) {
    enet_address_set_host(&clientAddress, gatewayIP.c_str());
    clientAddress.port = gatewayPort;

    client = enet_host_create(&clientAddress, 32, 2, 0, 0);
    if (client == nullptr) {
        std::cerr << "Failed to create Gateway Instance.\n";
        exit(EXIT_FAILURE);
    }else{
        cout << "Created Gateway Instance" << endl;
    }
}

Gateway::~Gateway() {
    if(shutdownFlag.load()){
        shutdown();
    }
}

void Gateway::start() {
    shutdownFlag.store(false);
    networkThread = thread(&Gateway::networkLoop, this);
}

void Gateway::shutdown() {

}

const ENetAddress &Gateway::getAddress() const {
    return clientAddress;
}

int Gateway::getPort() const {
    return clientPort;
}

const atomic<bool> &Gateway::getShutdownFlag() const {
    return shutdownFlag;
}

const map<string, pair<string, ENetPeer *>> &Gateway::getServerList() const {
    return serverList;
}

void Gateway::networkLoop() {
    if(client == nullptr){
        fprintf(stderr, "Gateway was not initialized. No ENetHost exists.\n");
        exit(EXIT_FAILURE);
    }
    ENetEvent event;
    printf("Network Loop Initialized\n");
    while(!shutdownFlag.load()){
        while(enet_host_service(client, &event,15000) > 0){
            switch(event.type){
                case ENET_EVENT_TYPE_RECEIVE:{
                    auto ODPacket = GetOD_Packet(event.packet->data);
                    if(!ODPacket){
                        cerr << "Invalid Packet: No serial Data" << endl;
                        break;
                    }else{
                        cout << " recieved packet in gateway" << endl;
                    }
                    receiveBuffer->addPacket(std::unique_ptr<ENetPacket>(event.packet));
                    break;
                }
                case ENET_EVENT_TYPE_NONE:
                    cout << "nothing happened" << endl;
                    break;
                case ENET_EVENT_TYPE_CONNECT:
                    printf("A Server connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("%s disconnected.\n", event.peer->data);
                    break;
            }
        }
    }

}