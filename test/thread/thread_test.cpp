#include <scratchcpp/thread.h>
#include <scratchcpp/script.h>
#include <scratchcpp/dev/executioncontext.h>
#include <scratchcpp/dev/promise.h>
#include <targetmock.h>
#include <enginemock.h>
#include <executablecodemock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::Invoke;
using ::testing::_;

class ThreadTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_script = std::make_unique<Script>(nullptr, nullptr, nullptr);
            m_code = std::make_shared<ExecutableCodeMock>();
            m_script->setCode(m_code);
            EXPECT_CALL(*m_code, createExecutionContext(_)).WillOnce(Invoke([this](Thread *thread) {
                m_ctx = std::make_shared<ExecutionContext>(thread);
                return m_ctx;
            }));
            m_thread = std::make_unique<Thread>(&m_target, &m_engine, m_script.get());
        }

        std::unique_ptr<Thread> m_thread;
        TargetMock m_target;
        EngineMock m_engine;
        std::unique_ptr<Script> m_script;
        std::shared_ptr<ExecutableCodeMock> m_code;
        std::shared_ptr<ExecutionContext> m_ctx;
};

TEST_F(ThreadTest, Constructor)
{
    ASSERT_EQ(m_thread->target(), &m_target);
    ASSERT_EQ(m_thread->engine(), &m_engine);
    ASSERT_EQ(m_thread->script(), m_script.get());
}

TEST_F(ThreadTest, Run)
{
    EXPECT_CALL(*m_code, run(m_ctx.get()));
    m_thread->run();
}

TEST_F(ThreadTest, Kill)
{
    EXPECT_CALL(*m_code, kill(m_ctx.get()));
    m_thread->kill();
}

TEST_F(ThreadTest, Reset)
{
    EXPECT_CALL(*m_code, reset(m_ctx.get()));
    m_thread->reset();
}

TEST_F(ThreadTest, IsFinished)
{
    EXPECT_CALL(*m_code, isFinished(m_ctx.get())).WillOnce(Return(false));
    ASSERT_FALSE(m_thread->isFinished());

    EXPECT_CALL(*m_code, isFinished(m_ctx.get())).WillOnce(Return(true));
    ASSERT_TRUE(m_thread->isFinished());
}

TEST_F(ThreadTest, Promise)
{
    ASSERT_EQ(m_thread->promise(), m_ctx->promise());

    auto promise = std::make_shared<Promise>();
    m_ctx->setPromise(promise);
    ASSERT_EQ(m_thread->promise(), m_ctx->promise());

    m_ctx->setPromise(nullptr);
    ASSERT_EQ(m_thread->promise(), m_ctx->promise());

    m_thread->setPromise(promise);
    ASSERT_EQ(m_thread->promise(), promise);
    ASSERT_EQ(m_ctx->promise(), promise);
}
