// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stack>
#include <vector>

#include "string_pool.h"
#include "spimpl.h"

namespace libscratchcpp
{

class Thread;
class IEngine;
class Promise;
class IStackTimer;
class IRandomGenerator;
struct StringPtr;
class ExecutionContextPrivate;

/*! \brief The ExecutionContext represents the execution context of a target (can be a clone) with variables, lists, etc. */
class LIBSCRATCHCPP_EXPORT ExecutionContext
{
    public:
        ExecutionContext(Thread *thread);
        ExecutionContext(const ExecutionContext &) = delete;
        virtual ~ExecutionContext();

        Thread *thread() const;
        IEngine *engine() const;

        std::shared_ptr<Promise> promise() const;
        void setPromise(std::shared_ptr<Promise> promise);

        IStackTimer *stackTimer() const;
        void setStackTimer(IStackTimer *newStackTimer);

        IRandomGenerator *rng() const;
        void setRng(IRandomGenerator *newRng);

        inline StringPtr **pushStringStack(size_t stringCount)
        {
            m_stringStack.push({});

            auto &strings = m_stringStack.top();

            for (size_t i = 0; i < stringCount; i++) {
                StringPtr *str = string_pool_new();
                strings.push_back(str);
            }

            return strings.data();
        }

        inline void popStringStack()
        {
            assert(!m_stringStack.empty());
            const auto &strings = m_stringStack.top();

            for (StringPtr *str : strings)
                string_pool_free(str);

            m_stringStack.pop();
        }

    private:
        spimpl::unique_impl_ptr<ExecutionContextPrivate> impl;

        std::stack<std::vector<StringPtr *>> m_stringStack;
};

} // namespace libscratchcpp
