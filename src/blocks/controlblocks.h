// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/iblocksection.h"

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
            DURATION
        };

        enum Fields
        {
            STOP_OPTION
        };

        enum FieldValues
        {
            StopAll,
            StopThisScript,
            StopOtherScriptsInSprite
        };

        ControlBlocks();

        std::string name() const override;

        static void compileRepeatForever(Compiler *compiler);
        static void compileRepeat(Compiler *compiler);
        static void compileRepeatUntil(Compiler *compiler);
        static void compileRepeatWhile(Compiler *compiler);
        static void compileIfStatement(Compiler *compiler);
        static void compileIfElseStatement(Compiler *compiler);
        static void compileStop(Compiler *compiler);
        static void compileWait(Compiler *compiler);

    private:
        static unsigned int stopAll(VirtualMachine *vm);
        static unsigned int stopOtherScriptsInSprite(VirtualMachine *vm);
        static unsigned int startWait(VirtualMachine *vm);
        static unsigned int wait(VirtualMachine *vm);

        static inline std::unordered_map<VirtualMachine *, std::pair<std::chrono::steady_clock::time_point, int>> m_timeMap;
};

} // namespace libscratchcpp
