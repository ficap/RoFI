#pragma once

#include <condition_variable>  // std::contidion_variable
#include <cstddef>  // std::size_t
#include <mutex>  // std::mutex
#include <queue>  // std::queue
#include <utility>  // std::forward


template < typename T, std::size_t CAPACITY = 8 >
class LimitedQueue {
public:
    void enqueue( T&& t ) {
        std::lock_guard< std::mutex > lock( _mtx );
        if ( _queue.size() >= CAPACITY ) {
            _queue.pop();
        }
        _queue.push( std::forward< T >( t ) );
        _cv.notify_one();
    }

    template<typename... Args>
    void emplace( Args&&... args ) {
        std::lock_guard< std::mutex > lock( _mtx );
        if ( _queue.size() >= CAPACITY ) {
            _queue.pop();
        }
        _queue.emplace( std::forward< Args >( args )... );
        _cv.notify_one();
    }

    T dequeue() {
        std::unique_lock< std::mutex > lock( _mtx );
        while( _queue.empty() ) {
            _cv.wait( lock );
        }

        T val = std::move( _queue.front() );
        _queue.pop();

        return val;
    }

    std::size_t size() {
        std::lock_guard< std::mutex > lock( _mtx );

        return _queue.size();
    }

private:
    std::queue< T > _queue{};
    mutable std::mutex _mtx{};
    std::condition_variable _cv{};
};
