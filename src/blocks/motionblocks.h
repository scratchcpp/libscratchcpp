// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class MotionBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;
        Rgb color() const override;

        void registerBlocks(IEngine *engine) override;

    private:
        static CompilerValue *compileMoveSteps(Compiler *compiler);
};

} // namespace libscratchcpp
