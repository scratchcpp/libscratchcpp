// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>

#include "test_export.h"

namespace libscratchcpp
{

class IAudioInput;

class LIBSCRATCHCPP_TEST_EXPORT EventBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;
        Rgb color() const override;

        void registerBlocks(IEngine *engine) override;

        static inline IAudioInput *audioInput = nullptr;

    private:
        static CompilerValue *compileWhenTouchingObject(Compiler *compiler);
        static CompilerValue *compileWhenTouchingObjectPredicate(Compiler *compiler);
        static CompilerValue *compileWhenFlagClicked(Compiler *compiler);
        static CompilerValue *compileWhenThisSpriteClicked(Compiler *compiler);
        static CompilerValue *compileWhenStageClicked(Compiler *compiler);
        static CompilerValue *compileWhenBroadcastReceived(Compiler *compiler);
        static CompilerValue *compileWhenBackdropSwitchesTo(Compiler *compiler);
        static CompilerValue *compileWhenGreaterThan(Compiler *compiler);
        static CompilerValue *compileWhenGreaterThanPredicate(Compiler *compiler);
        static CompilerValue *compileBroadcast(Compiler *compiler);
        static CompilerValue *compileBroadcastAndWait(Compiler *compiler);
        static CompilerValue *compileWhenKeyPressed(Compiler *compiler);
};

} // namespace libscratchcpp
