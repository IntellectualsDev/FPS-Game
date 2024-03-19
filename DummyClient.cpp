//
// Created by Anshul Gowda on 2/14/24.
//

#include "DummyClient.h"

DummyClient::DummyClient(int numConnections, int numChannels, int incomingBandwith, int outgoingBandwith){
    client = enet_host_create(NULL, numConnections, numChannels, incomingBandwith, outgoingBandwith);
    this->numConnections = numChannels;
    this->numChannels = numChannels;
    this->incomingBandwith = incomingBandwith;
    this->outgoingBandwith = outgoingBandwith;
    if (client == NULL) {
        fprintf(stderr, "Error occured while creating ENET Client\n");
        exit(EXIT_FAILURE);
    }

    printf("Created ENetHost Client instance @ %x:%u\n", client->address.host, client->address.port);
}
bool DummyClient::connect(const string& serverIP, int port){
    ENetAddress serverAddress;
    ENetEvent event;;
    this->serverIP = serverIP;
    this->serverPort = port;

    char* serverIPChar = new char[serverIP.length()+1]; // convert string IP to char * used in enet set host ip
    strcpy(serverIPChar, serverIP.c_str());

    printf("Resolved server IP: %s\n", serverIPChar);
    printf("Resolved server port: %d\n", port);

    enet_address_set_host_ip(& serverAddress, serverIPChar);
    serverAddress.port = port;

//    enet_address_set_host(&serverAddress, "127.0.0.1");
//    serverAddress.port = 5450;

    server = enet_host_connect(client, &serverAddress, 2, 0);

    if(server == NULL){
        fprintf(stderr, "No available server for initiating an ENet connection\n");
        exit(EXIT_FAILURE);
    }

    if(enet_host_service(client, & event, 15000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT){ //wait 5 seconds for a connection attempt to succeed
        printf("Connected to the server successfully\n\tIP: %s\n\tPort: %s\n", serverIP.c_str(), to_string(port).c_str());
        return true;
    }
    else{
        enet_peer_reset(server);
        printf("Failed to connect to server\n\tIP: %s\n\tPort: %s\n", serverIP.c_str(), to_string(port).c_str());
        return false;
    }
}

bool DummyClient::disconnect(){
    ENetEvent event;
    enet_peer_disconnect(server, 0);

    while(enet_host_service(client, &event, 3000) > 0){
        switch(event.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                printf("Successfully disconnected from server\n\tIP: %s\nPort: %s", serverIP.c_str(), to_string(serverPort).c_str());
                return true;

        }
    }
}

void DummyClient::listenForPackets(){
    ENetEvent event;

    while(enet_host_service(client, &event, 0) > 0){

        switch(event.type){
            case ENET_EVENT_TYPE_CONNECT:
                printf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
                        event.packet -> dataLength,
                        event.packet -> data,
                        event.peer -> data,
                        event.channelID);
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                printf ("%s disconnected.\n", event.peer -> data);
        }
    }
}

bool DummyClient::sendPacket(string message){
    char* messageChar = new char[message.length()+1]; // convert string IP to char * used in enet set host ip
    strcpy(messageChar, message.c_str());
    ENetPacket *packet = enet_packet_create(messageChar, strlen(messageChar)+1, ENET_PACKET_FLAG_RELIABLE);

    enet_peer_send(server, 0, packet);
    enet_host_flush(client);
}