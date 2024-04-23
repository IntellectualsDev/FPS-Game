//
// Created by Joseph on 3/19/2024.
//

#include "Gateway.h"
#include "BufferHandler.h"

Gateway::Gateway(string gatewayIP, int gatewayPort, PacketBuffer& buffer): receiveBuffer(buffer) {
    char* serverAddressChar = new char[gatewayIP.length()+1];
    strcpy(serverAddressChar, gatewayIP.c_str());
    printf("char array for Gateway Server = %s\n", serverAddressChar);
    enet_address_set_host_ip(&address, serverAddressChar);
    address.port = gatewayPort;
    port = gatewayPort;
    client = enet_host_create(&address,2,1,0,0);
    if (client == NULL) {
        fprintf(stderr, "An error occurred while trying to create Gateway Server ENetHost instance\n");
        exit(EXIT_FAILURE);
    }
    printf("Created Gateway Server ENetHost  instance @ %x:%u\n", client->address.host, client->address.port);
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
    return address;
}

int Gateway::getPort() const {
    return port;
}

const atomic<bool> &Gateway::getShutdownFlag() const {
    return shutdownFlag;
}

const map<string, pair<string, ENetPeer *>> &Gateway::getServerList() const {
    return serverList;
}

void Gateway::networkLoop() {
    if(client == NULL){
        fprintf(stderr, "Gateway was not initialized. No ENetHost exists.\n");
        exit(EXIT_FAILURE);
    }
    ENetEvent event;
    printf("Network Loop Initialized\n");
    while(!shutdownFlag.load()){
        while(enet_host_service(client, &event,0) > 0){
            switch(event.type){
                case ENET_EVENT_TYPE_RECEIVE:{
                    auto ODPacket = GetOD_Packet(event.packet->data);
                    if(!ODPacket){
                        cerr << "Invalid Packet: No serial Data" << endl;
                        break;
                    }
                    std::unique_ptr<uint8_t[]> bufferCopy(new uint8_t[event.packet->dataLength]);
                    memcpy(bufferCopy.get(), event.packet->data, event.packet->dataLength);
                    unique_ptr<BufferHandler> packetBufferHandler = std::make_unique<BufferHandler>(std::move(bufferCopy), event.packet->dataLength);
                    receiveBuffer.addBufferHandler(std::move(packetBufferHandler));
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

