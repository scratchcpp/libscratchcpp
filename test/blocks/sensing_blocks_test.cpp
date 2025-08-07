#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/list.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <enginemock.h>
#include <targetmock.h>

#include "../common.h"
#include "blocks/sensingblocks.h"
#include "util.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

using ::testing::Return;

class SensingBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<SensingBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
            registerBlocks(m_engine, m_extension.get());
        }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
};

TEST_F(SensingBlocksTest, TouchingObject_Sprite_CompileTime)
{
    auto targetMock = std::make_shared<TargetMock>();
    targetMock->setEngine(&m_engineMock);

    Sprite sprite;

    ScriptBuilder builder(m_extension.get(), m_engine, targetMock);
    builder.addBlock("sensing_touchingobject");
    builder.addDropdownInput("TOUCHINGOBJECTMENU", "Sprite2");
    Block *block = builder.currentBlock();

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(3));
    EXPECT_CALL(m_engineMock, targetAt(3)).WillOnce(Return(&sprite));
    Compiler compiler(&m_engineMock, targetMock.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(targetMock.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(targetMock.get(), &m_engineMock, &script);

    EXPECT_CALL(*targetMock, touchingClones).WillOnce(Return(true));
    ValueData value = thread.runReporter();
    ASSERT_TRUE(value_toBool(&value));
    value_free(&value);

    EXPECT_CALL(*targetMock, touchingClones).WillOnce(Return(false));
    value = thread.runReporter();
    ASSERT_FALSE(value_toBool(&value));
    value_free(&value);
}

TEST_F(SensingBlocksTest, TouchingObject_Sprite_Runtime)
{
    auto targetMock = std::make_shared<TargetMock>();
    targetMock->setEngine(&m_engineMock);

    Sprite sprite;

    ScriptBuilder builder(m_extension.get(), m_engine, targetMock);

    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "Sprite2");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("sensing_touchingobject");
    builder.addObscuredInput("TOUCHINGOBJECTMENU", valueBlock);
    Block *block = builder.currentBlock();

    Compiler compiler(&m_engineMock, targetMock.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(targetMock.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(targetMock.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillRepeatedly(Return(5));
    EXPECT_CALL(m_engineMock, targetAt(5)).WillRepeatedly(Return(&sprite));

    EXPECT_CALL(*targetMock, touchingClones).WillOnce(Return(true));
    ValueData value = thread.runReporter();
    ASSERT_TRUE(value_toBool(&value));
    value_free(&value);

    EXPECT_CALL(*targetMock, touchingClones).WillOnce(Return(false));
    value = thread.runReporter();
    ASSERT_FALSE(value_toBool(&value));
    value_free(&value);
}

TEST_F(SensingBlocksTest, TouchingObject_Stage_CompileTime)
{
    auto targetMock = std::make_shared<TargetMock>();
    targetMock->setEngine(&m_engineMock);

    Stage stage;

    ScriptBuilder builder(m_extension.get(), m_engine, targetMock);
    builder.addBlock("sensing_touchingobject");
    builder.addDropdownInput("TOUCHINGOBJECTMENU", "_stage_");
    Block *block = builder.currentBlock();

    EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillRepeatedly(Return(3));
    EXPECT_CALL(m_engineMock, targetAt(3)).WillRepeatedly(Return(&stage));
    Compiler compiler(&m_engineMock, targetMock.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(targetMock.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(targetMock.get(), &m_engineMock, &script);

    EXPECT_CALL(*targetMock, touchingClones).Times(0);
    ValueData value = thread.runReporter();
    ASSERT_FALSE(value_toBool(&value));
    value_free(&value);
}

TEST_F(SensingBlocksTest, TouchingObject_Stage_Runtime)
{
    auto targetMock = std::make_shared<TargetMock>();
    targetMock->setEngine(&m_engineMock);

    Stage stage;

    ScriptBuilder builder(m_extension.get(), m_engine, targetMock);

    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "_stage_");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("sensing_touchingobject");
    builder.addObscuredInput("TOUCHINGOBJECTMENU", valueBlock);
    Block *block = builder.currentBlock();

    Compiler compiler(&m_engineMock, targetMock.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(targetMock.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(targetMock.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillRepeatedly(Return(5));
    EXPECT_CALL(m_engineMock, targetAt(5)).WillRepeatedly(Return(&stage));

    EXPECT_CALL(*targetMock, touchingClones).Times(0);
    ValueData value = thread.runReporter();
    ASSERT_FALSE(value_toBool(&value));
    value_free(&value);
}

TEST_F(SensingBlocksTest, TouchingObject_Mouse_CompileTime)
{
    auto targetMock = std::make_shared<TargetMock>();
    targetMock->setEngine(&m_engineMock);

    Sprite sprite;

    ScriptBuilder builder(m_extension.get(), m_engine, targetMock);
    builder.addBlock("sensing_touchingobject");
    builder.addDropdownInput("TOUCHINGOBJECTMENU", "_mouse_");
    Block *block = builder.currentBlock();

    EXPECT_CALL(m_engineMock, findTarget).Times(0);
    EXPECT_CALL(m_engineMock, targetAt).Times(0);
    Compiler compiler(&m_engineMock, targetMock.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(targetMock.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(targetMock.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, mouseX).WillOnce(Return(56.2));
    EXPECT_CALL(m_engineMock, mouseY).WillOnce(Return(-89.5));
    EXPECT_CALL(*targetMock, touchingPoint(56.2, -89.5)).WillOnce(Return(true));
    ValueData value = thread.runReporter();
    ASSERT_TRUE(value_toBool(&value));
    value_free(&value);

    EXPECT_CALL(m_engineMock, mouseX).WillOnce(Return(-12.7));
    EXPECT_CALL(m_engineMock, mouseY).WillOnce(Return(2.2));
    EXPECT_CALL(*targetMock, touchingPoint(-12.7, 2.2)).WillOnce(Return(false));
    value = thread.runReporter();
    ASSERT_FALSE(value_toBool(&value));
    value_free(&value);
}

TEST_F(SensingBlocksTest, TouchingObject_Mouse_Runtime)
{
    auto targetMock = std::make_shared<TargetMock>();
    targetMock->setEngine(&m_engineMock);

    Sprite sprite;

    ScriptBuilder builder(m_extension.get(), m_engine, targetMock);

    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "_mouse_");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("sensing_touchingobject");
    builder.addObscuredInput("TOUCHINGOBJECTMENU", valueBlock);
    Block *block = builder.currentBlock();

    Compiler compiler(&m_engineMock, targetMock.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(targetMock.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(targetMock.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, findTarget).Times(0);
    EXPECT_CALL(m_engineMock, targetAt).Times(0);

    EXPECT_CALL(m_engineMock, mouseX).WillOnce(Return(56.2));
    EXPECT_CALL(m_engineMock, mouseY).WillOnce(Return(-89.5));
    EXPECT_CALL(*targetMock, touchingPoint(56.2, -89.5)).WillOnce(Return(true));
    ValueData value = thread.runReporter();
    ASSERT_TRUE(value_toBool(&value));
    value_free(&value);

    EXPECT_CALL(m_engineMock, mouseX).WillOnce(Return(-12.7));
    EXPECT_CALL(m_engineMock, mouseY).WillOnce(Return(2.2));
    EXPECT_CALL(*targetMock, touchingPoint(-12.7, 2.2)).WillOnce(Return(false));
    value = thread.runReporter();
    ASSERT_FALSE(value_toBool(&value));
    value_free(&value);
}

TEST_F(SensingBlocksTest, TouchingObject_Edge_CompileTime)
{
    auto targetMock = std::make_shared<TargetMock>();
    targetMock->setEngine(&m_engineMock);

    Sprite sprite;

    ScriptBuilder builder(m_extension.get(), m_engine, targetMock);
    builder.addBlock("sensing_touchingobject");
    builder.addDropdownInput("TOUCHINGOBJECTMENU", "_edge_");
    Block *block = builder.currentBlock();

    EXPECT_CALL(m_engineMock, findTarget).Times(0);
    EXPECT_CALL(m_engineMock, targetAt).Times(0);
    Compiler compiler(&m_engineMock, targetMock.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(targetMock.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(targetMock.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, stageWidth()).WillOnce(Return(2));
    EXPECT_CALL(m_engineMock, stageHeight()).WillOnce(Return(2));
    EXPECT_CALL(*targetMock, boundingRect()).WillOnce(Return(Rect(-5, 5, 5, -5)));
    ValueData value = thread.runReporter();
    ASSERT_TRUE(value_toBool(&value));
    value_free(&value);

    EXPECT_CALL(m_engineMock, stageWidth()).WillOnce(Return(10));
    EXPECT_CALL(m_engineMock, stageHeight()).WillOnce(Return(10));
    EXPECT_CALL(*targetMock, boundingRect()).WillOnce(Return(Rect(-5, 5, 5, -5)));
    value = thread.runReporter();
    ASSERT_FALSE(value_toBool(&value));
    value_free(&value);
}

TEST_F(SensingBlocksTest, TouchingObject_Edge_Runtime)
{
    auto targetMock = std::make_shared<TargetMock>();
    targetMock->setEngine(&m_engineMock);

    Sprite sprite;

    ScriptBuilder builder(m_extension.get(), m_engine, targetMock);

    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "_edge_");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("sensing_touchingobject");
    builder.addObscuredInput("TOUCHINGOBJECTMENU", valueBlock);
    Block *block = builder.currentBlock();

    Compiler compiler(&m_engineMock, targetMock.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(targetMock.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(targetMock.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, findTarget).Times(0);
    EXPECT_CALL(m_engineMock, targetAt).Times(0);

    EXPECT_CALL(m_engineMock, stageWidth()).WillOnce(Return(2));
    EXPECT_CALL(m_engineMock, stageHeight()).WillOnce(Return(2));
    EXPECT_CALL(*targetMock, boundingRect()).WillOnce(Return(Rect(-5, 5, 5, -5)));
    ValueData value = thread.runReporter();
    ASSERT_TRUE(value_toBool(&value));
    value_free(&value);

    EXPECT_CALL(m_engineMock, stageWidth()).WillOnce(Return(10));
    EXPECT_CALL(m_engineMock, stageHeight()).WillOnce(Return(10));
    EXPECT_CALL(*targetMock, boundingRect()).WillOnce(Return(Rect(-5, 5, 5, -5)));
    value = thread.runReporter();
    ASSERT_FALSE(value_toBool(&value));
    value_free(&value);
}

TEST_F(SensingBlocksTest, TouchingObject_Invalid_CompileTime)
{
    auto targetMock = std::make_shared<TargetMock>();
    targetMock->setEngine(&m_engineMock);

    Sprite sprite;

    ScriptBuilder builder(m_extension.get(), m_engine, targetMock);
    builder.addBlock("sensing_touchingobject");
    builder.addDropdownInput("TOUCHINGOBJECTMENU", "test");
    Block *block = builder.currentBlock();

    EXPECT_CALL(m_engineMock, findTarget("test")).WillOnce(Return(-1));
    EXPECT_CALL(m_engineMock, targetAt).WillRepeatedly(Return(nullptr));
    Compiler compiler(&m_engineMock, targetMock.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(targetMock.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(targetMock.get(), &m_engineMock, &script);

    ValueData value = thread.runReporter();
    ASSERT_FALSE(value_toBool(&value));
    value_free(&value);
}

TEST_F(SensingBlocksTest, TouchingObject_Invalid_Runtime)
{
    auto targetMock = std::make_shared<TargetMock>();
    targetMock->setEngine(&m_engineMock);

    Sprite sprite;

    ScriptBuilder builder(m_extension.get(), m_engine, targetMock);

    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "test");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("sensing_touchingobject");
    builder.addObscuredInput("TOUCHINGOBJECTMENU", valueBlock);
    Block *block = builder.currentBlock();

    Compiler compiler(&m_engineMock, targetMock.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(targetMock.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(targetMock.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, findTarget("test")).WillRepeatedly(Return(-1));
    EXPECT_CALL(m_engineMock, targetAt).WillRepeatedly(Return(nullptr));
    ValueData value = thread.runReporter();
    ASSERT_FALSE(value_toBool(&value));
    value_free(&value);
}
