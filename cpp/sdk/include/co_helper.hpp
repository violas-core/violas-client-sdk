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

            return std::move(value);
        }

        // auto await_transform(promise_type && p) noexcept
        // {
        //     struct awaiter : std::suspend_never
        //     {
        //         promise_type&& _p;
        //         auto await_resume() const noexcept
        //         {
        //             return _p.result();
        //         }
        //     };

        //     return awaiter{ p };
        // }
        // template <typename U>
        // U &&await_transform(U &&awaitable) noexcept
        // {
        //     return static_cast<U &&>(awaitable);
        // }
    };

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
        bool _is_ready = false;
        std::exception_ptr _exception;
        std::string _txt;

        Task<void> get_return_object()
        {
            return Task{std::move(co_handle::from_promise(*this))};
        }

        std::suspend_never initial_suspend() { return {}; }

        auto final_suspend() noexcept
        {
            _is_ready = true;

            if (_prev)
                _prev.resume();

            return std::suspend_never();
        }

        void return_void()
        {
            if (_exception)
                std::rethrow_exception(_exception);
        }

        void unhandled_exception()
        {
            _exception = std::current_exception();
            _txt = "hello";
        }
    };

    //
    // Chain calling
    //
    auto operator co_await()
    {
        struct Awaitor
        {
            co_handle &handle;

            bool await_ready() const noexcept { return handle.promise()._is_ready; }
            void await_resume() const
            {
                handle.promise().return_void();
                // std::move(task._handle.promise()).return_void();
            }

            void await_suspend(std::coroutine_handle<> h)
            {
                handle.promise()._prev = h;
            }
        };

        return Awaitor{_handle};
    }

    // Task(const Task &) = delete;
    // Task &operator=(const Task &) = delete;
};