// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>
#include <vector>
#include <memory>

#include "test_export.h"

namespace libscratchcpp
{

class IAudioInput;
class IClock;
class ExecutionContext;
class Thread;

class LIBSCRATCHCPP_TEST_EXPORT SensingBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;
        Rgb color() const override;

        void registerBlocks(IEngine *engine) override;
        void onInit(IEngine *engine) override;

        static void clearQuestions();
        static void askQuestion(ExecutionContext *ctx, const StringPtr *question);

        static inline IAudioInput *audioInput = nullptr;
        static inline IClock *clock = nullptr;

    private:
        struct Question
        {
                Question(const std::string &question, Thread *thread, bool wasVisible, bool wasStage) :
                    question(question),
                    thread(thread),
                    wasVisible(wasVisible),
                    wasStage(wasStage)
                {
                }

                std::string question;
                Thread *thread = nullptr;
                bool wasVisible = false;
                bool wasStage = false;
        };

        static CompilerValue *compileTouchingObject(Compiler *compiler);
        static CompilerValue *compileTouchingColor(Compiler *compiler);
        static CompilerValue *compileColorIsTouchingColor(Compiler *compiler);
        static CompilerValue *compileDistanceTo(Compiler *compiler);
        static CompilerValue *compileAskAndWait(Compiler *compiler);
        static CompilerValue *compileAnswer(Compiler *compiler);
        static CompilerValue *compileKeyPressed(Compiler *compiler);
        static CompilerValue *compileMouseDown(Compiler *compiler);
        static CompilerValue *compileMouseX(Compiler *compiler);
        static CompilerValue *compileMouseY(Compiler *compiler);
        static CompilerValue *compileSetDragMode(Compiler *compiler);
        static CompilerValue *compileLoudness(Compiler *compiler);
        static CompilerValue *compileLoud(Compiler *compiler);
        static CompilerValue *compileTimer(Compiler *compiler);
        static CompilerValue *compileResetTimer(Compiler *compiler);
        static CompilerValue *compileOf(Compiler *compiler);
        static CompilerValue *compileCurrent(Compiler *compiler);
        static CompilerValue *compileDaysSince2000(Compiler *compiler);

        static void onAnswer(const std::string &answer);
        static void enqueueAsk(const std::string &question, Thread *thread);
        static void askNextQuestion();

        static inline std::vector<std::unique_ptr<Question>> m_questions;
};

} // namespace libscratchcpp
