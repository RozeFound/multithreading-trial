#pragma once

#include <array>
#include <stdexcept>


template <typename T, std::size_t N>
class circular_buffer {

public:

    circular_buffer() = default;
    circular_buffer (const circular_buffer&) = delete;
    circular_buffer (circular_buffer&&) = delete;

    constexpr const std::size_t size() const;
    constexpr const std::size_t capacity() const { return N; }
    constexpr const bool empty() const { return head == tail; }
    constexpr const bool full() const { return size() == N; }

    void push (const T& value);
    void pop();

    T& front();

private:    

    std::array<T, N> buffer;
    std::size_t head = 0;
    std::size_t tail = 0;

};

template <typename T, std::size_t N>
constexpr const std::size_t circular_buffer<T, N>::size() const {

    if (head >= tail) {
        return head - tail;
    }
    return N - (tail - head);

}

template <typename T, std::size_t N>
void circular_buffer<T, N>::push (const T& value) {

    if (full())
        throw std::runtime_error("Circular buffer is full");

    buffer[head] = value;
    head = (head + 1) % N;

}

template <typename T, std::size_t N>
void circular_buffer<T, N>::pop() {

    if (empty())
        throw std::runtime_error("Circular buffer is empty");

    tail = (tail + 1) % N;

}

template <typename T, std::size_t N>
T& circular_buffer<T, N>::front() {

    if (empty())
        throw std::runtime_error("Circular buffer is empty");

    return buffer[tail];

}