// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/value.h>
#include <scratchcpp/input.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stringptr.h>
#include <scratchcpp/string_functions.h>
#include <utf8.h>

#include "sensingblocks.h"

using namespace libscratchcpp;

std::string SensingBlocks::name() const
{
    return "Sensing";
}

std::string SensingBlocks::description() const
{
    return name() + " blocks";
}

Rgb SensingBlocks::color() const
{
    return rgb(92, 177, 214);
}

void SensingBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "sensing_touchingobject", &compileTouchingObject);
    engine->addCompileFunction(this, "sensing_touchingcolor", &compileTouchingColor);
    engine->addCompileFunction(this, "sensing_coloristouchingcolor", &compileColorIsTouchingColor);
    engine->addCompileFunction(this, "sensing_distanceto", &compileDistanceTo);
}

CompilerValue *SensingBlocks::compileTouchingObject(Compiler *compiler)
{
    IEngine *engine = compiler->engine();
    Input *input = compiler->input("TOUCHINGOBJECTMENU");

    if (input->pointsToDropdownMenu()) {
        std::string value = input->selectedMenuItem();

        if (value == "_mouse_")
            return compiler->addTargetFunctionCall("sensing_touching_mouse", Compiler::StaticType::Bool);
        else if (value == "_edge_")
            return compiler->addTargetFunctionCall("sensing_touching_edge", Compiler::StaticType::Bool);
        else if (value != "_stage_") {
            Target *target = engine->targetAt(engine->findTarget(value));

            if (target) {
                CompilerValue *targetPtr = compiler->addConstValue(target);
                return compiler->addTargetFunctionCall("sensing_touching_sprite", Compiler::StaticType::Bool, { Compiler::StaticType::Pointer }, { targetPtr });
            }
        }
    } else {
        CompilerValue *object = compiler->addInput(input);
        return compiler->addTargetFunctionCall("sensing_touchingobject", Compiler::StaticType::Bool, { Compiler::StaticType::String }, { object });
    }

    return compiler->addConstValue(false);
}

CompilerValue *SensingBlocks::compileTouchingColor(Compiler *compiler)
{
    CompilerValue *color = compiler->addInput("COLOR");
    return compiler->addTargetFunctionCall("sensing_touchingcolor", Compiler::StaticType::Bool, { Compiler::StaticType::Unknown }, { color });
}

CompilerValue *SensingBlocks::compileColorIsTouchingColor(Compiler *compiler)
{
    CompilerValue *color = compiler->addInput("COLOR");
    CompilerValue *color2 = compiler->addInput("COLOR2");
    return compiler->addTargetFunctionCall("sensing_coloristouchingcolor", Compiler::StaticType::Bool, { Compiler::StaticType::Unknown, Compiler::StaticType::Unknown }, { color, color2 });
}

CompilerValue *SensingBlocks::compileDistanceTo(Compiler *compiler)
{
    if (compiler->target()->isStage())
        return compiler->addConstValue(10000.0);

    IEngine *engine = compiler->engine();
    Input *input = compiler->input("DISTANCETOMENU");

    if (input->pointsToDropdownMenu()) {
        std::string value = input->selectedMenuItem();

        if (value == "_mouse_")
            return compiler->addTargetFunctionCall("sensing_distance_to_mouse", Compiler::StaticType::Number);
        else if (value != "_stage_") {
            Target *target = engine->targetAt(engine->findTarget(value));

            if (target) {
                CompilerValue *targetPtr = compiler->addConstValue(target);
                return compiler->addTargetFunctionCall("sensing_distance_to_sprite", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { targetPtr });
            }
        }
    } else {
        CompilerValue *object = compiler->addInput(input);
        return compiler->addTargetFunctionCall("sensing_distanceto", Compiler::StaticType::Number, { Compiler::StaticType::String }, { object });
    }

    return compiler->addConstValue(10000.0);
}

extern "C" bool sensing_touching_mouse(Target *target)
{
    IEngine *engine = target->engine();
    return target->touchingPoint(engine->mouseX(), engine->mouseY());
}

extern "C" bool sensing_touching_edge(Target *target)
{
    return target->touchingEdge();
}

extern "C" bool sensing_touching_sprite(Target *target, Sprite *sprite)
{
    return target->touchingSprite(sprite);
}

extern "C" bool sensing_touchingobject(Target *target, const StringPtr *object)
{
    static const StringPtr MOUSE_STR("_mouse_");
    static const StringPtr EDGE_STR("_edge_");
    static const StringPtr STAGE_STR("_stage_");

    if (string_compare_case_sensitive(object, &MOUSE_STR) == 0)
        return sensing_touching_mouse(target);
    else if (string_compare_case_sensitive(object, &EDGE_STR) == 0)
        return sensing_touching_edge(target);
    else if (string_compare_case_sensitive(object, &STAGE_STR) != 0) {
        IEngine *engine = target->engine();
        // TODO: Use UTF-16 in engine
        std::string u8name = utf8::utf16to8(std::u16string(object->data));
        Target *objTarget = engine->targetAt(engine->findTarget(u8name));

        if (objTarget)
            return sensing_touching_sprite(target, static_cast<Sprite *>(objTarget));
    }

    return false;
}

extern "C" bool sensing_touchingcolor(Target *target, const ValueData *color)
{
    return target->touchingColor(value_toRgba(color));
}

extern "C" bool sensing_coloristouchingcolor(Target *target, const ValueData *color, const ValueData *color2)
{
    return target->touchingColor(value_toRgba(color), value_toRgba(color2));
}

static inline double sensing_distance(double x0, double y0, double x1, double y1)
{
    return std::sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}

extern "C" double sensing_distance_to_mouse(Sprite *sprite)
{
    IEngine *engine = sprite->engine();
    return sensing_distance(sprite->x(), sprite->y(), engine->mouseX(), engine->mouseY());
}

extern "C" double sensing_distance_to_sprite(Sprite *sprite, Sprite *targetSprite)
{
    return sensing_distance(sprite->x(), sprite->y(), targetSprite->x(), targetSprite->y());
}

extern "C" double sensing_distanceto(Sprite *sprite, const StringPtr *object)
{
    static const StringPtr MOUSE_STR("_mouse_");
    static const StringPtr STAGE_STR("_stage_");

    if (string_compare_case_sensitive(object, &MOUSE_STR) == 0)
        return sensing_distance_to_mouse(sprite);
    else if (string_compare_case_sensitive(object, &STAGE_STR) != 0) {
        IEngine *engine = sprite->engine();
        // TODO: Use UTF-16 in engine
        std::string u8name = utf8::utf16to8(std::u16string(object->data));
        Target *objTarget = engine->targetAt(engine->findTarget(u8name));

        if (objTarget)
            return sensing_distance_to_sprite(sprite, static_cast<Sprite *>(objTarget));
    }

    return 10000.0;
}
