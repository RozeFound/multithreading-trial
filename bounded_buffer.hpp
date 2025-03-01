#pragma once

#include <memory>
#include <mutex>
#include <condition_variable>

#include "circular_buffer.hpp"

template <typename T, std::size_t N>
class bounded_buffer {
public:

	std::size_t size() const;
	std::size_t capacity() const;

	void clear();

	// non-blocking operations
	bool try_push (const T&);
	std::unique_ptr<T> try_pop();
	bool try_pop(T&);
	void force_push (const T&);

	// blocking operations
	void push_wait (const T&);
	T& pop_wait();

	// timeout-based blocking operations
	bool push_wait_for (const T&, std::chrono::milliseconds = std::chrono::milliseconds::zero());
	std::unique_ptr<T> pop_wait_for (std::chrono::milliseconds = std::chrono::milliseconds::zero());
	bool pop_wait_for (T&, std::chrono::milliseconds = std::chrono::milliseconds::zero());


private:

	mutable std::mutex mutex;
	std::condition_variable has_element;
	std::condition_variable has_space;

	circular_buffer<T, N> queue;

};

template <typename T, std::size_t N>
std::size_t bounded_buffer<T, N>::size() const {
	std::lock_guard lock(mutex);
	return queue.size();
}

template <typename T, std::size_t N>
std::size_t bounded_buffer<T, N>::capacity() const {
	std::lock_guard lock(mutex);
	return queue.capacity();
}

template <typename T, std::size_t N>
void bounded_buffer<T, N>::clear() {
	std::lock_guard lock(mutex);
	while (!queue.empty())
		queue.pop();
}

template <typename T, std::size_t N>
 bool bounded_buffer<T, N>::try_push (const T& element) {

	 std::unique_lock lock(mutex);
	
	if (queue.size() == N)
		return false;

	queue.push(element);

	lock.unlock();
	has_element.notify_one();

	return true;

}

template <typename T, std::size_t N>
bool bounded_buffer<T, N>::try_pop (T& element) {

	std::unique_lock lock(mutex);

	if (queue.empty())
		return false;

	element = queue.front();

	queue.pop();

	lock.unlock();
	has_space.notify_one();

	return true;

}

template <typename T, std::size_t N>
std::unique_ptr<T> bounded_buffer<T, N>::try_pop () {

	std::lock_guard lock(mutex);

	auto element = T();

	if (try_pop(element))
		return std::make_unique<T, N>(std::move(element));

	return nullptr;

}

template <typename T, std::size_t N>
void bounded_buffer<T, N>::force_push (const T& element) {

	std::unique_lock lock(mutex);

	if (queue.size() == N)
		queue.pop();

	queue.push(element);

	lock.unlock();
	has_element.notify_one();

}

template <typename T, std::size_t N>
void bounded_buffer<T, N>::push_wait (const T& element) {

	std::unique_lock lock(mutex);

	has_space.wait(lock, [this] { return !queue.size() != N; });

	queue.push(element);

	lock.unlock();
	has_element.notify_one();

}

template <typename T, std::size_t N>
T& bounded_buffer<T, N>::pop_wait() {

	std::unique_lock lock(mutex);

	has_element.wait(lock, [this] { return !queue.empty(); });

	auto& element = queue.front();

	queue.pop();


	lock.unlock();
	has_space.notify_one();

	return element;

}

template <typename T, std::size_t N>
bool bounded_buffer<T, N>::push_wait_for (const T& element, std::chrono::milliseconds timeout) {

	std::unique_lock lock(mutex);

	bool success = false;

	if (has_space.wait_for(lock, timeout, [this] { return !queue.size() != N; })) {
		queue.push(element);
		success = true;
	}

	lock.unlock();
	has_element.notify_one();

	return success;

}

template <typename T, std::size_t N>
bool bounded_buffer<T, N>::pop_wait_for (T& element, std::chrono::milliseconds timeout) {

	std::unique_lock lock(mutex);

	if (has_element.wait_for(lock, timeout, [this] { return !queue.empty(); })) {
		element = queue.front();
		queue.pop();
	} else return false;

	lock.unlock();
	has_space.notify_one();

	return true;

}

template <typename T, std::size_t N>
std::unique_ptr<T> bounded_buffer<T, N>::pop_wait_for (std::chrono::milliseconds timeout) {

	std::lock_guard lock(mutex);

	auto element = T();

	if (pop_wait_for(element, timeout))
		return std::make_unique<T, N>(std::move(element));

	return nullptr;

}