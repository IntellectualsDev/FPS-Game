//
// Created by Joseph on 3/15/2024.
//

#ifndef FPS_GAME_CIRCULARBUFFER_H
#define FPS_GAME_CIRCULARBUFFER_H
#include <memory>
template <typename T>
class CircularBuffer {
private:
    std::unique_ptr<T[]> buffer;
    size_t head = 0;
    size_t tail = 0;
    size_t max_size;
    T empty_item;
public:
    explicit CircularBuffer<T>(size_t max_size)
    : buffer(std::unique_ptr<T[]>(new T[max_size])), max_size(max_size){}

    CircularBuffer();;
    void enqueue(T item);
    T dequeue();
    T front();
    bool isFull();
    bool isEmpty();
    size_t size();

};




#endif //FPS_GAME_CIRCULARBUFFER_H
