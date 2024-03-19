//
// Created by Joseph on 3/19/2024.
//

#ifndef FPS_GAME_BUFFERHANDLER_H
#define FPS_GAME_BUFFERHANDLER_H
#include <memory>
#include "game_state_generated.h"

class BufferHandler {
public:
    BufferHandler(std::unique_ptr<uint8_t[]> data, size_t size): data(std::move(data)), size(size) {};

    const OD_Packet* getPacketView() const {
        return GetOD_Packet(data.get());
    }


private:
    std::unique_ptr<uint8_t[]> data;
    size_t size;
};

#endif //FPS_GAME_BUFFERHANDLER_H
