//
// Created by Joseph on 3/15/2024.
//

#ifndef FPS_GAME_EXECUTOR_H
#define FPS_GAME_EXECUTOR_H

#include <memory>
#include <thread>
#include <iostream>
#include <atomic>
#include "CircularBuffer.h"

template <typename T >
class Executor{
public:
    Executor(CircularBuffer<T>& temp,bool type){
        buffer = temp;
        this->type = type;//0 for output 1 for input
    }

    Executor();

    ~Executor();
    void start();
    void stop();
private:
    std::atomic<bool> stopFlag{};
    CircularBuffer<T> buffer;
    std::thread worker;
    bool type{};
    //server info
    //UDP socket
    void run();

};



#endif //FPS_GAME_EXECUTOR_H
