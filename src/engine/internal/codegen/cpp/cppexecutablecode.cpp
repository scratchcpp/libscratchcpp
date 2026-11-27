// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/valuedata.h>
#include <scratchcpp/promise.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/target.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/itimer.h>
#include <scratchcpp/scratchconfiguration.h>
#include <scratchcpp/igraphicseffect.h>
#include <cassert>
#include <utility>
#include <stdexcept>

#include "cppexecutablecode.h"
#include "cppcompilercontext.h"
#include "cppexecutioncontext.h"
#include "cppcodesaver.h"

using namespace libscratchcpp;

CppExecutableCode::CppExecutableCode(CppCompilerContext *ctx, std::vector<CppObjectPointerDescriptor> objectPointers, std::shared_ptr<const CppCodeSource> source) :
    m_ctx(ctx),
    m_source(std::move(source)),
    m_objectPointerDescriptors(std::move(objectPointers))
{
    assert(m_ctx);

    if (m_ctx && !m_objectPointerDescriptors.empty())
        getObjectPointers(m_ctx->engine());
}

void CppExecutableCode::save(const std::string &destination) const
{
    if (!m_source)
        throw std::runtime_error("C++ export artifacts are not available for this executable");
    CppCodeSaver::save(*m_source, m_ctx, destination);
}

void CppExecutableCode::run(ExecutionContext *context)
{
    CppExecutionContext *ctx = getContext(context);

    if (ctx->finished())
        return;

    auto promise = ctx->promise();

    if (promise) {
        if (promise->isResolved())
            ctx->setPromise(nullptr);
        else
            return;
    }

    // TODO: Add support for coroutines
    // TODO: Add support for end thread sentinel value
    Target *target = ctx->thread()->target();
    runScript(context, target, target->variableData(), target->listData(), getObjectPointers(context->engine()));
    ctx->setFinished(true);
}

ValueData CppExecutableCode::runReporter(ExecutionContext *context)
{
    Target *target = context->thread()->target();
    return runReporter(context, target, target->variableData(), target->listData(), getObjectPointers(context->engine()));
}

bool CppExecutableCode::runPredicate(ExecutionContext *context)
{
    Target *target = context->thread()->target();
    return runPredicate(context, target, target->variableData(), target->listData(), getObjectPointers(context->engine()));
}

void CppExecutableCode::kill(ExecutionContext *context)
{
    CppExecutionContext *ctx = getContext(context);
    // TODO: Add support for coroutines
    // NOTE: Do not destroy coroutine, it will be destroyed later
    // (really bad things happen when a coroutine destroys itself...)
    // The code is finished and the coroutine won't be used again until reset() is called
    ctx->setFinished(true);
    ctx->setPromise(nullptr);
}

void CppExecutableCode::reset(ExecutionContext *context)
{
    CppExecutionContext *ctx = getContext(context);

    // TODO: Add support for coroutines
    ctx->setFinished(false);
    ctx->setPromise(nullptr);
}

bool CppExecutableCode::isFinished(ExecutionContext *context) const
{
    return getContext(context)->finished();
}

std::shared_ptr<ExecutionContext> CppExecutableCode::createExecutionContext(Thread *thread) const
{
    return std::make_shared<CppExecutionContext>(m_ctx, thread);
}

void CppExecutableCode::runScript(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists, void **objects)
{
    // Generated code overrides this
}

ValueData CppExecutableCode::runReporter(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists, void **objects)
{
    // Generated code overrides this
    return ValueData();
}

bool CppExecutableCode::runPredicate(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists, void **objects)
{
    // Generated code overrides this
    return false;
}

void **CppExecutableCode::getObjectPointers(IEngine *engine) const
{
    if (m_objectPointerDescriptors.empty())
        return nullptr;

    std::lock_guard<std::mutex> lock(m_objectPointerMutex);
    auto it = m_objectPointersByEngine.find(engine);

    if (it == m_objectPointersByEngine.cend()) {
        std::vector<void *> pointers(m_objectPointerDescriptors.size(), nullptr);

        for (size_t i = 0; i < m_objectPointerDescriptors.size(); ++i) {
            const CppObjectPointerDescriptor &descriptor = m_objectPointerDescriptors[i];

            switch (descriptor.kind) {
                case CppObjectPointerDescriptor::Kind::Target: {
                    const int targetIndex = engine->findTarget(descriptor.key);

                    if (targetIndex >= 0)
                        pointers[i] = engine->targetAt(targetIndex);

                    break;
                }

                case CppObjectPointerDescriptor::Kind::Variable:
                case CppObjectPointerDescriptor::Kind::List: {
                    for (const auto &target : engine->targets()) {
                        if (descriptor.kind == CppObjectPointerDescriptor::Kind::Variable) {
                            const int index = target->findVariableById(descriptor.key);

                            if (index >= 0) {
                                pointers[i] = target->variableAt(index).get();
                                break;
                            }
                        } else {
                            const int index = target->findListById(descriptor.key);

                            if (index >= 0) {
                                pointers[i] = target->listAt(index).get();
                                break;
                            }
                        }
                    }

                    break;
                }

                case CppObjectPointerDescriptor::Kind::Timer:
                    pointers[i] = engine->timer();
                    break;

                case CppObjectPointerDescriptor::Kind::Effect:
                    pointers[i] = ScratchConfiguration::getGraphicsEffect(descriptor.key);
                    break;
            }
        }

        it = m_objectPointersByEngine.emplace(engine, std::move(pointers)).first;
    }

    return it->second.data();
}

CppExecutionContext *CppExecutableCode::getContext(ExecutionContext *context)
{
    assert(dynamic_cast<CppExecutionContext *>(context));
    return static_cast<CppExecutionContext *>(context);
}
