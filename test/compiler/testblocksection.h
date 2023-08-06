#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

class TestBlockSection : public IBlockSection
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

        void registerBlocks(IEngine *engine) override;

        static void compileTestBlock1(Compiler *compiler);
        static void compileTestBlock2(Compiler *compiler);
};

} // namespace libscratchcpp
