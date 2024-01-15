// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unordered_map>
#include <chrono>

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

class Sprite;
class IRandomGenerator;
class IClock;

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
            Y,
            TO,
            SECS,
            DX,
            DY
        };

        enum Fields
        {
            STYLE
        };

        enum FieldValues
        {
            LeftRight,
            DoNotRotate,
            AllAround
        };

        std::string name() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileMoveSteps(Compiler *compiler);
        static void compileTurnRight(Compiler *compiler);
        static void compileTurnLeft(Compiler *compiler);
        static void compilePointInDirection(Compiler *compiler);
        static void compilePointTowards(Compiler *compiler);
        static void compileGoToXY(Compiler *compiler);
        static void compileGoTo(Compiler *compiler);
        static void compileGlideSecsToXY(Compiler *compiler);
        static void compileGlideTo(Compiler *compiler);
        static void compileChangeXBy(Compiler *compiler);
        static void compileSetX(Compiler *compiler);
        static void compileChangeYBy(Compiler *compiler);
        static void compileSetY(Compiler *compiler);
        static void compileIfOnEdgeBounce(Compiler *compiler);
        static void compileSetRotationStyle(Compiler *compiler);
        static void compileXPosition(Compiler *compiler);
        static void compileYPosition(Compiler *compiler);
        static void compileDirection(Compiler *compiler);

        static const std::string &xPositionMonitorName(Block *block);
        static const std::string &yPositionMonitorName(Block *block);
        static const std::string &directionMonitorName(Block *block);

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
        static unsigned int goTo(VirtualMachine *vm);
        static unsigned int goToByIndex(VirtualMachine *vm);
        static unsigned int goToMousePointer(VirtualMachine *vm);
        static unsigned int goToRandomPosition(VirtualMachine *vm);

        static void startGlidingToPos(VirtualMachine *vm, double x, double y, double secs);
        static void continueGliding(VirtualMachine *vm);

        static unsigned int startGlideSecsTo(VirtualMachine *vm);
        static unsigned int glideSecsTo(VirtualMachine *vm);
        static unsigned int startGlideTo(VirtualMachine *vm);
        static unsigned int startGlideToByIndex(VirtualMachine *vm);
        static unsigned int startGlideToMousePointer(VirtualMachine *vm);
        static unsigned int startGlideToRandomPosition(VirtualMachine *vm);

        static unsigned int changeXBy(VirtualMachine *vm);
        static unsigned int setX(VirtualMachine *vm);
        static unsigned int changeYBy(VirtualMachine *vm);
        static unsigned int setY(VirtualMachine *vm);

        static unsigned int ifOnEdgeBounce(VirtualMachine *vm);

        static unsigned int setLeftRightRotationStyle(VirtualMachine *vm);
        static unsigned int setDoNotRotateRotationStyle(VirtualMachine *vm);
        static unsigned int setAllAroundRotationStyle(VirtualMachine *vm);

        static unsigned int xPosition(VirtualMachine *vm);
        static unsigned int yPosition(VirtualMachine *vm);
        static unsigned int direction(VirtualMachine *vm);

        static IRandomGenerator *rng;
        static IClock *clock;

        static inline std::unordered_map<VirtualMachine *, std::pair<std::chrono::steady_clock::time_point, int>> m_timeMap;
        static inline std::unordered_map<VirtualMachine *, std::pair<std::pair<double, double>, std::pair<double, double>>> m_glideMap; // start pos, end pos
};

} // namespace libscratchcpp
