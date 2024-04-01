#include "PacketBuffer.h"

using namespace std;

PacketBuffer::PacketBuffer(mutex& consoleMutex):consoleMutex(consoleMutex),
        shutdownFlag(false), numberOfPackets(0) {}

void PacketBuffer::addPacket(unique_ptr<ENetPacket> packet) {
    {
        unique_lock<mutex> lock(bufferMutex);

        if(shutdownFlag){
            {
                std::lock_guard<std::mutex> guard(consoleMutex);
                cout << "Packet Buffer is in shutdown. Packets cannot be queued for service in shutdown.\n" << endl;


            }
            // TODO: deal with packets already in the system. Don't want data loss
            return;
        }
        packetQueue.push(std::move(packet));
        numberOfPackets++;
//        {
//            std::lock_guard<std::mutex> guard(consoleMutex);
//            printf("Packet received in Output Buffer.\n\ttick = %zu\n", GetOD_Packet(packetQueue.back()->data)->tick()->tick_number());
//
//        }
    }

    buffer_Condition.notify_all();

}
void PacketBuffer::addBufferHandler(unique_ptr<BufferHandler> packet) {
    {
        unique_lock<mutex> lock(bufferMutex);

        if(shutdownFlag){
            {
                std::lock_guard<std::mutex> guard(consoleMutex);
                cout << "Input Buffer is in shutdown. Packets cannot be queued for service in shutdown.\n" << endl;


            }
            // TODO: deal with packets already in the system. Don't want data loss
            return;
        }
        packetQueueIn.push(std::move(packet));
        numberOfPackets++;
//        {
//            std::lock_guard<std::mutex> guard(consoleMutex);
//            printf("Packet received in Input Buffer.\n\ttick = %zu\n", packetQueueIn.back()->getPacketView()->tick()->tick_number());
//
//        }
    }

    buffer_Condition.notify_all();

}
vector<unique_ptr<BufferHandler>> PacketBuffer::removePacketInstant() {
    unique_lock<mutex> lock(bufferMutex); // lock the buffer

    // enter wait state and unlock lock until the packetQueue is notified, then check if it satisfies the lambda function if not
    // go back to waiting. This approach prevents random wakeups as even if it is woken up randomly it will not proceed unless it
    // can
    buffer_Condition.wait(lock, [this] {
        return (!packetQueue.empty() || shutdownFlag.load());
    });
    if(packetQueueIn.empty() && shutdownFlag.load()){

        cout << "empty or shutdown in packet buffer instant method" << endl;
        vector<std::unique_ptr<BufferHandler>> temp2;
        return temp2;

    }
    vector<unique_ptr<BufferHandler>> packetList;
    packetList.reserve(packetQueueIn.size());
    while(!packetQueueIn.empty()){
        auto packet = std::move(packetQueueIn.front()); // pull out the packet
//        {
//            std::lock_guard<std::mutex> guard(consoleMutex);
//            cout <<"Number of packets(pulling from buffer in transmitter): " <<numberOfPackets << endl;
//            cout <<"Tick no. in transmitter: " <<packet->getPacketView()->tick()->tick_number() << endl;
//        }
        packetQueueIn.pop();
        packetList.push_back(std::move(packet));
        numberOfPackets--;

    }
//    cout << numberOfPackets << endl;

    return std::move(packetList);
}
/*
 * In removePacket() a lock based on the bufferMutex mutex is acquired, once acquired if the queue is empty the thread enters a wait
 * state based upon the buffer_Condition condition variable and it gives up the lock. This condition variable is passed the lock. Thus when this thread is
 * notified to wake up by cv.notify_one() or cv.notify_all() it will wake up and try to reacquire the lock (handled by the wait funciton)
 * , and once it reacquires the lock it will begin the service the request.
 */
vector<unique_ptr<ENetPacket>> PacketBuffer::removePacketWait() {
    unique_lock<mutex> lock(bufferMutex); // lock the buffer
//    {
//        std::lock_guard<std::mutex> guard(consoleMutex);
//        cout << "transmitter is attempting to pull from buffer" << endl;
//
//
//    }
    // enter wait state and unlock lock until the packetQueue is notified, then check if it satisfies the lambda function if not
    // go back to waiting. This approach prevents random wakeups as even if it is woken up randomly it will not proceed unless it
    // can
    buffer_Condition.wait(lock, [this] {
        return (!packetQueue.empty() || shutdownFlag.load());
    });
//    {
//        std::lock_guard<std::mutex> guard(consoleMutex);
//        cout << "transmitter got the green light" << endl;
//
//    }
    if(packetQueue.empty() || shutdownFlag.load()){
        {
            std::lock_guard<std::mutex> guard(consoleMutex);
            cout << "Packet Buffer is in shutDown. " << shutdownFlag.load() << endl << "All existing packets have been serviced." << endl;

        }
        vector<std::unique_ptr<ENetPacket>> temp2;
        return temp2; //
    }
//    cout << numberOfPackets << endl;
    vector<unique_ptr<ENetPacket>> packetList;
    packetList.reserve(packetQueue.size());

    while(!packetQueue.empty()){
        auto packet = std::move(packetQueue.front()); // pull out the packet
//        {
//            std::lock_guard<std::mutex> guard(consoleMutex);
//            cout <<"Number of packets(pulling from buffer in transmitter): " <<numberOfPackets << endl;
//            cout <<"Tick no. in transmitter: " <<GetOD_Packet(packet->data)->tick()->tick_number() << endl;
//        }
        packetQueue.pop();
        packetList.push_back(std::move(packet));
        numberOfPackets--;

    }
//    cout << numberOfPackets << endl;

    return std::move(packetList);
}

void PacketBuffer::notifyAll() {
    buffer_Condition.notify_all();
}

void PacketBuffer::shutdown() {
    shutdownFlag.store(true);
    buffer_Condition.notify_all(); // wake all threads waiting of this shutdown command
}