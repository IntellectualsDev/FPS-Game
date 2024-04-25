//
// Created by Joseph on 3/18/2024.
//

#include "Transmitter.h"

Transmitter::Transmitter(const string& temp_address, int port, PacketBuffer &transmitBuffer,mutex& consoleMutex):
    builder(1024),
    transmitBuffer(transmitBuffer),
    shutdownFlag(false),
    consoleMutex(consoleMutex)
    {
        char* clientAddressChar = new char[temp_address.size()+1];

        strcpy(clientAddressChar,temp_address.c_str());
        cout << clientAddressChar << endl;
        enet_address_set_host_ip(&address,clientAddressChar);

        address.port = port;
        client = enet_host_create(NULL,2,1,0,0);
        if (client == nullptr) {
//            {
//                std::lock_guard<std::mutex> guard(consoleMutex);
//                fprintf(stderr, "An error occurred while trying to create Transmitter CLient ENetHost instance\n");
//            }
            exit(EXIT_FAILURE);
        }
//        {
//            std::lock_guard<std::mutex> guard(consoleMutex);
//        printf("Created Transmitter Client ENetHost  instance @ %x:%u\n", client->address.host, client->address.port);
//        }
    }

void Transmitter::start(){
    shutdownFlag.store(false);
    transmitThread = thread(&Transmitter::transmitLoop,this);
}
void Transmitter::transmitLoop(){
//    {
//        std::lock_guard<std::mutex> guard(consoleMutex);
//        cout << "in transmit loop" << endl;
//    }
    connect("192.168.1.13",5450);
    while(!shutdownFlag.load()){

        auto packetList = transmitBuffer.removePacketWait();
        if(packetList.empty()) {
//            {
//                std::lock_guard<std::mutex> guard(consoleMutex);
//                cout << "no packtes to pull" << endl;
//            }
            continue;
        }else{
            int packetListSize = packetList.size();
            for(int i = packetListSize-1; i >= 0; i--){
                if(packetList[i] != nullptr){

                    transmitPacket(std::move(packetList[i]));
                }else{
//                    {
//                        std::lock_guard<std::mutex> guard(consoleMutex);
//                        fprintf(stderr, "Error in transmit loop,transmit packet pulled Null packet from PacketBuffer!");
//
//                    }
                }
            }
        }
    }
}
//TODO:
//on no connection try again a couple times then timeout
bool Transmitter::connect(const string& serverIP, int port){
    ENetAddress serverAddress;
    ENetEvent event;
    char* serverAddressChar = new char[serverIP.length()+1];
    strcpy(serverAddressChar,serverIP.c_str());
//    {
//        std::lock_guard<std::mutex> guard(consoleMutex);
//        printf("Resolved server IP: %s\n", serverAddressChar);
//        printf("Resolved server port: %d\n", port);
//    }

    enet_address_set_host_ip(& serverAddress, serverAddressChar);
    serverAddress.port = port;
    server = enet_host_connect(client,&serverAddress,2,0);
    if(server == nullptr){
//        {
//            std::lock_guard<std::mutex> guard(consoleMutex);
//            fprintf(stderr, "No available server for initiating an ENet connection\n");
//
//        }
        exit(EXIT_FAILURE);
    }
    if(enet_host_service(client, & event, 15000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT){ //wait 5 seconds for a connection attempt to succeed
//        {
//
//            std::lock_guard<std::mutex> guard(consoleMutex);
//            printf("Connected to the server successfully\n\tIP: %s\n\tPort: %s\n", serverIP.c_str(), to_string(port).c_str());
//
//        }
        return true;
    }
    else{

        enet_peer_reset(server);
//        {
//            std::lock_guard<std::mutex> guard(consoleMutex);
//            printf("Failed to connect to server\n\tIP: %s\n\tPort: %s\n", serverIP.c_str(), to_string(port).c_str());
//
//        }
        return false;
    }
}
void Transmitter::transmitPacket(unique_ptr<ENetPacket> packet) {
    ENetEvent event;
    enet_host_service(client, & event, 0);
    ENetPacket* packetToSend = enet_packet_create(packet->data,packet->dataLength,packet->flags);
    enet_peer_send(server, 0, packet.get());
    enet_host_flush(client);
    {
        std::lock_guard<std::mutex> guard(consoleMutex);
        std::unique_ptr<uint8_t[]> bufferCopy(new uint8_t[packet->dataLength]);
        memcpy(bufferCopy.get(), packet->data, packet->dataLength);
        uint8_t *temp= packet->data;
        auto temp_packet = flatbuffers::GetRoot<OD_Packet>(temp);
        auto input_size = temp_packet->payload()->payload_CI()->client_inputs()->size();
//        unique_ptr<BufferHandler> packetBufferHandler = std::make_unique<BufferHandler>(std::move(bufferCopy), packet->dataLength);
//        if(packetBufferHandler->getPacketView()->payload()->payload_type() == PayloadTypes_ClientInputs){
//            auto clientInputs = packetBufferHandler->getPacketView()->payload()->payload_as_ClientInputs();
//            if(clientInputs){
//                auto inputs = clientInputs->client_inputs();
//                inputs->
//                inputs->GetAs<Input>(0);
//                if(inputs && inputs->size() > 0){
//                    inputs->Get(0);
//                    auto temp = inputs->data();
//                    cout << "w:" << temp << endl;
//                }
//            }
//        }
//        cout << "output packet size: " << packetBufferHandler->getPacketView()->payload()->payload_as_ClientInputs()->client_inputs()->size() << endl;
//        cout << " output packet w: "<<packetBufferHandler->getPacketView()->payload()->payload_as_ClientInputs()->client_inputs()->Get(0)->w() << endl;

    }
    std::move(packet);


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
