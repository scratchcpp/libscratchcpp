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
#include <scratchcpp/stage.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/textbubble.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/stringptr.h>
#include <scratchcpp/string_functions.h>
#include <scratchcpp/string_pool.h>
#include <scratchcpp/itimer.h>
#include <utf8.h>

#include "sensingblocks.h"
#include "audio/audioinput.h"
#include "audio/iaudioloudness.h"
#include "engine/internal/clock.h"

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
    engine->addCompileFunction(this, "sensing_loudness", &compileLoudness);
    engine->addCompileFunction(this, "sensing_loud", &compileLoud);
    engine->addCompileFunction(this, "sensing_timer", &compileTimer);
    engine->addCompileFunction(this, "sensing_resettimer", &compileResetTimer);
    engine->addCompileFunction(this, "sensing_of", &compileOf);
    engine->addCompileFunction(this, "sensing_current", &compileCurrent);
    engine->addCompileFunction(this, "sensing_dayssince2000", &compileDaysSince2000);
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

CompilerValue *SensingBlocks::compileLoudness(Compiler *compiler)
{
    return compiler->addFunctionCall("sensing_loudness", Compiler::StaticType::Number);
}

CompilerValue *SensingBlocks::compileLoud(Compiler *compiler)
{
    CompilerValue *treshold = compiler->addConstValue(10);
    CompilerValue *loudness = compiler->addFunctionCall("sensing_loudness", Compiler::StaticType::Number);
    return compiler->createCmpGT(loudness, treshold);
}

CompilerValue *SensingBlocks::compileTimer(Compiler *compiler)
{
    ITimer *timer = compiler->engine()->timer();
    CompilerValue *timerPtr = compiler->addConstValue(timer);
    return compiler->addFunctionCall("sensing_timer", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { timerPtr });
}

CompilerValue *SensingBlocks::compileResetTimer(Compiler *compiler)
{
    ITimer *timer = compiler->engine()->timer();
    CompilerValue *timerPtr = compiler->addConstValue(timer);
    compiler->addFunctionCall("sensing_resettimer", Compiler::StaticType::Void, { Compiler::StaticType::Pointer }, { timerPtr });
    return nullptr;
}

CompilerValue *SensingBlocks::compileOf(Compiler *compiler)
{
    IEngine *engine = compiler->engine();
    Input *input = compiler->input("OBJECT");
    Field *field = compiler->field("PROPERTY");
    assert(input);
    assert(field);

    std::string property = field->value().toString();

    if (input->pointsToDropdownMenu()) {
        // Compile time
        std::string value = input->selectedMenuItem();
        Target *target = nullptr;
        CompilerValue *targetPtr = nullptr;

        if (value == "_stage_") {
            // Stage properties
            target = engine->stage();
            targetPtr = compiler->addConstValue(target);

            if (property == "backdrop #")
                return compiler->addFunctionCall("sensing_costume_number_of_target", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { targetPtr });
            else if (property == "backdrop name")
                return compiler->addFunctionCall("sensing_costume_name_of_target", Compiler::StaticType::String, { Compiler::StaticType::Pointer }, { targetPtr });
        } else {
            // Sprite properties
            target = engine->targetAt(engine->findTarget(value));

            if (target) {
                targetPtr = compiler->addConstValue(target);

                if (property == "x position")
                    return compiler->addFunctionCall("sensing_x_position_of_sprite", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { targetPtr });
                else if (property == "y position")
                    return compiler->addFunctionCall("sensing_y_position_of_sprite", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { targetPtr });
                else if (property == "direction")
                    return compiler->addFunctionCall("sensing_direction_of_sprite", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { targetPtr });
                else if (property == "costume #")
                    return compiler->addFunctionCall("sensing_costume_number_of_target", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { targetPtr });
                else if (property == "costume name")
                    return compiler->addFunctionCall("sensing_costume_name_of_target", Compiler::StaticType::String, { Compiler::StaticType::Pointer }, { targetPtr });
                else if (property == "size")
                    return compiler->addFunctionCall("sensing_size_of_sprite", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { targetPtr });
            }
        }

        // Common properties
        if (target && targetPtr) {
            if (property == "volume")
                return compiler->addFunctionCall("sensing_volume_of_target", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { targetPtr });
            else {
                auto var = target->variableAt(target->findVariable(property));

                if (var)
                    return compiler->addVariableValue(var.get());
            }
        }
    } else {
        // Runtime
        CompilerValue *targetName = compiler->addInput(input);
        CompilerValue *targetPtr = compiler->addFunctionCallWithCtx("sensing_get_target", Compiler::StaticType::Pointer, { Compiler::StaticType::String }, { targetName });

        // Stage properties
        if (property == "backdrop #") {
            return compiler->addFunctionCall("sensing_backdrop_number_of_stage_with_check", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { targetPtr });
        } else if (property == "backdrop name")
            return compiler->addFunctionCall("sensing_backdrop_name_of_stage_with_check", Compiler::StaticType::String, { Compiler::StaticType::Pointer }, { targetPtr });

        // Sprite properties
        if (property == "x position")
            return compiler->addFunctionCall("sensing_x_position_of_sprite_with_check", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { targetPtr });
        else if (property == "y position")
            return compiler->addFunctionCall("sensing_y_position_of_sprite_with_check", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { targetPtr });
        else if (property == "direction")
            return compiler->addFunctionCall("sensing_direction_of_sprite_with_check", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { targetPtr });
        else if (property == "costume #")
            return compiler->addFunctionCall("sensing_costume_number_of_sprite_with_check", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { targetPtr });
        else if (property == "costume name")
            return compiler->addFunctionCall("sensing_costume_name_of_sprite_with_check", Compiler::StaticType::String, { Compiler::StaticType::Pointer }, { targetPtr });
        else if (property == "size")
            return compiler->addFunctionCall("sensing_size_of_sprite_with_check", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { targetPtr });

        // Common properties
        if (property == "volume")
            return compiler->addFunctionCall("sensing_volume_of_target_with_check", Compiler::StaticType::Number, { Compiler::StaticType::Pointer }, { targetPtr });
        else {
            CompilerValue *varName = compiler->addConstValue(property);
            return compiler->addFunctionCall("sensing_variable_of_target", Compiler::StaticType::Unknown, { Compiler::StaticType::Pointer, Compiler::StaticType::String }, { targetPtr, varName });
        }
    }

    return compiler->addConstValue(0.0);
}

CompilerValue *SensingBlocks::compileCurrent(Compiler *compiler)
{
    Field *field = compiler->field("CURRENTMENU");
    assert(field);
    std::string option = field->value().toString();

    if (option == "YEAR")
        return compiler->addFunctionCall("sensing_current_year", Compiler::StaticType::Number);
    else if (option == "MONTH")
        return compiler->addFunctionCall("sensing_current_month", Compiler::StaticType::Number);
    else if (option == "DATE")
        return compiler->addFunctionCall("sensing_current_date", Compiler::StaticType::Number);
    else if (option == "DAYOFWEEK")
        return compiler->addFunctionCall("sensing_current_day_of_week", Compiler::StaticType::Number);
    else if (option == "HOUR")
        return compiler->addFunctionCall("sensing_current_hour", Compiler::StaticType::Number);
    else if (option == "MINUTE")
        return compiler->addFunctionCall("sensing_current_minute", Compiler::StaticType::Number);
    else if (option == "SECOND")
        return compiler->addFunctionCall("sensing_current_second", Compiler::StaticType::Number);
    else
        return compiler->addConstValue(Value());
}

CompilerValue *SensingBlocks::compileDaysSince2000(Compiler *compiler)
{
    return compiler->addFunctionCall("sensing_dayssince2000", Compiler::StaticType::Number);
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

BLOCK_EXPORT bool sensing_touching_mouse(Target *target)
{
    IEngine *engine = target->engine();
    return target->touchingPoint(engine->mouseX(), engine->mouseY());
}

BLOCK_EXPORT bool sensing_touching_edge(Target *target)
{
    return target->touchingEdge();
}

BLOCK_EXPORT bool sensing_touching_sprite(Target *target, Sprite *sprite)
{
    return target->touchingSprite(sprite);
}

BLOCK_EXPORT bool sensing_touchingobject(Target *target, const StringPtr *object)
{
    static const StringPtr MOUSE_STR("_mouse_");
    static const StringPtr EDGE_STR("_edge_");
    static const StringPtr STAGE_STR("_stage_");

    if (strings_equal_case_sensitive(object, &MOUSE_STR))
        return sensing_touching_mouse(target);
    else if (strings_equal_case_sensitive(object, &EDGE_STR))
        return sensing_touching_edge(target);
    else if (!strings_equal_case_sensitive(object, &STAGE_STR)) {
        IEngine *engine = target->engine();
        // TODO: Use UTF-16 in engine
        std::string u8name = utf8::utf16to8(std::u16string(object->data));
        Target *objTarget = engine->targetAt(engine->findTarget(u8name));

        if (objTarget)
            return sensing_touching_sprite(target, static_cast<Sprite *>(objTarget));
    }

    return false;
}

BLOCK_EXPORT bool sensing_touchingcolor(Target *target, const ValueData *color)
{
    return target->touchingColor(value_toRgba(color));
}

BLOCK_EXPORT bool sensing_coloristouchingcolor(Target *target, const ValueData *color, const ValueData *color2)
{
    return target->touchingColor(value_toRgba(color), value_toRgba(color2));
}

static inline double sensing_distance(double x0, double y0, double x1, double y1)
{
    return std::sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}

BLOCK_EXPORT double sensing_distance_to_mouse(Sprite *sprite)
{
    IEngine *engine = sprite->engine();
    return sensing_distance(sprite->x(), sprite->y(), engine->mouseX(), engine->mouseY());
}

BLOCK_EXPORT double sensing_distance_to_sprite(Sprite *sprite, Sprite *targetSprite)
{
    return sensing_distance(sprite->x(), sprite->y(), targetSprite->x(), targetSprite->y());
}

BLOCK_EXPORT double sensing_distanceto(Sprite *sprite, const StringPtr *object)
{
    static const StringPtr MOUSE_STR("_mouse_");
    static const StringPtr STAGE_STR("_stage_");

    if (strings_equal_case_sensitive(object, &MOUSE_STR))
        return sensing_distance_to_mouse(sprite);
    else if (!strings_equal_case_sensitive(object, &STAGE_STR)) {
        IEngine *engine = sprite->engine();
        // TODO: Use UTF-16 in engine
        std::string u8name = utf8::utf16to8(std::u16string(object->data));
        Target *objTarget = engine->targetAt(engine->findTarget(u8name));

        if (objTarget)
            return sensing_distance_to_sprite(sprite, static_cast<Sprite *>(objTarget));
    }

    return 10000.0;
}

BLOCK_EXPORT void sensing_askandwait(ExecutionContext *ctx, const StringPtr *question)
{
    SensingBlocks::askQuestion(ctx, question);
    ctx->thread()->setPromise(std::make_shared<Promise>());
}

BLOCK_EXPORT void sensing_answer(StringPtr *ret, ExecutionContext *ctx)
{
    string_assign(ret, ctx->engine()->answer());
}

BLOCK_EXPORT bool sensing_keypressed(ExecutionContext *ctx, const StringPtr *key)
{
    // TODO: Use UTF-16 in engine
    std::string u8name = utf8::utf16to8(std::u16string(key->data));
    return ctx->engine()->keyPressed(u8name);
}

BLOCK_EXPORT bool sensing_mousedown(ExecutionContext *ctx)
{
    return ctx->engine()->mousePressed();
}

BLOCK_EXPORT double sensing_mousex(ExecutionContext *ctx)
{
    return ctx->engine()->mouseX();
}

BLOCK_EXPORT double sensing_mousey(ExecutionContext *ctx)
{
    return ctx->engine()->mouseY();
}

BLOCK_EXPORT void sensing_setdragmode(Sprite *sprite, bool draggable)
{
    sprite->setDraggable(draggable);
}

BLOCK_EXPORT double sensing_loudness()
{
    if (!SensingBlocks::audioInput)
        SensingBlocks::audioInput = AudioInput::instance().get();

    auto audioLoudness = SensingBlocks::audioInput->audioLoudness();
    return audioLoudness->getLoudness();
}

BLOCK_EXPORT double sensing_timer(ITimer *timer)
{
    return timer->value();
}

BLOCK_EXPORT void sensing_resettimer(ITimer *timer)
{
    timer->reset();
}

BLOCK_EXPORT double sensing_x_position_of_sprite(Sprite *sprite)
{
    return sprite->x();
}

BLOCK_EXPORT double sensing_y_position_of_sprite(Sprite *sprite)
{
    return sprite->y();
}

BLOCK_EXPORT double sensing_direction_of_sprite(Sprite *sprite)
{
    return sprite->direction();
}

BLOCK_EXPORT double sensing_costume_number_of_target(Target *target)
{
    return target->costumeIndex() + 1;
}

BLOCK_EXPORT void sensing_costume_name_of_target(StringPtr *ret, Target *target)
{
    const std::string &name = target->currentCostume()->name();
    string_assign_cstring(ret, name.c_str());
}

BLOCK_EXPORT double sensing_size_of_sprite(Sprite *sprite)
{
    return sprite->size();
}

BLOCK_EXPORT double sensing_volume_of_target(Target *target)
{
    return target->volume();
}

BLOCK_EXPORT Target *sensing_get_target(ExecutionContext *ctx, const StringPtr *name)
{
    // TODO: Use UTF-16 in engine
    std::string u8name = utf8::utf16to8(std::u16string(name->data));
    IEngine *engine = ctx->engine();
    return engine->targetAt(engine->findTarget(u8name));
}

BLOCK_EXPORT double sensing_x_position_of_sprite_with_check(Target *target)
{
    if (target && !target->isStage()) {
        Sprite *sprite = static_cast<Sprite *>(target);
        return sprite->x();
    }

    return 0.0;
}

BLOCK_EXPORT double sensing_y_position_of_sprite_with_check(Target *target)
{
    if (target && !target->isStage()) {
        Sprite *sprite = static_cast<Sprite *>(target);
        return sprite->y();
    }

    return 0.0;
}

BLOCK_EXPORT double sensing_direction_of_sprite_with_check(Target *target)
{
    if (target && !target->isStage()) {
        Sprite *sprite = static_cast<Sprite *>(target);
        return sprite->direction();
    }

    return 0.0;
}

BLOCK_EXPORT double sensing_costume_number_of_sprite_with_check(Target *target)
{
    if (target && !target->isStage())
        return target->costumeIndex() + 1;

    return 0.0;
}

BLOCK_EXPORT void sensing_costume_name_of_sprite_with_check(StringPtr *ret, Target *target)
{
    if (target && !target->isStage()) {
        const std::string &name = target->currentCostume()->name();
        string_assign_cstring(ret, name.c_str());
    } else
        string_assign_cstring(ret, "0");
}

BLOCK_EXPORT double sensing_size_of_sprite_with_check(Target *target)
{
    if (target && !target->isStage()) {
        Sprite *sprite = static_cast<Sprite *>(target);
        return sprite->size();
    }

    return 0.0;
}

BLOCK_EXPORT double sensing_backdrop_number_of_stage_with_check(Target *target)
{
    if (target && target->isStage())
        return target->costumeIndex() + 1;

    return 0.0;
}

BLOCK_EXPORT void sensing_backdrop_name_of_stage_with_check(StringPtr *ret, Target *target)
{
    if (target && target->isStage()) {
        const std::string &name = target->currentCostume()->name();
        string_assign_cstring(ret, name.c_str());
    } else
        string_assign_cstring(ret, "");
}

BLOCK_EXPORT double sensing_volume_of_target_with_check(Target *target)
{
    if (target)
        return target->volume();

    return 0.0;
}

BLOCK_EXPORT ValueData sensing_variable_of_target(Target *target, const StringPtr *varName)
{
    if (target) {
        // TODO: Use UTF-16 in... Target?
        std::string u8name = utf8::utf16to8(std::u16string(varName->data));
        int varIndex = target->findVariable(u8name);
        if (varIndex >= 0) {
            auto var = target->variableAt(varIndex);

            if (var) {
                ValueData ret;
                value_init(&ret);
                value_assign_copy(&ret, &var->value().data());
                return ret;
            }
        }
    }

    ValueData ret;
    value_init(&ret);
    return ret;
}

BLOCK_EXPORT double sensing_current_year()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    return ltm->tm_year + 1900;
}

BLOCK_EXPORT double sensing_current_month()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    return ltm->tm_mon + 1;
}

BLOCK_EXPORT double sensing_current_date()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    return ltm->tm_mday;
}

BLOCK_EXPORT double sensing_current_day_of_week()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    return ltm->tm_wday + 1;
}

BLOCK_EXPORT double sensing_current_hour()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    return ltm->tm_hour;
}

BLOCK_EXPORT double sensing_current_minute()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    return ltm->tm_min;
}

BLOCK_EXPORT double sensing_current_second()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    return ltm->tm_sec;
}

BLOCK_EXPORT double sensing_dayssince2000()
{
    if (!SensingBlocks::clock)
        SensingBlocks::clock = Clock::instance().get();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(SensingBlocks::clock->currentSystemTime().time_since_epoch()).count();
    return ms / 86400000.0 - 10957.0;
}
