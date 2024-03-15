//
// Created by Joseph on 3/15/2024.
//

#include "Executor.h"

template<typename T>
Executor<T>::~Executor() {
    stop();
}

template<typename T>
void Executor<T>::start() {
    stopFlag.store(false);
    worker = thread(&Executor<T>::run,this);
}

template<typename T>
void Executor<T>::stop() {
    stopFlag.store(true);
}

template<typename T>
void Executor<T>::run() {
    while(!stopFlag.load()){
        //handle input/output thread functionality
    }
}

template<typename T>
Executor<T>::Executor() {}

