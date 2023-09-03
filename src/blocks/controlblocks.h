// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>
#include <unordered_map>
#include <chrono>

namespace libscratchcpp
{

class Compiler;
class VirtualMachine;

/*! \brief The ControlBlocks class contains the implementation of control blocks. */
class ControlBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            SUBSTACK,
            SUBSTACK2,
            TIMES,
            CONDITION,
            DURATION,
            VALUE,
            CLONE_OPTION
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
        static void compileStartAsClone(Compiler *compiler);
        static void compileCreateClone(Compiler *compiler);
        static void compileDeleteThisClone(Compiler *compiler);

        static unsigned int stopAll(VirtualMachine *vm);
        static unsigned int stopOtherScriptsInSprite(VirtualMachine *vm);
        static unsigned int startWait(VirtualMachine *vm);
        static unsigned int wait(VirtualMachine *vm);
        static unsigned int waitUntil(VirtualMachine *vm);
        static unsigned int createClone(VirtualMachine *vm);
        static unsigned int createCloneByIndex(VirtualMachine *vm);
        static unsigned int createCloneOfMyself(VirtualMachine *vm);
        static unsigned int deleteThisClone(VirtualMachine *vm);

        static inline std::unordered_map<VirtualMachine *, std::pair<std::chrono::steady_clock::time_point, int>> m_timeMap;
};

} // namespace libscratchcpp
