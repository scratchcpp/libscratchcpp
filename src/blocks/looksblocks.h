// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

class Target;

/*! \brief The LooksBlocks class contains the implementation of looks blocks. */
class LooksBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            CHANGE,
            SIZE,
            COSTUME
        };

        enum Fields
        {
            NUMBER_NAME
        };

        enum FieldValues
        {
            CostumeNumber,
            CostumeName
        };

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileShow(Compiler *compiler);
        static void compileHide(Compiler *compiler);
        static void compileChangeSizeBy(Compiler *compiler);
        static void compileSetSizeTo(Compiler *compiler);
        static void compileSize(Compiler *compiler);
        static void compileSwitchCostumeTo(Compiler *compiler);
        static void compileCostumeNumberName(Compiler *compiler);

        static unsigned int show(VirtualMachine *vm);
        static unsigned int hide(VirtualMachine *vm);
        static unsigned int changeSizeBy(VirtualMachine *vm);
        static unsigned int setSizeTo(VirtualMachine *vm);
        static unsigned int size(VirtualMachine *vm);

        static void setCostumeByIndex(Target *target, long index);
        static unsigned int switchCostumeToByIndex(VirtualMachine *vm);
        static unsigned int switchCostumeTo(VirtualMachine *vm);
        static unsigned int nextCostume(VirtualMachine *vm);
        static unsigned int previousCostume(VirtualMachine *vm);

        static unsigned int costumeNumber(VirtualMachine *vm);
        static unsigned int costumeName(VirtualMachine *vm);
};

} // namespace libscratchcpp
