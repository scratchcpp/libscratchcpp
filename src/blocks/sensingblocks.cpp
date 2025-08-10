// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/promise.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/value.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/textbubble.h>
#include <scratchcpp/stringptr.h>
#include <scratchcpp/string_functions.h>
#include <scratchcpp/string_pool.h>
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
    engine->addCompileFunction(this, "sensing_askandwait", &compileAskAndWait);
    engine->addCompileFunction(this, "sensing_answer", &compileAnswer);
    engine->addCompileFunction(this, "sensing_keypressed", &compileKeyPressed);
    engine->addCompileFunction(this, "sensing_mousedown", &compileMouseDown);
    engine->addCompileFunction(this, "sensing_mousex", &compileMouseX);
    engine->addCompileFunction(this, "sensing_mousey", &compileMouseY);
    engine->addCompileFunction(this, "sensing_setdragmode", &compileSetDragMode);
}

void SensingBlocks::onInit(IEngine *engine)
{
    engine->questionAnswered().connect(&onAnswer);

    engine->threadAboutToStop().connect([engine](Thread *thread) {
        if (!m_questions.empty()) {
            // Abort the question of this thread if it's currently being displayed
            if (m_questions.front()->thread == thread) {
                thread->target()->bubble()->setText("");
                engine->questionAborted()();
            }

            m_questions.erase(std::remove_if(m_questions.begin(), m_questions.end(), [thread](const std::unique_ptr<Question> &question) { return question->thread == thread; }), m_questions.end());
        }
    });
}

void SensingBlocks::clearQuestions()
{
    m_questions.clear();
}

void SensingBlocks::askQuestion(ExecutionContext *ctx, const StringPtr *question)
{
    const bool isQuestionAsked = !m_questions.empty();
    // TODO: Use UTF-16 in engine (and TextBubble?)
    std::string u8str = utf8::utf16to8(std::u16string(question->data));
    enqueueAsk(u8str, ctx->thread());

    if (!isQuestionAsked)
        askNextQuestion();
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

CompilerValue *SensingBlocks::compileAskAndWait(Compiler *compiler)
{
    CompilerValue *question = compiler->addInput("QUESTION");
    compiler->addFunctionCallWithCtx("sensing_askandwait", Compiler::StaticType::Void, { Compiler::StaticType::String }, { question });
    compiler->createYield();
    return nullptr;
}

CompilerValue *SensingBlocks::compileAnswer(Compiler *compiler)
{
    return compiler->addFunctionCallWithCtx("sensing_answer", Compiler::StaticType::String);
}

CompilerValue *SensingBlocks::compileKeyPressed(Compiler *compiler)
{
    CompilerValue *key = compiler->addInput("KEY_OPTION");
    return compiler->addFunctionCallWithCtx("sensing_keypressed", Compiler::StaticType::Bool, { Compiler::StaticType::String }, { key });
}

CompilerValue *SensingBlocks::compileMouseDown(Compiler *compiler)
{
    return compiler->addFunctionCallWithCtx("sensing_mousedown", Compiler::StaticType::Bool);
}

CompilerValue *SensingBlocks::compileMouseX(Compiler *compiler)
{
    return compiler->addFunctionCallWithCtx("sensing_mousex", Compiler::StaticType::Number);
}

CompilerValue *SensingBlocks::compileMouseY(Compiler *compiler)
{
    return compiler->addFunctionCallWithCtx("sensing_mousey", Compiler::StaticType::Number);
}

CompilerValue *SensingBlocks::compileSetDragMode(Compiler *compiler)
{
    if (compiler->target()->isStage())
        return nullptr;

    Field *field = compiler->field("DRAG_MODE");
    assert(field);

    std::string mode = field->value().toString();
    CompilerValue *draggable = nullptr;

    if (mode == "draggable")
        draggable = compiler->addConstValue(true);
    else if (mode == "not draggable")
        draggable = compiler->addConstValue(false);
    else
        return nullptr;

    compiler->addTargetFunctionCall("sensing_setdragmode", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { draggable });
    return nullptr;
}

void SensingBlocks::onAnswer(const std::string &answer)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/6055823f203a696165084b873e661713806583ec/src/blocks/scratch3_sensing.js#L99-L115
    if (!m_questions.empty()) {
        Question *question = m_questions.front().get();
        Thread *thread = question->thread;
        assert(thread);
        assert(thread->target());

        // If the target was visible when asked, hide the say bubble unless the target was the stage
        if (question->wasVisible && !question->wasStage)
            thread->target()->bubble()->setText("");

        m_questions.erase(m_questions.begin());
        thread->promise()->resolve();
        askNextQuestion();
    }
}

void SensingBlocks::enqueueAsk(const std::string &question, Thread *thread)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/6055823f203a696165084b873e661713806583ec/src/blocks/scratch3_sensing.js#L117-L119
    assert(thread);
    Target *target = thread->target();
    assert(target);
    bool visible = true;
    bool isStage = target->isStage();

    if (!isStage) {
        Sprite *sprite = static_cast<Sprite *>(target);
        visible = sprite->visible();
    }

    m_questions.push_back(std::make_unique<Question>(question, thread, visible, isStage));
}

void SensingBlocks::askNextQuestion()
{
    // https://github.com/scratchfoundation/scratch-vm/blob/6055823f203a696165084b873e661713806583ec/src/blocks/scratch3_sensing.js#L121-L133
    if (m_questions.empty())
        return;

    Question *question = m_questions.front().get();
    Target *target = question->thread->target();
    IEngine *engine = question->thread->engine();

    // If the target is visible, emit a blank question and show
    // a bubble unless the target was the stage
    if (question->wasVisible && !question->wasStage) {
        target->bubble()->setType(TextBubble::Type::Say);
        target->bubble()->setText(question->question);

        engine->questionAsked()("");
    } else
        engine->questionAsked()(question->question);
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

extern "C" void sensing_askandwait(ExecutionContext *ctx, const StringPtr *question)
{
    SensingBlocks::askQuestion(ctx, question);
    ctx->thread()->setPromise(std::make_shared<Promise>());
}

extern "C" StringPtr *sensing_answer(ExecutionContext *ctx)
{
    StringPtr *ret = string_pool_new();
    string_assign(ret, ctx->engine()->answer());
    return ret;
}

extern "C" bool sensing_keypressed(ExecutionContext *ctx, const StringPtr *key)
{
    // TODO: Use UTF-16 in engine
    std::string u8name = utf8::utf16to8(std::u16string(key->data));
    return ctx->engine()->keyPressed(u8name);
}

extern "C" bool sensing_mousedown(ExecutionContext *ctx)
{
    return ctx->engine()->mousePressed();
}

extern "C" double sensing_mousex(ExecutionContext *ctx)
{
    return ctx->engine()->mouseX();
}

extern "C" double sensing_mousey(ExecutionContext *ctx)
{
    return ctx->engine()->mouseY();
}

extern "C" void sensing_setdragmode(Sprite *sprite, bool draggable)
{
    sprite->setDraggable(draggable);
}
