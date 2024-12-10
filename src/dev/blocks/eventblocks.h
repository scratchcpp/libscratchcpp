// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class EventBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;

        void registerBlocks(IEngine *engine) override;

    private:
        static CompilerValue *compileWhenTouchingObject(Compiler *compiler);
};

} // namespace libscratchcpp
