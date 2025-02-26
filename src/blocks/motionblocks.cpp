// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/sprite.h>
#include <cmath>

#include "motionblocks.h"

using namespace libscratchcpp;

static const double pi = std::acos(-1); // TODO: Use std::numbers::pi in C++20

std::string MotionBlocks::name() const
{
    return "Motion";
}

std::string MotionBlocks::description() const
{
    return name() + " blocks";
}

Rgb MotionBlocks::color() const
{
    return rgb(76, 151, 255);
}

void MotionBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "motion_movesteps", &compileMoveSteps);
    engine->addCompileFunction(this, "motion_turnright", &compileTurnRight);
    engine->addCompileFunction(this, "motion_turnleft", &compileTurnLeft);
    engine->addCompileFunction(this, "motion_pointindirection", &compilePointInDirection);
}

CompilerValue *MotionBlocks::compileMoveSteps(Compiler *compiler)
{
    if (!compiler->target()->isStage()) {
        CompilerValue *steps = compiler->addInput("STEPS");
        compiler->addTargetFunctionCall("motion_movesteps", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { steps });
    }

    return nullptr;
}

CompilerValue *MotionBlocks::compileTurnRight(Compiler *compiler)
{
    if (!compiler->target()->isStage()) {
        CompilerValue *degrees = compiler->addInput("DEGREES");
        compiler->addTargetFunctionCall("motion_turnright", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { degrees });
    }

    return nullptr;
}

CompilerValue *MotionBlocks::compileTurnLeft(Compiler *compiler)
{
    if (!compiler->target()->isStage()) {
        CompilerValue *degrees = compiler->addInput("DEGREES");
        compiler->addTargetFunctionCall("motion_turnleft", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { degrees });
    }

    return nullptr;
}

CompilerValue *MotionBlocks::compilePointInDirection(Compiler *compiler)
{
    if (!compiler->target()->isStage()) {
        CompilerValue *direction = compiler->addInput("DIRECTION");
        compiler->addTargetFunctionCall("motion_pointindirection", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { direction });
    }

    return nullptr;
}

extern "C" void motion_movesteps(Sprite *sprite, double steps)
{
    double dir = sprite->direction();
    sprite->setPosition(sprite->x() + std::sin(dir * pi / 180) * steps, sprite->y() + std::cos(dir * pi / 180) * steps);
}

extern "C" void motion_turnright(Sprite *sprite, double degrees)
{
    sprite->setDirection(sprite->direction() + degrees);
}

extern "C" void motion_turnleft(Sprite *sprite, double degrees)
{
    sprite->setDirection(sprite->direction() - degrees);
}

extern "C" void motion_pointindirection(Sprite *sprite, double direction)
{
    sprite->setDirection(direction);
}
