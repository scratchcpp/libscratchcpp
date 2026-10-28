#include <scratchcpp/target.h>
#include <scratchcpp/value_functions.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/promise.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/script.h>
#include <engine/internal/codegen/cpp/cppexecutablecode.h>
#include <engine/internal/codegen/cpp/cppcompilercontext.h>
#include <engine/internal/codegen/cpp/cppexecutioncontext.h>
#include <enginemock.h>

#include "cppexecutablecodemock.h"

using namespace libscratchcpp;

using ::testing::Return;

class CppExecutableCodeTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_target.setName("test");
            m_ctx = std::make_unique<CppCompilerContext>(&m_engine, &m_target);
            m_code = std::make_shared<CppExecutableCodeMock>(m_ctx.get());

            m_script = std::make_unique<Script>(&m_target, nullptr, &m_engine);
            m_script->setCode(m_code);
        }

        std::unique_ptr<CppCompilerContext> m_ctx;
        Target m_target;
        EngineMock m_engine;
        std::unique_ptr<Script> m_script;
        std::shared_ptr<CppExecutableCodeMock> m_code;
};

// TODO: Add string support (see the StringCount test case)

TEST_F(CppExecutableCodeTest, CreateExecutionContext)
{
    Thread thread(&m_target, &m_engine, m_script.get());
    auto ctx = m_code->createExecutionContext(&thread);
    ASSERT_TRUE(ctx);
    ASSERT_EQ(ctx->thread(), &thread);
    ASSERT_TRUE(dynamic_cast<CppExecutionContext *>(ctx.get()));
}

TEST_F(CppExecutableCodeTest, ScriptFunction)
{
    m_target.addVariable(std::make_shared<Variable>("", ""));
    m_target.addList(std::make_shared<List>("", ""));

    Thread thread(&m_target, &m_engine, m_script.get());
    auto ctx = m_code->createExecutionContext(&thread);
    ASSERT_FALSE(m_code->isFinished(ctx.get()));

    EXPECT_CALL(*m_code, runScript(ctx.get(), &m_target, m_target.variableData(), m_target.listData()));
    m_code->run(ctx.get());
    ASSERT_TRUE(m_code->isFinished(ctx.get()));

    m_code->kill(ctx.get());
    ASSERT_TRUE(m_code->isFinished(ctx.get()));

    m_code->reset(ctx.get());
    ASSERT_FALSE(m_code->isFinished(ctx.get()));

    m_code->kill(ctx.get());
    ASSERT_TRUE(m_code->isFinished(ctx.get()));

    EXPECT_CALL(*m_code, runScript).Times(0);
    m_code->run(ctx.get());
    ASSERT_TRUE(m_code->isFinished(ctx.get()));

    // Test with another context
    Target anotherTarget;
    anotherTarget.addVariable(std::make_shared<Variable>("", ""));
    anotherTarget.addList(std::make_shared<List>("", ""));
    Script script(&anotherTarget, nullptr, &m_engine);
    script.setCode(m_code);
    Thread anotherThread(&anotherTarget, &m_engine, &script);
    auto anotherCtx = m_code->createExecutionContext(&anotherThread);
    ASSERT_FALSE(m_code->isFinished(anotherCtx.get()));
    ASSERT_TRUE(m_code->isFinished(ctx.get()));

    EXPECT_CALL(*m_code, runScript(anotherCtx.get(), &anotherTarget, anotherTarget.variableData(), anotherTarget.listData()));
    m_code->run(anotherCtx.get());
    ASSERT_TRUE(m_code->isFinished(anotherCtx.get()));
    ASSERT_TRUE(m_code->isFinished(ctx.get()));

    m_code->reset(ctx.get());
    ASSERT_TRUE(m_code->isFinished(anotherCtx.get()));
    ASSERT_FALSE(m_code->isFinished(ctx.get()));
}

TEST_F(CppExecutableCodeTest, PredicateFunction)
{
    m_target.addVariable(std::make_shared<Variable>("", ""));
    m_target.addList(std::make_shared<List>("", ""));

    Thread thread(&m_target, &m_engine, m_script.get());
    auto ctx = m_code->createExecutionContext(&thread);

    EXPECT_CALL(*m_code, runPredicate(ctx.get(), &m_target, m_target.variableData(), m_target.listData())).WillOnce(Return(true));
    ASSERT_TRUE(std::static_pointer_cast<CppExecutableCode>(m_code)->runPredicate(ctx.get()));

    EXPECT_CALL(*m_code, runPredicate(ctx.get(), &m_target, m_target.variableData(), m_target.listData())).WillOnce(Return(true));
    ASSERT_TRUE(std::static_pointer_cast<CppExecutableCode>(m_code)->runPredicate(ctx.get()));

    EXPECT_CALL(*m_code, runPredicate(ctx.get(), &m_target, m_target.variableData(), m_target.listData())).WillOnce(Return(false));
    ASSERT_FALSE(std::static_pointer_cast<CppExecutableCode>(m_code)->runPredicate(ctx.get()));

    EXPECT_CALL(*m_code, runPredicate(ctx.get(), &m_target, m_target.variableData(), m_target.listData())).WillOnce(Return(false));
    ASSERT_FALSE(std::static_pointer_cast<CppExecutableCode>(m_code)->runPredicate(ctx.get()));
}

TEST_F(CppExecutableCodeTest, ReporterFunction)
{
    m_target.addVariable(std::make_shared<Variable>("", ""));
    m_target.addList(std::make_shared<List>("", ""));

    Thread thread(&m_target, &m_engine, m_script.get());
    auto ctx = m_code->createExecutionContext(&thread);

    Value v = "test";
    EXPECT_CALL(*m_code, runReporter(ctx.get(), &m_target, m_target.variableData(), m_target.listData())).WillOnce(Return(v.data()));
    ASSERT_EQ(Value(std::static_pointer_cast<CppExecutableCode>(m_code)->runReporter(ctx.get())).toString(), v.toString());
}

TEST_F(CppExecutableCodeTest, Promise)
{
    Thread thread(&m_target, &m_engine, m_script.get());
    auto ctx = m_code->createExecutionContext(&thread);
    ASSERT_FALSE(m_code->isFinished(ctx.get()));

    // run()
    auto promise = std::make_shared<Promise>();
    ctx->setPromise(promise);
    EXPECT_CALL(*m_code, runScript).Times(0);

    for (int i = 0; i < 10; i++) {
        m_code->run(ctx.get());
        ASSERT_FALSE(m_code->isFinished(ctx.get()));
    }

    promise->resolve();

    EXPECT_CALL(*m_code, runScript);
    m_code->run(ctx.get());
    ASSERT_TRUE(m_code->isFinished(ctx.get()));
    ASSERT_EQ(ctx->promise(), nullptr);
    m_code->reset(ctx.get());

    // kill()
    promise = std::make_shared<Promise>();
    ctx->setPromise(promise);
    EXPECT_CALL(*m_code, runScript).Times(0);

    for (int i = 0; i < 10; i++) {
        m_code->run(ctx.get());
        ASSERT_FALSE(m_code->isFinished(ctx.get()));
    }

    m_code->kill(ctx.get());
    ASSERT_TRUE(m_code->isFinished(ctx.get()));
    ASSERT_EQ(ctx->promise(), nullptr);
    m_code->reset(ctx.get());

    // reset()
    promise = std::make_shared<Promise>();
    ctx->setPromise(promise);
    EXPECT_CALL(*m_code, runScript).Times(0);

    for (int i = 0; i < 10; i++) {
        m_code->run(ctx.get());
        ASSERT_FALSE(m_code->isFinished(ctx.get()));
    }

    m_code->reset(ctx.get());
    ASSERT_FALSE(m_code->isFinished(ctx.get()));
    ASSERT_EQ(ctx->promise(), nullptr);

    EXPECT_CALL(*m_code, runScript);
    m_code->run(ctx.get());
    ASSERT_TRUE(m_code->isFinished(ctx.get()));

    // Test with another context
    Target anotherTarget;
    Script script(&anotherTarget, nullptr, &m_engine);
    script.setCode(m_code);
    Thread anotherThread(&anotherTarget, &m_engine, &script);
    auto anotherCtx = m_code->createExecutionContext(&anotherThread);
    ASSERT_FALSE(m_code->isFinished(anotherCtx.get()));
    ASSERT_TRUE(m_code->isFinished(ctx.get()));

    promise = std::make_shared<Promise>();
    anotherCtx->setPromise(promise);
    EXPECT_CALL(*m_code, runScript).Times(0);

    for (int i = 0; i < 10; i++) {
        m_code->run(anotherCtx.get());
        ASSERT_FALSE(m_code->isFinished(anotherCtx.get()));
    }

    promise->resolve();

    EXPECT_CALL(*m_code, runScript);
    m_code->run(anotherCtx.get());
    ASSERT_TRUE(m_code->isFinished(anotherCtx.get()));
    ASSERT_TRUE(m_code->isFinished(ctx.get()));

    m_code->reset(ctx.get());
    ASSERT_TRUE(m_code->isFinished(anotherCtx.get()));
    ASSERT_FALSE(m_code->isFinished(ctx.get()));
}
