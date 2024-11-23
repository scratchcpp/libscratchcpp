#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <scratchcpp/comment.h>
#include <scratchcpp/target.h>
#ifdef USE_LLVM
#include <scratchcpp/dev/compiler.h>
#else
#include <scratchcpp/compiler.h>
#endif
#include <enginemock.h>

#include "../common.h"

using namespace libscratchcpp;

class BlockTestMock
{
    public:
#ifdef USE_LLVM
        MOCK_METHOD(CompilerValue *, compileTest, (Compiler *), ());
#else
        MOCK_METHOD(void, compileTest, (Compiler *), ());
#endif
};

class BlockTest : public testing::Test
{
    public:
        void SetUp() override { m_mockPtr = &m_mock; }

#ifdef USE_LLVM
        static CompilerValue *compileTest(Compiler *compiler)
        {
            EXPECT_TRUE(m_mockPtr);
            return m_mockPtr->compileTest(compiler);
        }
#else
        static void compileTest(Compiler *compiler)
        {
            ASSERT_TRUE(m_mockPtr);
            m_mockPtr->compileTest(compiler);
        }
#endif // USE_LLVM

        static inline BlockTestMock *m_mockPtr = nullptr;
        BlockTestMock m_mock;
};

TEST_F(BlockTest, Constructors)
{
    Block block("abc", "motion_movesteps");
    ASSERT_EQ(block.id(), "abc");
    ASSERT_EQ(block.opcode(), "motion_movesteps");
}

TEST_F(BlockTest, Next)
{
    Block block("", "");
    ASSERT_EQ(block.next(), nullptr);
    ASSERT_EQ(block.nextId(), "");

    block.setNextId("hello");
    ASSERT_EQ(block.next(), nullptr);
    ASSERT_EQ(block.nextId(), "hello");

    auto nextBlock = std::make_shared<Block>("abc", "");
    block.setNext(nextBlock);
    ASSERT_EQ(block.next(), nextBlock);
    ASSERT_EQ(block.nextId(), "abc");

    block.setNextId("def");
    ASSERT_EQ(block.next(), nullptr);
    ASSERT_EQ(block.nextId(), "def");

    block.setNext(nullptr);
    ASSERT_EQ(block.next(), nullptr);
    ASSERT_EQ(block.nextId(), "");
}

TEST_F(BlockTest, Parent)
{
    Block block("", "");
    ASSERT_EQ(block.parent(), nullptr);
    ASSERT_EQ(block.parentId(), "");

    block.setParentId("hello");
    ASSERT_EQ(block.parent(), nullptr);
    ASSERT_EQ(block.parentId(), "hello");

    auto parentBlock = std::make_shared<Block>("abc", "");
    block.setParent(parentBlock);
    ASSERT_EQ(block.parent(), parentBlock);
    ASSERT_EQ(block.parentId(), "abc");

    block.setParentId("def");
    ASSERT_EQ(block.parent(), nullptr);
    ASSERT_EQ(block.parentId(), "def");

    block.setParent(nullptr);
    ASSERT_EQ(block.parent(), nullptr);
    ASSERT_EQ(block.parentId(), "");
}

TEST_F(BlockTest, Inputs)
{
    auto i1 = std::make_shared<Input>("VALUE1", Input::Type::Shadow);
    i1->setInputId(11);

    auto i2 = std::make_shared<Input>("VALUE2", Input::Type::Shadow);
    i2->setInputId(12);

    auto i3 = std::make_shared<Input>("VALUE3", Input::Type::Shadow);
    i3->setInputId(15);

    Block block("", "");
    ASSERT_EQ(block.addInput(i1), 0);
    ASSERT_EQ(block.addInput(i2), 1);
    ASSERT_EQ(block.addInput(i3), 2);
    ASSERT_EQ(block.addInput(i2), 1); // add existing input

    ASSERT_EQ(block.inputs(), std::vector<std::shared_ptr<Input>>({ i1, i2, i3 }));
    ASSERT_EQ(block.inputAt(0), i1);
    ASSERT_EQ(block.inputAt(1), i2);
    ASSERT_EQ(block.inputAt(2), i3);
    ASSERT_EQ(block.inputAt(3), nullptr);
    ASSERT_EQ(block.inputAt(-1), nullptr);

    ASSERT_EQ(block.findInput("invalid"), -1);
    ASSERT_EQ(block.findInput("VALUE1"), 0);
    ASSERT_EQ(block.findInput("VALUE2"), 1);
    ASSERT_EQ(block.findInput("VALUE3"), 2);

    ASSERT_EQ(block.findInputById(5), nullptr);
    ASSERT_EQ(block.findInputById(11), i1.get());
    ASSERT_EQ(block.findInputById(12), i2.get());
    ASSERT_EQ(block.findInputById(15), i3.get());

    auto i4 = std::make_shared<Input>("VALUE4", Input::Type::Shadow);
    block.addInput(i4);
    i4->setInputId(20);
    ASSERT_EQ(block.findInputById(20), i4.get());
}

TEST_F(BlockTest, Fields)
{
    auto f1 = std::make_shared<Field>("VARIABLE1", Value());
    f1->setFieldId(11);

    auto f2 = std::make_shared<Field>("VARIABLE2", Value());
    f2->setFieldId(12);

    auto f3 = std::make_shared<Field>("VARIABLE3", Value());
    f3->setFieldId(15);

    Block block("", "");
    ASSERT_EQ(block.addField(f1), 0);
    ASSERT_EQ(block.addField(f2), 1);
    ASSERT_EQ(block.addField(f3), 2);
    ASSERT_EQ(block.addField(f2), 1); // add existing field

    ASSERT_EQ(block.fields(), std::vector<std::shared_ptr<Field>>({ f1, f2, f3 }));
    ASSERT_EQ(block.fieldAt(0), f1);
    ASSERT_EQ(block.fieldAt(1), f2);
    ASSERT_EQ(block.fieldAt(2), f3);
    ASSERT_EQ(block.fieldAt(3), nullptr);
    ASSERT_EQ(block.fieldAt(-1), nullptr);

    ASSERT_EQ(block.findField("invalid"), -1);
    ASSERT_EQ(block.findField("VARIABLE1"), 0);
    ASSERT_EQ(block.findField("VARIABLE2"), 1);
    ASSERT_EQ(block.findField("VARIABLE3"), 2);

    ASSERT_EQ(block.findFieldById(5), nullptr);
    ASSERT_EQ(block.findFieldById(11), f1.get());
    ASSERT_EQ(block.findFieldById(12), f2.get());
    ASSERT_EQ(block.findFieldById(15), f3.get());

    auto f4 = std::make_shared<Field>("VARIABLE4", Value());
    block.addField(f4);
    f4->setFieldId(20);
    ASSERT_EQ(block.findFieldById(20), f4.get());
}

TEST_F(BlockTest, Shadow)
{
    Block block("", "");
    ASSERT_FALSE(block.shadow());

    block.setShadow(true);
    ASSERT_TRUE(block.shadow());
}

TEST_F(BlockTest, TopLevel)
{
    Block block("", "");
    ASSERT_TRUE(block.topLevel());

    block.setParentId("hello");
    ASSERT_FALSE(block.topLevel());

    auto parentBlock = std::make_shared<Block>("abc", "");
    block.setParent(parentBlock);
    ASSERT_FALSE(block.topLevel());

    block.setParentId("def");
    ASSERT_FALSE(block.topLevel());

    block.setParent(nullptr);
    ASSERT_TRUE(block.topLevel());
}

TEST_F(BlockTest, X)
{
    Block block("", "");
    ASSERT_EQ(block.x(), 0);

    block.setX(12);
    ASSERT_EQ(block.x(), 12);
}

TEST_F(BlockTest, Y)
{
    Block block("", "");
    ASSERT_EQ(block.y(), 0);

    block.setY(8);
    ASSERT_EQ(block.y(), 8);
}

TEST_F(BlockTest, Comment)
{
    Block block("", "");
    ASSERT_EQ(block.comment(), nullptr);
    ASSERT_TRUE(block.commentId().empty());

    block.setCommentId("hello");
    ASSERT_EQ(block.comment(), nullptr);
    ASSERT_EQ(block.commentId(), "hello");

    auto comment = std::make_shared<Comment>("abc");
    block.setComment(comment);
    ASSERT_EQ(block.comment(), comment);
    ASSERT_EQ(block.commentId(), "abc");

    block.setCommentId("def");
    ASSERT_EQ(block.comment(), nullptr);
    ASSERT_EQ(block.commentId(), "def");

    block.setComment(nullptr);
    ASSERT_EQ(block.comment(), nullptr);
    ASSERT_EQ(block.commentId(), "");
}

TEST_F(BlockTest, Engine)
{
    Block block("", "");
    ASSERT_EQ(block.engine(), nullptr);

    EngineMock engine;
    block.setEngine(&engine);
    ASSERT_EQ(block.engine(), &engine);
}

TEST_F(BlockTest, Target)
{
    Block block("", "");
    ASSERT_EQ(block.target(), nullptr);

    Target target;
    block.setTarget(&target);
    ASSERT_EQ(block.target(), &target);
}

TEST_F(BlockTest, CompileFunction)
{
    Block block("", "");
    ASSERT_EQ(block.compileFunction(), nullptr);

    block.setCompileFunction(&compileTest);
    ASSERT_EQ(block.compileFunction(), &compileTest);
}

TEST_F(BlockTest, HatPredicateCompileFunction)
{
    Block block("", "");
    ASSERT_EQ(block.hatPredicateCompileFunction(), nullptr);

    block.setHatPredicateCompileFunction(&compileTest);
    ASSERT_EQ(block.hatPredicateCompileFunction(), &compileTest);
}

TEST_F(BlockTest, Compile)
{
    Block block("", "");
    EngineMock engine;
    Compiler compiler(&engine, nullptr);

    block.compile(&compiler); // test with null compile function

    block.setCompileFunction(&compileTest);
    EXPECT_CALL(m_mock, compileTest(&compiler)).Times(1);
    block.compile(&compiler);
}

TEST_F(BlockTest, MutationHasNext)
{
    Block block("", "");
    ASSERT_TRUE(block.mutationHasNext());

    block.setMutationHasNext(false);
    ASSERT_FALSE(block.mutationHasNext());
}

TEST_F(BlockTest, MutationPrototype)
{
    Block block("", "");
    ASSERT_TRUE(block.mutationPrototype());
}

TEST_F(BlockTest, TopLevelReporter)
{
    Block block("", "");
    ASSERT_FALSE(block.isTopLevelReporter());
    ASSERT_EQ(block.topLevelReporterInfo(), nullptr);

    for (int i = 0; i < 2; i++) {
        block.setIsTopLevelReporter(true);
        ASSERT_TRUE(block.isTopLevelReporter());
        ASSERT_TRUE(block.topLevelReporterInfo());

        block.setIsTopLevelReporter(false);
        ASSERT_FALSE(block.isTopLevelReporter());
        ASSERT_EQ(block.topLevelReporterInfo(), nullptr);
    }
}
