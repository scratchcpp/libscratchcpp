// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
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
    engine->addCompileFunction(this, "motion_glideto", &compileGlideTo);
    engine->addCompileFunction(this, "motion_changexby", &compileChangeXBy);
    engine->addCompileFunction(this, "motion_setx", &compileSetX);
    engine->addCompileFunction(this, "motion_changeyby", &compileChangeYBy);
    engine->addCompileFunction(this, "motion_sety", &compileSetY);
    engine->addCompileFunction(this, "motion_ifonedgebounce", &compileIfOnEdgeBounce);
    engine->addCompileFunction(this, "motion_setrotationstyle", &compileSetRotationStyle);
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

CompilerValue *MotionBlocks::compileGlideTo(Compiler *compiler)
{
    Target *target = compiler->target();
    CompilerValue *duration = compiler->addInput("SECS");
    CompilerValue *startX = target->isStage() ? compiler->addConstValue(0) : compiler->addTargetFunctionCall("motion_xposition", Compiler::StaticType::Number);
    CompilerValue *startY = target->isStage() ? compiler->addConstValue(0) : compiler->addTargetFunctionCall("motion_yposition", Compiler::StaticType::Number);
    CompilerValue *endX = target->isStage() ? compiler->addConstValue(0) : nullptr;
    CompilerValue *endY = target->isStage() ? compiler->addConstValue(0) : nullptr;

    Input *input = compiler->input("TO");
    bool ifStatement = false;

    if (input->pointsToDropdownMenu()) {
        std::string value = input->selectedMenuItem();

        if (value == "_mouse_") {
            if (!target->isStage()) {
                endX = compiler->addFunctionCallWithCtx("motion_get_mouse_x", Compiler::StaticType::Number);
                endY = compiler->addFunctionCallWithCtx("motion_get_mouse_y", Compiler::StaticType::Number);
            }
        } else if (value == "_random_") {
            if (!target->isStage()) {
                endX = compiler->addFunctionCallWithCtx("motion_get_random_x", Compiler::StaticType::Number);
                endY = compiler->addFunctionCallWithCtx("motion_get_random_y", Compiler::StaticType::Number);
            }
        } else {
            int index = compiler->engine()->findTarget(value);
            Target *anotherTarget = compiler->engine()->targetAt(index);

            if (anotherTarget && !anotherTarget->isStage()) {
                if (!target->isStage()) {
                    endX = compiler->addFunctionCallWithCtx("motion_get_sprite_x_by_index", Compiler::StaticType::Number, { Compiler::StaticType::Number }, { compiler->addConstValue(index) });
                    endY = compiler->addFunctionCallWithCtx("motion_get_sprite_y_by_index", Compiler::StaticType::Number, { Compiler::StaticType::Number }, { compiler->addConstValue(index) });
                }
            } else
                return nullptr;
        }
    } else {
        CompilerValue *to = compiler->addInput(input);
        CompilerValue *valid = compiler->addFunctionCallWithCtx("motion_is_target_valid", Compiler::StaticType::Bool, { Compiler::StaticType::String }, { to });
        ifStatement = true;
        compiler->beginIfStatement(valid);

        if (!target->isStage()) {
            endX = compiler->addFunctionCallWithCtx("motion_get_target_x", Compiler::StaticType::Number, { Compiler::StaticType::String }, { to });
            endY = compiler->addFunctionCallWithCtx("motion_get_target_y", Compiler::StaticType::Number, { Compiler::StaticType::String }, { to });
        }
    }

    assert(endX && endY);
    assert(!target->isStage() || (target->isStage() && endX->isConst() && endY->isConst()));

    compiler->addFunctionCallWithCtx("motion_start_glide", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { duration });
    compiler->createYield();

    compiler->beginLoopCondition();
    auto numType = Compiler::StaticType::Number;
    CompilerValue *elapsed = compiler->addFunctionCallWithCtx("motion_glide", Compiler::StaticType::Bool, { numType, numType, numType, numType, numType }, { duration, startX, startY, endX, endY });
    compiler->beginRepeatUntilLoop(elapsed);
    compiler->endLoop();

    if (ifStatement)
        compiler->endIf();

    return nullptr;
}

CompilerValue *MotionBlocks::compileChangeXBy(Compiler *compiler)
{
    if (!compiler->target()->isStage()) {
        CompilerValue *dx = compiler->addInput("DX");
        compiler->addTargetFunctionCall("motion_changexby", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { dx });
    }

    return nullptr;
}

CompilerValue *MotionBlocks::compileSetX(Compiler *compiler)
{
    if (!compiler->target()->isStage()) {
        CompilerValue *x = compiler->addInput("X");
        compiler->addTargetFunctionCall("motion_setx", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { x });
    }

    return nullptr;
}

CompilerValue *MotionBlocks::compileChangeYBy(Compiler *compiler)
{
    if (!compiler->target()->isStage()) {
        CompilerValue *dy = compiler->addInput("DY");
        compiler->addTargetFunctionCall("motion_changeyby", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { dy });
    }

    return nullptr;
}

CompilerValue *MotionBlocks::compileSetY(Compiler *compiler)
{
    if (!compiler->target()->isStage()) {
        CompilerValue *y = compiler->addInput("Y");
        compiler->addTargetFunctionCall("motion_sety", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { y });
    }

    return nullptr;
}

CompilerValue *MotionBlocks::compileIfOnEdgeBounce(Compiler *compiler)
{
    if (!compiler->target()->isStage())
        compiler->addTargetFunctionCall("motion_ifonedgebounce");

    return nullptr;
}

CompilerValue *MotionBlocks::compileSetRotationStyle(Compiler *compiler)
{
    Target *target = compiler->target();

    if (target->isStage())
        return nullptr;

    Sprite *sprite = static_cast<Sprite *>(target);
    Field *field = compiler->field("STYLE");

    if (!field)
        return nullptr;

    std::string option = field->value().toString();
    sprite->setRotationStyle(field->value().toString());

    if (option == "left-right")
        compiler->addTargetFunctionCall("motion_set_left_right_style");
    else if (option == "don't rotate")
        compiler->addTargetFunctionCall("motion_set_do_not_rotate_style");
    else if (option == "all around")
        compiler->addTargetFunctionCall("motion_set_all_around_style");

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

extern "C" double motion_get_mouse_x(ExecutionContext *ctx)
{
    return ctx->engine()->mouseX();
}

extern "C" double motion_get_mouse_y(ExecutionContext *ctx)
{
    return ctx->engine()->mouseY();
}

extern "C" double motion_get_random_x(ExecutionContext *ctx)
{
    const int stageWidth = ctx->engine()->stageWidth();
    return ctx->rng()->randintDouble(-stageWidth / 2.0, stageWidth / 2.0);
}

extern "C" double motion_get_random_y(ExecutionContext *ctx)
{
    const int stageHeight = ctx->engine()->stageHeight();
    return ctx->rng()->randintDouble(-stageHeight / 2.0, stageHeight / 2.0);
}

extern "C" double motion_get_sprite_x_by_index(ExecutionContext *ctx, double index)
{
    assert(!ctx->engine()->targetAt(index)->isStage());
    Sprite *sprite = static_cast<Sprite *>(ctx->engine()->targetAt(index));
    return sprite->x();
}

extern "C" double motion_get_sprite_y_by_index(ExecutionContext *ctx, double index)
{
    assert(!ctx->engine()->targetAt(index)->isStage());
    Sprite *sprite = static_cast<Sprite *>(ctx->engine()->targetAt(index));
    return sprite->y();
}

extern "C" double motion_get_target_x(ExecutionContext *ctx, const StringPtr *name)
{
    static const StringPtr MOUSE_STR("_mouse_");
    static const StringPtr RANDOM_STR("_random_");

    if (string_compare_case_sensitive(name, &MOUSE_STR) == 0)
        return ctx->engine()->mouseX();
    else if (string_compare_case_sensitive(name, &RANDOM_STR) == 0)
        return motion_get_random_x(ctx);
    else {
        // TODO: Use UTF-16 in engine
        std::string u8name = utf8::utf16to8(std::u16string(name->data));
        IEngine *engine = ctx->engine();
        Target *target = engine->targetAt(engine->findTarget(u8name));

        if (target && !target->isStage()) {
            Sprite *sprite = static_cast<Sprite *>(target);
            return sprite->x();
        } else
            return 0;
    }
}

extern "C" double motion_get_target_y(ExecutionContext *ctx, const StringPtr *name)
{
    static const StringPtr MOUSE_STR("_mouse_");
    static const StringPtr RANDOM_STR("_random_");

    if (string_compare_case_sensitive(name, &MOUSE_STR) == 0)
        return ctx->engine()->mouseY();
    else if (string_compare_case_sensitive(name, &RANDOM_STR) == 0)
        return motion_get_random_y(ctx);
    else {
        // TODO: Use UTF-16 in engine
        std::string u8name = utf8::utf16to8(std::u16string(name->data));
        IEngine *engine = ctx->engine();
        Target *target = engine->targetAt(engine->findTarget(u8name));

        if (target && !target->isStage()) {
            Sprite *sprite = static_cast<Sprite *>(target);
            return sprite->y();
        } else
            return 0;
    }
}

extern "C" bool motion_is_target_valid(ExecutionContext *ctx, const StringPtr *name)
{
    static const StringPtr MOUSE_STR("_mouse_");
    static const StringPtr RANDOM_STR("_random_");

    if (string_compare_case_sensitive(name, &MOUSE_STR) == 0 || string_compare_case_sensitive(name, &RANDOM_STR) == 0)
        return true;
    else {
        // TODO: Use UTF-16 in engine
        std::string u8name = utf8::utf16to8(std::u16string(name->data));
        IEngine *engine = ctx->engine();
        Target *target = engine->targetAt(engine->findTarget(u8name));
        return (target && !target->isStage());
    }
}

extern "C" void motion_changexby(Sprite *sprite, double dx)
{
    sprite->setX(sprite->x() + dx);
}

extern "C" void motion_setx(Sprite *sprite, double x)
{
    sprite->setX(x);
}

extern "C" void motion_changeyby(Sprite *sprite, double dy)
{
    sprite->setY(sprite->y() + dy);
}

extern "C" void motion_sety(Sprite *sprite, double y)
{
    sprite->setY(y);
}

extern "C" void motion_ifonedgebounce(Sprite *sprite)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/c37745e97e6d8a77ad1dc31a943ea728dd17ba78/src/blocks/scratch3_motion.js#L186-L240
    IEngine *engine = sprite->engine();
    Rect bounds = sprite->boundingRect();

    // Measure distance to edges
    // Values are zero when the sprite is beyond
    double stageWidth = engine->stageWidth();
    double stageHeight = engine->stageHeight();
    double distLeft = std::max(0.0, (stageWidth / 2.0) + bounds.left());
    double distTop = std::max(0.0, (stageHeight / 2.0) - bounds.top());
    double distRight = std::max(0.0, (stageWidth / 2.0) - bounds.right());
    double distBottom = std::max(0.0, (stageHeight / 2.0) + bounds.bottom());

    // Find the nearest edge
    // 1 - left
    // 2 - top
    // 3 - right
    // 4 - bottom
    unsigned short nearestEdge = 0;
    double minDist = std::numeric_limits<double>::infinity();

    if (distLeft < minDist) {
        minDist = distLeft;
        nearestEdge = 1;
    }

    if (distTop < minDist) {
        minDist = distTop;
        nearestEdge = 2;
    }

    if (distRight < minDist) {
        minDist = distRight;
        nearestEdge = 3;
    }

    if (distBottom < minDist) {
        minDist = distBottom;
        nearestEdge = 4;
    }

    if (minDist > 0)
        return; // Not touching any edge

    assert(nearestEdge != 0);

    // Point away from the nearest edge
    double radians = (90 - sprite->direction()) * pi / 180;
    double dx = std::cos(radians);
    double dy = -std::sin(radians);

    switch (nearestEdge) {
        case 1:
            // Left
            dx = std::max(0.2, std::abs(dx));
            break;

        case 2:
            // Top
            dy = std::max(0.2, std::abs(dy));
            break;

        case 3:
            // Right
            dx = 0 - std::max(0.2, std::abs(dx));
            break;

        case 4:
            // Bottom
            dy = 0 - std::max(0.2, std::abs(dy));
            break;
    }

    double newDirection = (180 / pi) * (std::atan2(dy, dx)) + 90;
    sprite->setDirection(newDirection);

    // Keep within the stage
    double fencedX, fencedY;
    sprite->keepInFence(sprite->x(), sprite->y(), &fencedX, &fencedY);
    sprite->setPosition(fencedX, fencedY);
}

extern "C" void motion_set_left_right_style(Sprite *sprite)
{
    sprite->setRotationStyle(Sprite::RotationStyle::LeftRight);
}

extern "C" void motion_set_do_not_rotate_style(Sprite *sprite)
{
    sprite->setRotationStyle(Sprite::RotationStyle::DoNotRotate);
}

extern "C" void motion_set_all_around_style(Sprite *sprite)
{
    sprite->setRotationStyle(Sprite::RotationStyle::AllAround);
}

extern "C" double motion_xposition(Sprite *sprite)
{
    return sprite->x();
}

extern "C" double motion_yposition(Sprite *sprite)
{
    return sprite->y();
}
