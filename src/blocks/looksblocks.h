// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

class Target;
class Stage;
class Value;
class IRandomGenerator;

/*! \brief The LooksBlocks class contains the implementation of looks blocks. */
class LooksBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            CHANGE,
            SIZE,
            COSTUME,
            BACKDROP
        };

        enum Fields
        {
            NUMBER_NAME
        };

        enum FieldValues
        {
            Number,
            Name
        };

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileShow(Compiler *compiler);
        static void compileHide(Compiler *compiler);
        static void compileChangeSizeBy(Compiler *compiler);
        static void compileSetSizeTo(Compiler *compiler);
        static void compileSize(Compiler *compiler);
        static void compileSwitchCostumeTo(Compiler *compiler);
        static void compileNextCostume(Compiler *compiler);
        static void compileSwitchBackdropTo(Compiler *compiler);
        static void compileSwitchBackdropToAndWait(Compiler *compiler);
        static void compileNextBackdrop(Compiler *compiler);
        static void compileCostumeNumberName(Compiler *compiler);
        static void compileBackdropNumberName(Compiler *compiler);

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

        static void startBackdropScripts(VirtualMachine *vm, bool wait);
        static void switchBackdropToByIndexImpl(VirtualMachine *vm);
        static void switchBackdropToImpl(VirtualMachine *vm);
        static void nextBackdropImpl(VirtualMachine *vm);
        static void previousBackdropImpl(VirtualMachine *vm);
        static void randomBackdropImpl(VirtualMachine *vm);

        static unsigned int switchBackdropToByIndex(VirtualMachine *vm);
        static unsigned int switchBackdropTo(VirtualMachine *vm);
        static unsigned int switchBackdropToByIndexAndWait(VirtualMachine *vm);
        static unsigned int switchBackdropToAndWait(VirtualMachine *vm);
        static unsigned int nextBackdrop(VirtualMachine *vm);
        static unsigned int nextBackdropAndWait(VirtualMachine *vm);
        static unsigned int previousBackdrop(VirtualMachine *vm);
        static unsigned int previousBackdropAndWait(VirtualMachine *vm);
        static unsigned int randomBackdrop(VirtualMachine *vm);
        static unsigned int randomBackdropAndWait(VirtualMachine *vm);
        static unsigned int checkBackdropScripts(VirtualMachine *vm);

        static unsigned int costumeNumber(VirtualMachine *vm);
        static unsigned int costumeName(VirtualMachine *vm);
        static unsigned int backdropNumber(VirtualMachine *vm);
        static unsigned int backdropName(VirtualMachine *vm);

        static IRandomGenerator *rng;
};

} // namespace libscratchcpp
