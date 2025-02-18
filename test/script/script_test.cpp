#include <scratchcpp/script.h>
#include <scratchcpp/block.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/executioncontext.h>
#include <enginemock.h>
#include <executablecodemock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::Invoke;
using ::testing::_;

class ScriptTest : public testing::Test
{
    public:
        Target m_target;
        EngineMock m_engine;
};

TEST_F(ScriptTest, Constructors)
{
    auto block = std::make_shared<Block>("", "");
    Script script(&m_target, block, &m_engine);
    ASSERT_EQ(script.target(), &m_target);
    ASSERT_EQ(script.topBlock(), block);
}

TEST_F(ScriptTest, Code)
{
    Script script(nullptr, nullptr, nullptr);
    ASSERT_EQ(script.code(), nullptr);

    auto code = std::make_shared<ExecutableCodeMock>();
    script.setCode(code);
    ASSERT_EQ(script.code(), code.get());
}

TEST_F(ScriptTest, HatPredicateCode)
{
    Script script(nullptr, nullptr, nullptr);
    ASSERT_EQ(script.hatPredicateCode(), nullptr);

    auto code = std::make_shared<ExecutableCodeMock>();
    script.setHatPredicateCode(code);
    ASSERT_EQ(script.hatPredicateCode(), code.get());
}

TEST_F(ScriptTest, RunHatPredicate)
{
    Script script(nullptr, nullptr, &m_engine);
    auto code = std::make_shared<ExecutableCodeMock>();
    std::shared_ptr<ExecutionContext> ctx;
    script.setHatPredicateCode(code);

    EXPECT_CALL(*code, createExecutionContext(_)).WillRepeatedly(Invoke([&ctx](Thread *thread) {
        ctx = std::make_shared<ExecutionContext>(thread);
        return ctx;
    }));

    EXPECT_CALL(*code, runPredicate(_)).WillOnce(Return(true));
    ASSERT_TRUE(script.runHatPredicate(&m_target));

    EXPECT_CALL(*code, runPredicate(_)).WillOnce(Return(true));
    ASSERT_TRUE(script.runHatPredicate(&m_target));

    EXPECT_CALL(*code, runPredicate(_)).WillOnce(Return(false));
    ASSERT_FALSE(script.runHatPredicate(&m_target));
}

TEST_F(ScriptTest, Start)
{
    Script script1(nullptr, nullptr, nullptr);

    std::shared_ptr<Thread> thread = script1.start();
    ASSERT_TRUE(thread);
    ASSERT_EQ(thread->target(), nullptr);
    ASSERT_EQ(thread->engine(), nullptr);
    ASSERT_EQ(thread->script(), &script1);

    Script script2(&m_target, nullptr, &m_engine);

    thread = script2.start();
    ASSERT_TRUE(thread);
    ASSERT_EQ(thread->target(), &m_target);
    ASSERT_EQ(thread->engine(), &m_engine);

    Script script3(&m_target, nullptr, &m_engine);
    thread = script3.start();
    ASSERT_TRUE(thread);

    Target target;
    thread = script3.start(&target);
    ASSERT_TRUE(thread);
    ASSERT_EQ(thread->target(), &target);

    Sprite root;
    root.setEngine(&m_engine);

    EXPECT_CALL(m_engine, cloneLimit()).Times(2).WillRepeatedly(Return(300));
    EXPECT_CALL(m_engine, cloneCount()).WillOnce(Return(0));
    EXPECT_CALL(m_engine, initClone).Times(1);
    EXPECT_CALL(m_engine, moveDrawableBehindOther(_, &root));
    EXPECT_CALL(m_engine, requestRedraw());
    auto clone = root.clone();

    Script script4(&root, nullptr, &m_engine);
    thread = script4.start(clone.get());
    ASSERT_TRUE(thread);
    ASSERT_EQ(thread->target(), clone.get());

    EXPECT_CALL(m_engine, deinitClone(clone));
    clone->deleteClone();
}
