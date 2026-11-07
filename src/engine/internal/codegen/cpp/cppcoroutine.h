// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <coroutine>
#include <exception>
#include <utility>

namespace libscratchcpp
{

/*! Return type of generated scripts and procedures. The returned value of co_return is the end thread flag. */
class CppCoroutine
{
    public:
        struct promise_type
        {
                CppCoroutine get_return_object() { return CppCoroutine(std::coroutine_handle<promise_type>::from_promise(*this)); }
                std::suspend_never initial_suspend() noexcept { return {}; }
                std::suspend_always final_suspend() noexcept { return {}; }
                void return_value(bool endThread) { this->endThread = endThread; }
                void unhandled_exception() { std::terminate(); }

                bool endThread = false;
        };

        using Handle = std::coroutine_handle<promise_type>;

        CppCoroutine() = default;

        /*! Creates a finished coroutine (used by code which doesn't need to be suspended). */
        explicit CppCoroutine(bool endThread) :
            m_endThread(endThread)
        {
        }

        CppCoroutine(const CppCoroutine &) = delete;

        CppCoroutine(CppCoroutine &&other) noexcept :
            m_handle(std::exchange(other.m_handle, nullptr)),
            m_endThread(other.m_endThread)
        {
        }

        ~CppCoroutine() { destroy(); }

        CppCoroutine &operator=(const CppCoroutine &) = delete;

        CppCoroutine &operator=(CppCoroutine &&other) noexcept
        {
            if (this != &other) {
                destroy();
                m_handle = std::exchange(other.m_handle, nullptr);
                m_endThread = other.m_endThread;
            }

            return *this;
        }

        /*! Returns true if the coroutine is finished or there isn't any coroutine. */
        bool done() const { return !m_handle || m_handle.done(); }

        /*! Resumes the coroutine and returns true if it's finished. */
        bool resume()
        {
            if (!done())
                m_handle.resume();

            return done();
        }

        /*! Returns true if the coroutine finished with a thread stop. */
        bool endThread() const { return m_handle ? (m_handle.done() && m_handle.promise().endThread) : m_endThread; }

        /*! Destroys the coroutine frame. */
        void destroy()
        {
            if (m_handle) {
                m_handle.destroy();
                m_handle = nullptr;
            }
        }

    private:
        explicit CppCoroutine(Handle handle) :
            m_handle(handle)
        {
        }

        Handle m_handle = nullptr;
        bool m_endThread = false;
};

} // namespace libscratchcpp
