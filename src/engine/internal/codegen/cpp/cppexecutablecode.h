// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/executablecode.h>

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "cppobjectpointer.h"
#include "test_export.h"

namespace libscratchcpp
{

class Target;
class List;
class IEngine;
class CppCompilerContext;
class CppExecutionContext;
struct CppCodeSource;

class LIBSCRATCHCPP_TEST_EXPORT CppExecutableCode : public ExecutableCode
{
    public:
        CppExecutableCode(CppCompilerContext *ctx, std::vector<CppObjectPointerDescriptor> objectPointers = {}, std::shared_ptr<const CppCodeSource> source = nullptr);

        void save(const std::string &destination) const;

        virtual void run(ExecutionContext *context) override final;
        virtual ValueData runReporter(ExecutionContext *context) override final;
        virtual bool runPredicate(ExecutionContext *context) override final;
        virtual void kill(ExecutionContext *context) override final;
        virtual void reset(ExecutionContext *context) override final;

        virtual bool isFinished(ExecutionContext *context) const override final;

        virtual std::shared_ptr<ExecutionContext> createExecutionContext(Thread *thread) const override final;

    protected:
        virtual void runScript(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists, void **objects);
        virtual ValueData runReporter(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists, void **objects);
        virtual bool runPredicate(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists, void **objects);

    private:
        static CppExecutionContext *getContext(ExecutionContext *context);
        void **getObjectPointers(IEngine *engine) const;

        CppCompilerContext *m_ctx = nullptr;
        std::shared_ptr<const CppCodeSource> m_source;
        std::vector<CppObjectPointerDescriptor> m_objectPointerDescriptors;
        mutable std::mutex m_objectPointerMutex;
        mutable std::unordered_map<IEngine *, std::vector<void *>> m_objectPointersByEngine;
};

} // namespace libscratchcpp
