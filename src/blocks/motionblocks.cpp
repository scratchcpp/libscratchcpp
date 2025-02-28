// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/input.h>
#include <scratchcpp/value.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/irandomgenerator.h>
#include <scratchcpp/istacktimer.h>
#include <scratchcpp/stringptr.h>
#include <scratchcpp/string_functions.h>
#include <cmath>
#include <utf8.h>

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
    engine->addCompileFunction(this, "motion_pointtowards", &compilePointTowards);
    engine->addCompileFunction(this, "motion_gotoxy", &compileGoToXY);
    engine->addCompileFunction(this, "motion_goto", &compileGoTo);
    engine->addCompileFunction(this, "motion_glidesecstoxy", &compileGlideSecsToXY);
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

CompilerValue *MotionBlocks::compilePointTowards(Compiler *compiler)
{
    if (compiler->target()->isStage())
        return nullptr;

    Input *input = compiler->input("TOWARDS");

    if (input->pointsToDropdownMenu()) {
        std::string value = input->selectedMenuItem();

        if (value == "_mouse_")
            compiler->addTargetFunctionCall("motion_point_towards_mouse");
        else if (value == "_random_")
            compiler->addFunctionCallWithCtx("motion_point_towards_random_pos");
        else {
            int index = compiler->engine()->findTarget(value);
            Target *anotherTarget = compiler->engine()->targetAt(index);

            if (anotherTarget && !anotherTarget->isStage())
                compiler->addTargetFunctionCall("motion_point_towards_target_by_index", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { compiler->addConstValue(index) });
        }
    } else {
        CompilerValue *towards = compiler->addInput(input);
        compiler->addFunctionCallWithCtx("motion_pointtowards", Compiler::StaticType::Void, { Compiler::StaticType::String }, { towards });
    }

    return nullptr;
}

CompilerValue *MotionBlocks::compileGoToXY(Compiler *compiler)
{
    if (!compiler->target()->isStage()) {
        CompilerValue *x = compiler->addInput("X");
        CompilerValue *y = compiler->addInput("Y");
        compiler->addTargetFunctionCall("motion_gotoxy", Compiler::StaticType::Void, { Compiler::StaticType::Number, Compiler::StaticType::Number }, { x, y });
    }

    return nullptr;
}

CompilerValue *MotionBlocks::compileGoTo(Compiler *compiler)
{
    if (compiler->target()->isStage())
        return nullptr;

    Input *input = compiler->input("TO");

    if (input->pointsToDropdownMenu()) {
        std::string value = input->selectedMenuItem();

        if (value == "_mouse_")
            compiler->addTargetFunctionCall("motion_go_to_mouse");
        else if (value == "_random_")
            compiler->addFunctionCallWithCtx("motion_go_to_random_pos");
        else {
            int index = compiler->engine()->findTarget(value);
            Target *anotherTarget = compiler->engine()->targetAt(index);

            if (anotherTarget && !anotherTarget->isStage())
                compiler->addTargetFunctionCall("motion_go_to_target_by_index", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { compiler->addConstValue(index) });
        }
    } else {
        CompilerValue *to = compiler->addInput(input);
        compiler->addFunctionCallWithCtx("motion_goto", Compiler::StaticType::Void, { Compiler::StaticType::String }, { to });
    }

    return nullptr;
}

CompilerValue *MotionBlocks::compileGlideSecsToXY(Compiler *compiler)
{
    Target *target = compiler->target();
    CompilerValue *duration = compiler->addInput("SECS");
    CompilerValue *startX = target->isStage() ? compiler->addConstValue(0) : compiler->addTargetFunctionCall("motion_xposition", Compiler::StaticType::Number);
    CompilerValue *startY = target->isStage() ? compiler->addConstValue(0) : compiler->addTargetFunctionCall("motion_yposition", Compiler::StaticType::Number);
    CompilerValue *endX = compiler->addInput("X");
    CompilerValue *endY = compiler->addInput("Y");

    compiler->addFunctionCallWithCtx("motion_start_glide", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { duration });
    compiler->createYield();

    compiler->beginLoopCondition();
    auto numType = Compiler::StaticType::Number;
    CompilerValue *elapsed = compiler->addFunctionCallWithCtx("motion_glide", Compiler::StaticType::Bool, { numType, numType, numType, numType, numType }, { duration, startX, startY, endX, endY });
    compiler->beginRepeatUntilLoop(elapsed);
    compiler->endLoop();

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

inline void motion_point_towards_pos(Sprite *sprite, double x, double y)
{
    // https://en.scratch-wiki.info/wiki/Point_Towards_()_(block)#Workaround
    double deltaX = x - sprite->x();
    double deltaY = y - sprite->y();

    if (deltaY == 0) {
        if (deltaX < 0)
            sprite->setDirection(-90);
        else
            sprite->setDirection(90);
    } else if (deltaY < 0)
        sprite->setDirection(180 + (180 / pi) * std::atan(deltaX / deltaY));
    else
        sprite->setDirection((180 / pi) * std::atan(deltaX / deltaY));
}

extern "C" void motion_point_towards_mouse(Sprite *sprite)
{
    IEngine *engine = sprite->engine();
    motion_point_towards_pos(sprite, engine->mouseX(), engine->mouseY());
}

extern "C" void motion_point_towards_random_pos(ExecutionContext *ctx)
{
    Sprite *sprite = static_cast<Sprite *>(ctx->thread()->target());
    IEngine *engine = ctx->engine();
    const int stageWidth = engine->stageWidth();
    const int stageHeight = engine->stageHeight();
    IRandomGenerator *rng = ctx->rng();
    motion_point_towards_pos(sprite, rng->randintDouble(-stageWidth / 2.0, stageWidth / 2.0), rng->randintDouble(-stageHeight / 2.0, stageHeight / 2.0));
}

extern "C" void motion_point_towards_target_by_index(Sprite *sprite, double index)
{
    Sprite *anotherSprite = static_cast<Sprite *>(sprite->engine()->targetAt(index));
    motion_point_towards_pos(sprite, anotherSprite->x(), anotherSprite->y());
}

extern "C" void motion_pointtowards(ExecutionContext *ctx, const StringPtr *towards)
{
    static const StringPtr MOUSE_STR("_mouse_");
    static const StringPtr RANDOM_STR("_random_");

    Sprite *sprite = static_cast<Sprite *>(ctx->thread()->target());

    if (string_compare_case_sensitive(towards, &MOUSE_STR) == 0)
        motion_point_towards_mouse(sprite);
    else if (string_compare_case_sensitive(towards, &RANDOM_STR) == 0)
        motion_point_towards_random_pos(ctx);
    else {
        // TODO: Use UTF-16 in engine
        std::string u8name = utf8::utf16to8(std::u16string(towards->data));
        IEngine *engine = ctx->engine();
        Target *anotherTarget = engine->targetAt(engine->findTarget(u8name));

        if (anotherTarget && !anotherTarget->isStage()) {
            Sprite *anotherSprite = static_cast<Sprite *>(anotherTarget);
            motion_point_towards_pos(sprite, anotherSprite->x(), anotherSprite->y());
        }
    }
}

extern "C" void motion_gotoxy(Sprite *sprite, double x, double y)
{
    sprite->setPosition(x, y);
}

extern "C" void motion_go_to_mouse(Sprite *sprite)
{
    IEngine *engine = sprite->engine();
    sprite->setPosition(engine->mouseX(), engine->mouseY());
}

extern "C" void motion_go_to_random_pos(ExecutionContext *ctx)
{
    Sprite *sprite = static_cast<Sprite *>(ctx->thread()->target());
    IEngine *engine = ctx->engine();
    const int stageWidth = engine->stageWidth();
    const int stageHeight = engine->stageHeight();
    IRandomGenerator *rng = ctx->rng();
    sprite->setPosition(rng->randintDouble(-stageWidth / 2.0, stageWidth / 2.0), rng->randintDouble(-stageHeight / 2.0, stageHeight / 2.0));
}

extern "C" void motion_go_to_target_by_index(Sprite *sprite, double index)
{
    Sprite *anotherSprite = static_cast<Sprite *>(sprite->engine()->targetAt(index));
    sprite->setPosition(anotherSprite->x(), anotherSprite->y());
}

extern "C" void motion_goto(ExecutionContext *ctx, const StringPtr *towards)
{
    static const StringPtr MOUSE_STR("_mouse_");
    static const StringPtr RANDOM_STR("_random_");

    Sprite *sprite = static_cast<Sprite *>(ctx->thread()->target());

    if (string_compare_case_sensitive(towards, &MOUSE_STR) == 0)
        motion_go_to_mouse(sprite);
    else if (string_compare_case_sensitive(towards, &RANDOM_STR) == 0)
        motion_go_to_random_pos(ctx);
    else {
        // TODO: Use UTF-16 in engine
        std::string u8name = utf8::utf16to8(std::u16string(towards->data));
        IEngine *engine = ctx->engine();
        Target *anotherTarget = engine->targetAt(engine->findTarget(u8name));

        if (anotherTarget && !anotherTarget->isStage()) {
            Sprite *anotherSprite = static_cast<Sprite *>(anotherTarget);
            sprite->setPosition(anotherSprite->x(), anotherSprite->y());
        }
    }
}

extern "C" void motion_start_glide(ExecutionContext *ctx, double duration)
{
    ctx->stackTimer()->start(duration);
}

extern "C" bool motion_glide(ExecutionContext *ctx, double duration, double startX, double startY, double endX, double endY)
{
    IStackTimer *timer = ctx->stackTimer();
    Target *target = ctx->thread()->target();
    Sprite *sprite = nullptr;
    double elapsedTime = timer->elapsedTime();

    if (!target->isStage())
        sprite = static_cast<Sprite *>(target);

    if (elapsedTime >= duration) {
        if (sprite)
            sprite->setPosition(endX, endY);

        return true;
    } else {
        if (sprite) {
            double factor = elapsedTime / duration;
            assert(factor >= 0 && factor < 1);
            sprite->setPosition(startX + (endX - startX) * factor, startY + (endY - startY) * factor);
        }

        return false;
    }
}

extern "C" double motion_xposition(Sprite *sprite)
{
    return sprite->x();
}

extern "C" double motion_yposition(Sprite *sprite)
{
    return sprite->y();
}
