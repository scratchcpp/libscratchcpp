// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

/*! \brief The MotionBlocks class contains the implementation of motion blocks. */
class MotionBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            STEPS
        };

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileMoveSteps(Compiler *compiler);

        static unsigned int moveSteps(VirtualMachine *vm);
};

} // namespace libscratchcpp
