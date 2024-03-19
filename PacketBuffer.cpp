#include "PacketBuffer.h"

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
        printf("Packet received in Receive Buffer.\n\tpacketType = %zu\n\tPayload = %s\n", GetOD_Packet(packetQueue.front()->data)->packet_type(),
               GetOD_Packet(packetQueue.front()->data)->payload());
    }

    buffer_Condition.notify_one();
}
unique_ptr<ENetPacket> PacketBuffer::removePacketInstant() {
    unique_lock<mutex> lock(bufferMutex); // lock the buffer

    // enter wait state and unlock lock until the packetQueue is notified, then check if it satisfies the lambda function if not
    // go back to waiting. This approach prevents random wakeups as even if it is woken up randomly it will not proceed unless it
    // can

    if(packetQueue.empty() || shutdownFlag.load()){
        cout << "empty or shutdown in packet buffer instant method" << endl;
        return nullptr;
    }
    auto packet = std::move(packetQueue.front()); // pull out the packet
    packetQueue.pop();
    numberOfPackets--;
    return packet;
}
/*
 * In removePacket() a lock based on the bufferMutex mutex is acquired, once acquired if the queue is empty the thread enters a wait
 * state based upon the buffer_Condition condition variable and it gives up the lock. This condition variable is passed the lock. Thus when this thread is
 * notified to wake up by cv.notify_one() or cv.notify_all() it will wake up and try to reacquire the lock (handled by the wait funciton)
 * , and once it reacquires the lock it will begin the service the request.
 */
vector<unique_ptr<ENetPacket>>* PacketBuffer::removePacketWait() {
    unique_lock<mutex> lock(bufferMutex); // lock the buffer

    // enter wait state and unlock lock until the packetQueue is notified, then check if it satisfies the lambda function if not
    // go back to waiting. This approach prevents random wakeups as even if it is woken up randomly it will not proceed unless it
    // can
    buffer_Condition.wait(lock, [this] {
        return (!packetQueue.empty() || shutdownFlag.load());
    });

    if(packetQueue.empty() && shutdownFlag.load()){
        cout << "Packet Buffer is in shutDown. " << shutdownFlag.load() << endl << "All existing packets have been serviced." << endl;
        return nullptr; //
    }
    vector<unique_ptr<ENetPacket>>* packetList;
    for(int i = 0; i < packetQueue.size(); i++){
        auto packet = std::move(packetQueue.front()); // pull out the packet
        packetQueue.pop();
        packetList->push_back(std::move(packet));
        numberOfPackets--;
    }

    return packetList;
}

void PacketBuffer::notifyAll() {
    buffer_Condition.notify_all();
}

void PacketBuffer::shutdown() {
    shutdownFlag.store(true);
    buffer_Condition.notify_all(); // wake all threads waiting of this shutdown command
}