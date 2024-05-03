//
// Created by Anshul Gowda on 2/5/24.
//

#include "PacketBuffer.h"
#include "game_state_generated.h"

using namespace std;

PacketBuffer::PacketBuffer():
        shutdownFlag(false), numberOfPackets(0) {}

void PacketBuffer::addPacket(unique_ptr<ENetPacket> packet) {
    {
        unique_lock<mutex> lock(bufferMutex);

        if(shutdownFlag){
            cout << "Packet Buffer is in shutdown. Packets cannot be queued for service in shutdown.\n" << endl;
            // TODO: deal with packets already in the system. Don't want data loss
            return;
        }
        packetQueue.push(std::move(packet));
        numberOfPackets++;

        const auto* od_Packet = flatbuffers::GetRoot<OD_Packet>(packetQueue.front().get()->data);
//        cout << "Packet received in Buffer with Packet Type: " << EnumNamePacketType(od_Packet->packet_type()) << endl;
        while(packetQueue.empty()){

        }
//        printf("Packet received in Receive Buffer.\n\tlength = %zu\n\tdata = %s\n", packetQueue.front()->packet->ge->dataLength, packetQueue.front()->packet->data);
    }

    buffer_Condition.notify_one();
}

/*
 * In removePacket() a lock based on the bufferMutex mutex is acquired, once acquired if the queue is empty the thread enters a wait
 * state based upon the buffer_Condition condition variable and it gives up the lock. This condition variable is passed the lock. Thus when this thread is
 * notified to wake up by cv.notify_one() or cv.notify_all() it will wake up and try to reacquire the lock (handled by the wait funciton)
 * , and once it reacquires the lock it will begin the service the request.
 */
unique_ptr<ENetPacket> PacketBuffer::removePacket() {
    unique_lock<mutex> lock(bufferMutex); // lock the buffer

    // enter wait state and unlock lock until the packetQueue is notified, then check if it satisfies the lambda function if not
    // go back to waiting. This approach prevents random wakeups as even if it is woken up randomly it will not proceed unless it
    // can
//    buffer_Condition.wait(lock, [this] {
//        return (!packetQueue.empty() || shutdownFlag.load());
//    });

    buffer_Condition.wait(lock, [this] {
        return ((numberOfPackets > 0) || shutdownFlag.load());
    });
//    cout << "In Add Packet: " << numberOfPackets << ", " << shutdownFlag.load() << ", " << packetQueue.empty() << endl;

    if(packetQueue.empty() && shutdownFlag.load()){
        cout << "Packet Buffer is in shutDown. " << shutdownFlag.load() << endl << "All existing packets have been serviced." << endl;
        return nullptr; //
    }

    auto packet = std::move(packetQueue.front()); // pull out the packet
    if(packetQueue.front() != nullptr){
//        cout << "before pop packetqueue.front is not nulltpr" << endl;
    }
    packetQueue.pop();
    if(packetQueue.front() != nullptr){
//        cout << "after pop packetqueue.front is not nulltpr" << endl;
//        cout << packetQueue.size() << endl;
    }
    numberOfPackets--;
    if(packet == nullptr){
        cout << "FUCK " << endl;
    }
    return packet;
}

void PacketBuffer::notifyAll() {
    buffer_Condition.notify_all();
}

void PacketBuffer::shutdown() {
    shutdownFlag.store(true);
    buffer_Condition.notify_all(); // wake all threads waiting of this shutdown command
}