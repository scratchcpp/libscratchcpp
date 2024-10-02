#include <scratchcpp/thread.h>
#include <scratchcpp/script.h>
#include <scratchcpp/virtualmachine.h>
#include <scratchcpp/dev/executioncontext.h>
#include <targetmock.h>
#include <enginemock.h>
#include <executablecodemock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;

class ThreadTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_script = std::make_unique<Script>(nullptr, nullptr, nullptr);
#ifdef USE_LLVM
            m_code = std::make_shared<ExecutableCodeMock>();
            m_script->setCode(m_code);
            m_ctx = std::make_shared<ExecutionContext>(&m_target);
            EXPECT_CALL(*m_code, createExecutionContext(&m_target)).WillOnce(Return(m_ctx));
#endif
            m_thread = std::make_unique<Thread>(&m_target, &m_engine, m_script.get());
        }

        std::unique_ptr<Thread> m_thread;
        TargetMock m_target;
        EngineMock m_engine;
        std::unique_ptr<Script> m_script;
#ifdef USE_LLVM
        std::shared_ptr<ExecutableCodeMock> m_code;
        std::shared_ptr<ExecutionContext> m_ctx;
#endif
};

TEST_F(ThreadTest, Constructor)
{
    ASSERT_EQ(m_thread->target(), &m_target);
    ASSERT_EQ(m_thread->engine(), &m_engine);
    ASSERT_EQ(m_thread->script(), m_script.get());
    ASSERT_TRUE(m_thread->vm());
    ASSERT_EQ(m_thread->vm()->target(), &m_target);
    ASSERT_EQ(m_thread->vm()->engine(), &m_engine);
    ASSERT_EQ(m_thread->vm()->script(), m_script.get());
}

#ifdef USE_LLVM
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

// TODO: Test promise() and resolvePromise()
#endif // USE_LLVM
