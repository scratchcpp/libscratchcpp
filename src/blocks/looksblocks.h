// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

/*! \brief The LooksBlocks class contains the implementation of looks blocks. */
class LooksBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            CHANGE,
            SIZE
        };

        enum Fields
        {
        };

        enum FieldValues
        {
        };

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileShow(Compiler *compiler);
        static void compileHide(Compiler *compiler);
        static void compileChangeSizeBy(Compiler *compiler);
        static void compileSetSizeTo(Compiler *compiler);

        static unsigned int show(VirtualMachine *vm);
        static unsigned int hide(VirtualMachine *vm);
        static unsigned int changeSizeBy(VirtualMachine *vm);
        static unsigned int setSizeTo(VirtualMachine *vm);
};

} // namespace libscratchcpp
