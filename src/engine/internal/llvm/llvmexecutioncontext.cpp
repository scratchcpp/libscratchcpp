// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/string_pool.h>

#include "llvmexecutioncontext.h"
#include "llvmexecutablecode.h"

using namespace libscratchcpp;

LLVMExecutionContext::LLVMExecutionContext(LLVMCompilerContext *compilerCtx, Thread *thread) :
    ExecutionContext(thread),
    m_compilerCtx(compilerCtx)
{
    // Allocate strings for all scripts in the target
    // TODO: Allocate strings for this script and procedures it calls
    const auto &codeMap = compilerCtx->codeMap();

    for (const auto &[functionId, code] : codeMap) {
        size_t count = code->stringCount();

        if (count > 0) {
            m_stringVectors[functionId] = {};

            auto &strings = m_stringVectors[functionId];
            strings.reserve(count);

            for (size_t i = 0; i < count; i++)
                strings.push_back(string_pool_new());

            m_stringArrays[functionId] = strings.data();
        }
    }
}

LLVMExecutionContext::~LLVMExecutionContext()
{
    if (m_coroutineHandle) {
        if (m_compilerCtx) {
            m_compilerCtx->destroyCoroutine(m_coroutineHandle);
            m_coroutineHandle = nullptr;
        } else
            assert(false);
    }

    // Deallocate strings
    for (const auto &[functionId, strings] : m_stringVectors) {
        for (StringPtr *str : strings)
            string_pool_free(str);
    }
}

void *LLVMExecutionContext::coroutineHandle() const
{
    return m_coroutineHandle;
}

void LLVMExecutionContext::setCoroutineHandle(void *newCoroutineHandle)
{
    m_coroutineHandle = newCoroutineHandle;
}

bool LLVMExecutionContext::finished() const
{
    return m_finished;
}

void LLVMExecutionContext::setFinished(bool newFinished)
{
    m_finished = newFinished;
}
