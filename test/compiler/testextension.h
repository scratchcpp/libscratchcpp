#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class TestExtension : public IExtension
{
    public:
        enum Inputs
        {
            INPUT1
        };

        enum Fields
        {
            FIELD1
        };

        enum FieldValues
        {
            TestValue
        };

        std::string name() const override;
        std::string description() const override;
        Rgb color() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileTestBlock1(Compiler *compiler);
        static void compileTestBlock2(Compiler *compiler);
};

} // namespace libscratchcpp
