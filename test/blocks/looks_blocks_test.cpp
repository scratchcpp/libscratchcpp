#include <scratchcpp/project.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/executablecode.h>
#include <scratchcpp/executioncontext.h>
#include <enginemock.h>
#include <graphicseffectmock.h>
#include <stacktimermock.h>

#include "../common.h"
#include "blocks/looksblocks.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

using ::testing::Return;

class LooksBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<LooksBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
            m_extension->onInit(m_engine);
        }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
};

TEST_F(LooksBlocksTest, StopProject)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();
    GraphicsEffectMock effect;
    m_engine->setTargets({ stage, sprite });

    stage->bubble()->setText("abc");
    sprite->bubble()->setText("def");
    EXPECT_CALL(effect, clamp(10)).WillOnce(Return(10));
    sprite->setGraphicsEffectValue(&effect, 10);
    EXPECT_CALL(effect, clamp(2.5)).WillOnce(Return(2.5));
    stage->setGraphicsEffectValue(&effect, 2.5);

    m_engine->stop();
    ASSERT_TRUE(stage->bubble()->text().empty());
    ASSERT_TRUE(sprite->bubble()->text().empty());
    ASSERT_EQ(stage->graphicsEffectValue(&effect), 0);
    ASSERT_EQ(sprite->graphicsEffectValue(&effect), 0);
}

TEST_F(LooksBlocksTest, SayAndThinkForSecs)
{
    std::vector<TextBubble::Type> types = { TextBubble::Type::Say, TextBubble::Type::Think };
    std::vector<std::string> opcodes = { "looks_sayforsecs", "looks_thinkforsecs" };

    for (int i = 0; i < types.size(); i++) {
        TextBubble::Type type = types[i];
        const std::string &opcode = opcodes[i];

        auto sprite = std::make_shared<Sprite>();
        sprite->setEngine(&m_engineMock);
        m_engine->clear();

        ScriptBuilder builder(m_extension.get(), m_engine, sprite);

        builder.addBlock(opcode);
        builder.addValueInput("MESSAGE", "Hello world");
        builder.addValueInput("SECS", 2.5);
        auto block = builder.currentBlock();

        Compiler compiler(&m_engineMock, sprite.get());
        auto code = compiler.compile(block);
        Script script(sprite.get(), block, &m_engineMock);
        script.setCode(code);
        Thread thread(sprite.get(), &m_engineMock, &script);
        auto ctx = code->createExecutionContext(&thread);
        StackTimerMock timer;
        ctx->setStackTimer(&timer);

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
        ASSERT_EQ(sprite->bubble()->text(), "Hello world");
        ASSERT_EQ(sprite->bubble()->type(), type);

        EXPECT_CALL(timer, elapsed()).WillOnce(Return(false));
        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));
        ASSERT_EQ(sprite->bubble()->text(), "Hello world");
        ASSERT_EQ(sprite->bubble()->type(), type);

        EXPECT_CALL(timer, elapsed()).WillOnce(Return(true));
        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
        ASSERT_TRUE(sprite->bubble()->text().empty());

        // Change text while waiting
        code->reset(ctx.get());

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(m_engineMock, requestRedraw());
        sprite->bubble()->setText("test");

        EXPECT_CALL(timer, elapsed()).WillOnce(Return(true));
        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
        ASSERT_EQ(sprite->bubble()->text(), "test");
        ASSERT_EQ(sprite->bubble()->type(), type);

        code->reset(ctx.get());

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(m_engineMock, requestRedraw());
        sprite->bubble()->setText("Hello world");

        EXPECT_CALL(timer, elapsed()).WillOnce(Return(true));
        EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
        code->run(ctx.get());
        ASSERT_TRUE(code->isFinished(ctx.get()));
        ASSERT_EQ(sprite->bubble()->text(), "Hello world");
        ASSERT_EQ(sprite->bubble()->type(), type);

        // Kill waiting thread
        code->reset(ctx.get());

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        m_engine->threadAboutToStop()(&thread);
        code->kill(ctx.get());
        ASSERT_EQ(sprite->bubble()->owner(), nullptr);
        ASSERT_TRUE(sprite->bubble()->text().empty());

        // Kill waiting thread after changing text
        code->reset(ctx.get());

        EXPECT_CALL(timer, start(2.5));
        EXPECT_CALL(m_engineMock, requestRedraw());
        code->run(ctx.get());
        ASSERT_FALSE(code->isFinished(ctx.get()));

        EXPECT_CALL(m_engineMock, requestRedraw());
        sprite->bubble()->setText("test");

        m_engine->threadAboutToStop()(&thread);
        code->kill(ctx.get());
        ASSERT_EQ(sprite->bubble()->owner(), nullptr);
        ASSERT_EQ(sprite->bubble()->text(), "test");
        ASSERT_EQ(sprite->bubble()->type(), type);
    }
}

TEST_F(LooksBlocksTest, Say)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("looks_say");
    builder.addValueInput("MESSAGE", "Hello world");

    builder.build();
    builder.run();
    ASSERT_EQ(sprite->bubble()->text(), "Hello world");
    ASSERT_EQ(sprite->bubble()->type(), TextBubble::Type::Say);
    ASSERT_EQ(sprite->bubble()->owner(), nullptr);
}
