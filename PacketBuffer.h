//
// Created by Joseph on 3/18/2024.
//

#ifndef FPS_GAME_PACKETBUFFER_H
#define FPS_GAME_PACKETBUFFER_H


#include <enet/enet.h>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <string>
#include <mutex>
#include <queue>
#include <memory>
#include "BufferHandler.h"
// Needed for manual building
#include <functional>
#include <atomic>
#include "game_state_generated.h"


using namespace std;

/*
 * The PacketBuffer acts as an interface between the network "gateway" and the slave threads that will process the packets from the "gateway". Now that we have
 *  seperated the Game Server from the Gateway. The gateway will not receive real-time packets, so there is no need for a priority queue
 *  implementation.
 *
 *  Unique_ptr Use:
 *  In this multithreaded context where Packets are being passed between components, ownership becomes obscure, but it is important for memory
 *  deallocation and performance that ownership is known. Thus Packets are referenced to using unique_ptrs which allows for owner transfership
 *  and unique_ptrs automatically handle memory deallocation and also avoids the case where pass by value would be used to stop memory leaks.

 *
 *  //TODO: Performance Considerations:         Currently the buffer accomplishes thread-safe operations using mutexes and locking mechniams, simplifying it to atomic operations would make this more efficeint but that is notoriously difficult
 *
 *
 */



using SubscriberCallback = function<void (const ENetPacket*)>;

class PacketBuffer {
public:
    PacketBuffer(mutex& consoleMutex);
//        ~PacketBuffer();

    // delete the copy constructor and copy assignment operator
    // This is because in our multi-threaded context the Packet Buffer is shared between components and it has unique ownership
    // and responsibility of its contents and copying means that it must share ownership. Since our Packet Buffer's queue is made
    // up of unique_ptrs copying it would lead to compile time errors
    PacketBuffer(const PacketBuffer&) = delete;
    PacketBuffer& operator=(const PacketBuffer&) = delete;

    // add to the buffer, (acquire the lock and add to end)
    void addPacket(unique_ptr<ENetPacket> packet);
    void addBufferHandler(unique_ptr<BufferHandler> packet);
    // remove from the buffer, wait until not empty or shutdown & acquire the lock and pop from front
    vector<unique_ptr<ENetPacket>> removePacketWait();
    vector<unique_ptr<BufferHandler>> removePacketInstant();
    // wake all waiting threads
    void notifyAll();

    // TODO: get buffer stats, (% full, service rate, how often full, etc)

    // initiate shutdown event
    void shutdown();

private:
    mutex& consoleMutex;
    int numberOfPackets;
    atomic<bool> shutdownFlag;
    mutex bufferMutex;
    condition_variable buffer_Condition;
    queue<unique_ptr<ENetPacket>> packetQueue;
    queue<unique_ptr<BufferHandler>> packetQueueIn;
};

#endif