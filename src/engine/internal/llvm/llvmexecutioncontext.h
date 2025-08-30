// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/executioncontext.h>

#include "llvmcompilercontext.h"

namespace libscratchcpp
{

struct StringPtr;

class LLVMExecutionContext : public ExecutionContext
{
    public:
        LLVMExecutionContext(LLVMCompilerContext *compilerCtx, Thread *thread);
        ~LLVMExecutionContext();

        void *coroutineHandle() const;
        void setCoroutineHandle(void *newCoroutineHandle);

        bool finished() const;
        void setFinished(bool newFinished);

        inline StringPtr **getStringArray(function_id_t functionId)
        {
            assert(m_stringArrays.find(functionId) != m_stringArrays.cend());
            return m_stringArrays[functionId];
        }

    private:
        LLVMCompilerContext *m_compilerCtx = nullptr;
        void *m_coroutineHandle = nullptr;
        bool m_finished = false;

        std::unordered_map<function_id_t, std::vector<StringPtr *>> m_stringVectors;
        std::unordered_map<function_id_t, StringPtr **> m_stringArrays;
};

} // namespace libscratchcpp
