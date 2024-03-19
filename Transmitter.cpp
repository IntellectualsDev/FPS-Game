//
// Created by Joseph on 3/18/2024.
//

#include "Transmitter.h"

Transmitter::Transmitter(ENetHost *client, ENetAddress address, int port, PacketBuffer &transmitBuffer):
    client(client),
    builder(1024),
    address(address),
    port(port),
    transmitBuffer(transmitBuffer),
    shutdownFlag(false)
    {}

void Transmitter::start(){
    shutdownFlag.store(false);
    transmitThread = thread(&Transmitter::transmitLoop,this);
}
void Transmitter::transmitLoop(){
    while(!shutdownFlag.load()){
        auto packetList = transmitBuffer.removePacketWait();
        unique_ptr<ENetPacket> packet = std::move(packetList->front());
        if(packet){
            transmitPacket(std::move(packet));
        }else{
            fprintf(stderr, "Error in transmit loop,transmit packet pulled Null packet from PacketBuffer!");
        }
    }
}
void Transmitter::transmitPacket(unique_ptr<ENetPacket> packet) {
    ENetEvent event;
    char* serverAddressChar = new char[GetOD_Packet(packet->data)->dest_point()->address()->size()+1];
    strcpy(serverAddressChar,GetOD_Packet(packet->data)->dest_point()->address()->c_str());
    ENetAddress serverAddress;
    enet_address_set_host_ip(&serverAddress,serverAddressChar);
    serverAddress.port = GetOD_Packet(packet->data)->dest_point()->port();
    if(GetOD_Packet(packet->data)->dest_point() != nullptr){
        ENetPeer* server = enet_host_connect(client,&serverAddress,1,0);
        if(server == nullptr){
            exit(EXIT_FAILURE);
        }if(enet_host_service(client, &event, 6000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT){
            printf("Transmitter Connected to the server successfully\n\tIP: %u\n\tPort: %s\n\n", client->address.host, to_string(port).c_str());
        }else{
            enet_peer_reset(server);
            fprintf(stderr, "ERR In Transmitter's Transmit method() | line 51\n\tFailed to connect to server\n\tIP: %u\n\tPort: %s\n\n", server->address.host, to_string(port).c_str());
            return;
        }

        ENetPacket* packetToSend = enet_packet_create(packet->data,packet->dataLength,packet->flags);
        enet_peer_send(server, 0, packetToSend);
        enet_host_flush(client);

        printf("Transmitter Sent packet\n\tpayload_type = %s", GetOD_Packet(packet->data)->packet_type());
        std::move(packet);
    }else{
        fprintf(stderr, "ERR In Transmitter's Transmit method() dest_point in packet is nullptr");
    }

}

PacketBuffer &Transmitter::getPacketBuffer() {
    return transmitBuffer;
}

void Transmitter::shutdown() {
    shutdownFlag.store(true);
    transmitBuffer.notifyAll();
    if(transmitThread.joinable()){
        transmitThread.join();
    }
}
