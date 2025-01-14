// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>

#include "compiler_p.h"

#include "internal/codebuilderfactory.h"
#include "internal/icodebuilder.h"

using namespace libscratchcpp;

CompilerPrivate::CompilerPrivate(CompilerContext *ctx) :
    ctx(ctx)
{
    assert(ctx);
    initBuilderFactory();
}

CompilerPrivate::CompilerPrivate(IEngine *engine, Target *target) :
    ctxPtr(Compiler::createContext(engine, target)),
    ctx(ctxPtr.get())
{
    initBuilderFactory();
}

void CompilerPrivate::initBuilderFactory()
{
    if (!builderFactory)
        builderFactory = CodeBuilderFactory::instance().get();
}

void CompilerPrivate::substackEnd()
{
    auto &parent = substackTree.back();

    switch (parent.second) {
        case SubstackType::Loop:
            // Yield at loop end if not running without screen refresh
            /*if (!warp)
                builder->yield();*/

            builder->endLoop();
            break;

        case SubstackType::IfStatement:
            if (parent.first.second) {
                builder->beginElseBranch();
                block = parent.first.second;
                parent.first.second = nullptr;
                return;
            } else
                builder->endIf();

            break;
    }

    auto parentBlock = parent.first.first;

    if (parentBlock)
        block = parentBlock->next();
    else
        block = nullptr;

    substackTree.pop_back();

    if (!block && !substackTree.empty())
        substackEnd();
}
