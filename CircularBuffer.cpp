//
// Created by Joseph on 3/15/2024.
//

#include "CircularBuffer.h"
template<typename T>
void CircularBuffer<T>::enqueue(T item) {
    if(isFull()) throw std::runtime_error("Buffer is full cannot enqueue!");
    buffer[tail] = item;
    tail = (tail+1) % max_size;

}

template<typename T>
T CircularBuffer<T>::dequeue() {
    if(isEmpty()) throw std::runtime_error("Buffer is empty cannot dequeue");
    T item = buffer[head];
    buffer[head] = empty_item;
    head=(head+1) %max_size;
    return item;
}

template<typename T>
T CircularBuffer<T>::front() {
    return buffer[head];
}

template<typename T>
size_t CircularBuffer<T>::size() {
    if(tail >= head){
        return tail-head;
    }
    return max_size-head -tail;
}

template<typename T>
bool CircularBuffer<T>::isEmpty() {
    return head == tail;
}

template<typename T>
bool CircularBuffer<T>::isFull() {
    return tail == (head-1)%max_size;
}

template<typename T>
CircularBuffer<T>::CircularBuffer() {}
