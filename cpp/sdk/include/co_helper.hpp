#pragma once

#include <coroutine>
//
//  Task class for C++ 20 coroutine
//
template <typename T>
struct Task
{
    struct promise_type;
    using co_handle = std::coroutine_handle<promise_type>;
    co_handle _handle;

    struct promise_type
    {
        T value;
        std::coroutine_handle<> _prev = nullptr; // previous promise
        std::exception_ptr _exception = nullptr;

        Task get_return_object() { return Task{co_handle::from_promise(*this)}; }

        std::suspend_never initial_suspend() { return {}; }

        std::suspend_never final_suspend() noexcept
        {
            if (_prev)
                _prev.resume();

            return {};
        }

        void unhandled_exception()
        {
            _exception = std::current_exception();
        }

        std::suspend_never return_value(T v)
        {
            value = v;
            return {};
        }

        T result()
        {
            if (_exception)
                std::rethrow_exception(_exception);
            
            return value;
        }
    };    

    //
    //  Calling co_await by chain
    //
    auto operator co_await()
    {
        struct Awaitor
        {
            Task &task;

            bool await_ready() const noexcept { return false; }
            
            T await_resume() const
            {
                return task._handle.promise().result();                
            }

            // template <typename PROMISE>
            void await_suspend(std::coroutine_handle<> h)
            {
                task._handle.promise()._prev = h;
            }
        };

        return Awaitor{*this};
    }
};

template <>
struct Task<void>
{
    struct promise_type;
    using co_handle = std::coroutine_handle<promise_type>;
    co_handle _handle;

    struct promise_type
    {
        std::coroutine_handle<> _prev = nullptr; // previous promise
        std::exception_ptr _exception = nullptr;

        Task<void> get_return_object()
        {
            return Task{co_handle::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept
        {
            if (_prev)
                _prev.resume();

            return {};
        }
        
        void return_void() {}
        
        void unhandled_exception()
        {
            _exception = std::current_exception();
        }

        void result()
        {
            if (_exception)
                std::rethrow_exception(_exception);
        }
    };

    //
    // Chain calling
    //
    auto operator co_await()
    {
        struct Awaitor
        {
            Task<void> &task;

            bool await_ready() const noexcept { return false; }
            void await_resume() const
            {
                task._handle.promise().result();
            }

            void await_suspend(std::coroutine_handle<> h)
            {
                task._handle.promise()._prev = h;
            }
        };

        return Awaitor{*this};
    }
};