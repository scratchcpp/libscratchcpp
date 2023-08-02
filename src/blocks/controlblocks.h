// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>
#include <chrono>
#include "engine/compiler.h"

namespace libscratchcpp
{

/*! \brief The ControlBlocks class contains the implementation of control blocks. */
class LIBSCRATCHCPP_EXPORT ControlBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            SUBSTACK,
            SUBSTACK2,
            TIMES,
            CONDITION,
            DURATION,
            VALUE
        };

        enum Fields
        {
            STOP_OPTION,
            VARIABLE
        };

        enum FieldValues
        {
            StopAll,
            StopThisScript,
            StopOtherScriptsInSprite
        };

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileRepeatForever(Compiler *compiler);
        static void compileRepeat(Compiler *compiler);
        static void compileRepeatUntil(Compiler *compiler);
        static void compileRepeatWhile(Compiler *compiler);
        static void compileRepeatForEach(Compiler *compiler);
        static void compileIfStatement(Compiler *compiler);
        static void compileIfElseStatement(Compiler *compiler);
        static void compileStop(Compiler *compiler);
        static void compileWait(Compiler *compiler);
        static void compileWaitUntil(Compiler *compiler);

    private:
        static unsigned int stopAll(VirtualMachine *vm);
        static unsigned int stopOtherScriptsInSprite(VirtualMachine *vm);
        static unsigned int startWait(VirtualMachine *vm);
        static unsigned int wait(VirtualMachine *vm);
        static unsigned int waitUntil(VirtualMachine *vm);

        static inline std::unordered_map<VirtualMachine *, std::pair<std::chrono::steady_clock::time_point, int>> m_timeMap;
};

} // namespace libscratchcpp
