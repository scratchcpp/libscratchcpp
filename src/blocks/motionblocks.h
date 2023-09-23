// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

class Sprite;
class IRandomGenerator;

/*! \brief The MotionBlocks class contains the implementation of motion blocks. */
class MotionBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            STEPS,
            DEGREES,
            DIRECTION,
            TOWARDS,
            X,
            Y
        };

        enum Fields
        {
        };

        enum FieldValues
        {
        };

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileMoveSteps(Compiler *compiler);
        static void compileTurnRight(Compiler *compiler);
        static void compileTurnLeft(Compiler *compiler);
        static void compilePointInDirection(Compiler *compiler);
        static void compilePointTowards(Compiler *compiler);
        static void compileGoToXY(Compiler *compiler);

        static unsigned int moveSteps(VirtualMachine *vm);
        static unsigned int turnRight(VirtualMachine *vm);
        static unsigned int turnLeft(VirtualMachine *vm);
        static unsigned int pointInDirection(VirtualMachine *vm);

        static void pointTowardsPos(Sprite *sprite, double x, double y);

        static unsigned int pointTowards(VirtualMachine *vm);
        static unsigned int pointTowardsByIndex(VirtualMachine *vm);
        static unsigned int pointTowardsMousePointer(VirtualMachine *vm);
        static unsigned int pointTowardsRandomPosition(VirtualMachine *vm);

        static unsigned int goToXY(VirtualMachine *vm);

        static IRandomGenerator *rng;
};

} // namespace libscratchcpp
