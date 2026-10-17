// SPDX-License-Identifier: Apache-2.0

#include "cppexecutioncontext.h"
#include "cppprelude.h"

using namespace libscratchcpp;

CppExecutionContext::CppExecutionContext(Thread *thread) :
    ExecutionContext(thread)
{
}

CppExecutionContext::~CppExecutionContext()
{
    if (m_frame)
        cpp::destroy(m_frame);
}

/*! Returns the frame of the suspended script (or nullptr). */
void *CppExecutionContext::frame() const
{
    return m_frame;
}

void CppExecutionContext::setFrame(void *newFrame)
{
    m_frame = newFrame;
}

bool CppExecutionContext::finished() const
{
    return m_finished;
}

void CppExecutionContext::setFinished(bool newFinished)
{
    m_finished = newFinished;
}
