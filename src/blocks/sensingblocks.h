// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>
#include "../engine/internal/clock.h"

namespace libscratchcpp
{

/*! \brief The SensingBlocks class contains the implementation of sensing blocks. */
class SensingBlocks : public IBlockSection
{
    public:
        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileTimer(Compiler *compiler);
        static void compileResetTimer(Compiler *compiler);
        static void compileDaysSince2000(Compiler *compiler);

        static unsigned int timer(VirtualMachine *vm);
        static unsigned int resetTimer(VirtualMachine *vm);
        static unsigned int daysSince2000(VirtualMachine *vm);

        static IClock *clock;
};

} // namespace libscratchcpp
