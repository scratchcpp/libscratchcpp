#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/list.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/textbubble.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <enginemock.h>
#include <targetmock.h>

#include "../common.h"
#include "blocks/sensingblocks.h"
#include "util.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

using ::testing::Return;
using ::testing::ReturnRef;

class SensingBlocksTest : public testing::Test
{
    public:
        struct QuestionSpy
        {
                MOCK_METHOD(void, asked, (const std::string &), ());
                MOCK_METHOD(void, aborted, (), ());
        };

        void SetUp() override
        {
            m_extension = std::make_unique<SensingBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
            m_extension->onInit(m_engine);
            registerBlocks(m_engine, m_extension.get());
        }

        void TearDown() override { SensingBlocks::clearQuestions(); }

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

TEST_F(SensingBlocksTest, TouchingColor_String)
{
    auto targetMock = std::make_shared<TargetMock>();
    targetMock->setEngine(&m_engineMock);

    ScriptBuilder builder(m_extension.get(), m_engine, targetMock);
    builder.addBlock("sensing_touchingcolor");
    builder.addValueInput("COLOR", "#00ffff");
    Block *block = builder.currentBlock();

    Compiler compiler(&m_engineMock, targetMock.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(targetMock.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(targetMock.get(), &m_engineMock, &script);

    EXPECT_CALL(*targetMock, touchingColor(rgb(0, 255, 255))).WillOnce(Return(true));
    ValueData value = thread.runReporter();
    ASSERT_TRUE(value_toBool(&value));
    value_free(&value);

    EXPECT_CALL(*targetMock, touchingColor(rgb(0, 255, 255))).WillOnce(Return(false));
    value = thread.runReporter();
    ASSERT_FALSE(value_toBool(&value));
    value_free(&value);
}

TEST_F(SensingBlocksTest, TouchingColor_Number)
{
    auto targetMock = std::make_shared<TargetMock>();
    targetMock->setEngine(&m_engineMock);

    ScriptBuilder builder(m_extension.get(), m_engine, targetMock);
    builder.addBlock("sensing_touchingcolor");
    builder.addValueInput("COLOR", rgb(255, 54, 23));
    Block *block = builder.currentBlock();

    Compiler compiler(&m_engineMock, targetMock.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(targetMock.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(targetMock.get(), &m_engineMock, &script);

    EXPECT_CALL(*targetMock, touchingColor(rgb(255, 54, 23))).WillOnce(Return(true));
    ValueData value = thread.runReporter();
    ASSERT_TRUE(value_toBool(&value));
    value_free(&value);

    EXPECT_CALL(*targetMock, touchingColor(rgb(255, 54, 23))).WillOnce(Return(false));
    value = thread.runReporter();
    ASSERT_FALSE(value_toBool(&value));
    value_free(&value);
}

TEST_F(SensingBlocksTest, ColorIsTouchingColor_StringNumber)
{
    auto targetMock = std::make_shared<TargetMock>();
    targetMock->setEngine(&m_engineMock);

    ScriptBuilder builder(m_extension.get(), m_engine, targetMock);
    builder.addBlock("sensing_coloristouchingcolor");
    builder.addValueInput("COLOR", "#00ffff");
    builder.addValueInput("COLOR2", rgb(255, 54, 23));
    Block *block = builder.currentBlock();

    Compiler compiler(&m_engineMock, targetMock.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(targetMock.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(targetMock.get(), &m_engineMock, &script);

    EXPECT_CALL(*targetMock, touchingColor(rgb(0, 255, 255), rgb(255, 54, 23))).WillOnce(Return(true));
    ValueData value = thread.runReporter();
    ASSERT_TRUE(value_toBool(&value));
    value_free(&value);

    EXPECT_CALL(*targetMock, touchingColor(rgb(0, 255, 255), rgb(255, 54, 23))).WillOnce(Return(false));
    value = thread.runReporter();
    ASSERT_FALSE(value_toBool(&value));
    value_free(&value);
}

TEST_F(SensingBlocksTest, ColorIsTouchingColor_NumberString)
{
    auto targetMock = std::make_shared<TargetMock>();
    targetMock->setEngine(&m_engineMock);

    ScriptBuilder builder(m_extension.get(), m_engine, targetMock);
    builder.addBlock("sensing_coloristouchingcolor");
    builder.addValueInput("COLOR", rgb(255, 54, 23));
    builder.addValueInput("COLOR2", "#00ffff");
    Block *block = builder.currentBlock();

    Compiler compiler(&m_engineMock, targetMock.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(targetMock.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(targetMock.get(), &m_engineMock, &script);

    EXPECT_CALL(*targetMock, touchingColor(rgb(255, 54, 23), rgb(0, 255, 255))).WillOnce(Return(true));
    ValueData value = thread.runReporter();
    ASSERT_TRUE(value_toBool(&value));
    value_free(&value);

    EXPECT_CALL(*targetMock, touchingColor(rgb(255, 54, 23), rgb(0, 255, 255))).WillOnce(Return(false));
    value = thread.runReporter();
    ASSERT_FALSE(value_toBool(&value));
    value_free(&value);
}

TEST_F(SensingBlocksTest, DistanceTo_Sprite_CompileTime)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    Sprite targetSprite;

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sensing_distanceto");
    builder.addDropdownInput("DISTANCETOMENU", "Sprite2");
    Block *block = builder.currentBlock();

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillOnce(Return(3));
    EXPECT_CALL(m_engineMock, targetAt(3)).WillOnce(Return(&targetSprite));
    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    sprite->setX(-50.35);
    sprite->setY(33.04);

    targetSprite.setX(108.564);
    targetSprite.setY(-168.452);

    ValueData value = thread.runReporter();
    ASSERT_EQ(std::round(value_toDouble(&value) * 10000) / 10000, 256.6178);
    value_free(&value);
}

TEST_F(SensingBlocksTest, DistanceTo_Sprite_Runtime)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    Sprite targetSprite;

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "Sprite2");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("sensing_distanceto");
    builder.addObscuredInput("DISTANCETOMENU", valueBlock);
    Block *block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    sprite->setX(-50.35);
    sprite->setY(33.04);

    targetSprite.setX(108.564);
    targetSprite.setY(-168.452);

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillRepeatedly(Return(5));
    EXPECT_CALL(m_engineMock, targetAt(5)).WillRepeatedly(Return(&targetSprite));

    ValueData value = thread.runReporter();
    ASSERT_EQ(std::round(value_toDouble(&value) * 10000) / 10000, 256.6178);
    value_free(&value);
}

TEST_F(SensingBlocksTest, DistanceTo_Sprite_FromStage)
{
    auto stage = std::make_shared<Stage>();
    stage->setEngine(&m_engineMock);

    Sprite targetSprite;

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("sensing_distanceto");
    builder.addDropdownInput("DISTANCETOMENU", "Sprite2");
    Block *block = builder.currentBlock();

    EXPECT_CALL(m_engineMock, findTarget("Sprite2")).WillRepeatedly(Return(3));
    EXPECT_CALL(m_engineMock, targetAt(3)).WillRepeatedly(Return(&targetSprite));
    Compiler compiler(&m_engineMock, stage.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(stage.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(stage.get(), &m_engineMock, &script);

    targetSprite.setX(108.564);
    targetSprite.setY(-168.452);

    ValueData value = thread.runReporter();
    ASSERT_EQ(value_toDouble(&value), 10000.0);
    value_free(&value);
}

TEST_F(SensingBlocksTest, DistanceTo_Stage_CompileTime)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    Stage stage;

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sensing_distanceto");
    builder.addDropdownInput("DISTANCETOMENU", "_stage_");
    Block *block = builder.currentBlock();

    EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillRepeatedly(Return(3));
    EXPECT_CALL(m_engineMock, targetAt(3)).WillRepeatedly(Return(&stage));
    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    ValueData value = thread.runReporter();
    ASSERT_EQ(value_toDouble(&value), 10000.0);
    value_free(&value);
}

TEST_F(SensingBlocksTest, DistanceTo_Stage_Runtime)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    Stage stage;

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "_stage_");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("sensing_distanceto");
    builder.addObscuredInput("DISTANCETOMENU", valueBlock);
    Block *block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, findTarget("_stage_")).WillRepeatedly(Return(5));
    EXPECT_CALL(m_engineMock, targetAt(5)).WillRepeatedly(Return(&stage));

    ValueData value = thread.runReporter();
    ASSERT_EQ(value_toDouble(&value), 10000.0);
    value_free(&value);
}

TEST_F(SensingBlocksTest, DistanceTo_Mouse_CompileTime)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sensing_distanceto");
    builder.addDropdownInput("DISTANCETOMENU", "_mouse_");
    Block *block = builder.currentBlock();

    EXPECT_CALL(m_engineMock, findTarget).Times(0);
    EXPECT_CALL(m_engineMock, targetAt).Times(0);
    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    sprite->setX(-50.35);
    sprite->setY(33.04);

    EXPECT_CALL(m_engineMock, mouseX).WillOnce(Return(-239.98));
    EXPECT_CALL(m_engineMock, mouseY).WillOnce(Return(-86.188));
    ValueData value = thread.runReporter();
    ASSERT_EQ(std::round(value_toDouble(&value) * 10000) / 10000, 223.9974);
    value_free(&value);
}

TEST_F(SensingBlocksTest, DistanceTo_Mouse_Runtime)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "_mouse_");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("sensing_distanceto");
    builder.addObscuredInput("DISTANCETOMENU", valueBlock);
    Block *block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, findTarget).Times(0);
    EXPECT_CALL(m_engineMock, targetAt).Times(0);

    sprite->setX(-50.35);
    sprite->setY(33.04);

    EXPECT_CALL(m_engineMock, mouseX).WillOnce(Return(-239.98));
    EXPECT_CALL(m_engineMock, mouseY).WillOnce(Return(-86.188));
    ValueData value = thread.runReporter();
    ASSERT_EQ(std::round(value_toDouble(&value) * 10000) / 10000, 223.9974);
    value_free(&value);
}

TEST_F(SensingBlocksTest, DistanceTo_Invalid_CompileTime)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sensing_distanceto");
    builder.addDropdownInput("DISTANCETOMENU", "test");
    Block *block = builder.currentBlock();

    EXPECT_CALL(m_engineMock, findTarget("test")).WillOnce(Return(-1));
    EXPECT_CALL(m_engineMock, targetAt).WillRepeatedly(Return(nullptr));
    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    ValueData value = thread.runReporter();
    ASSERT_EQ(value_toDouble(&value), 10000.0);
    value_free(&value);
}

TEST_F(SensingBlocksTest, DistanceTo_Invalid_Runtime)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);

    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "test");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("sensing_distanceto");
    builder.addObscuredInput("DISTANCETOMENU", valueBlock);
    Block *block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block, Compiler::CodeType::Reporter);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread(sprite.get(), &m_engineMock, &script);

    EXPECT_CALL(m_engineMock, findTarget("test")).WillRepeatedly(Return(-1));
    EXPECT_CALL(m_engineMock, targetAt).WillRepeatedly(Return(nullptr));
    ValueData value = thread.runReporter();
    ASSERT_EQ(value_toDouble(&value), 10000.0);
    value_free(&value);
}

TEST_F(SensingBlocksTest, AskAndWait_VisibleSprite)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    // Build
    ScriptBuilder builder1(m_extension.get(), m_engine, sprite);
    builder1.addBlock("sensing_askandwait");
    builder1.addValueInput("QUESTION", "test");
    Block *block1 = builder1.currentBlock();

    Compiler compiler1(&m_engineMock, sprite.get());
    auto code1 = compiler1.compile(block1);
    Script script1(sprite.get(), block1, &m_engineMock);
    script1.setCode(code1);
    Thread thread1(sprite.get(), &m_engineMock, &script1);

    ScriptBuilder builder2(m_extension.get(), m_engine, sprite);
    builder2.addBlock("sensing_answer");
    Block *block2 = builder2.currentBlock();

    Compiler compiler2(m_engine, sprite.get());
    auto code2 = compiler2.compile(block2, Compiler::CodeType::Reporter);
    Script script2(sprite.get(), block2, m_engine);
    script2.setCode(code2);
    Thread thread2(sprite.get(), m_engine, &script2);

    // Run
    QuestionSpy spy;
    auto asked = std::bind(&QuestionSpy::asked, &spy, std::placeholders::_1);
    sigslot::signal<const std::string &> askedSignal;
    askedSignal.connect(asked);

    sprite->bubble()->setType(TextBubble::Type::Think);
    sprite->setVisible(true);

    EXPECT_CALL(m_engineMock, questionAsked()).WillOnce(ReturnRef(askedSignal));
    EXPECT_CALL(spy, asked("")); // visible => empty question
    thread1.run();
    ASSERT_EQ(sprite->bubble()->type(), TextBubble::Type::Say);
    ASSERT_EQ(sprite->bubble()->text(), "test");

    ValueData value = thread2.runReporter();
    std::string str;
    value_toString(&value, &str);
    value_free(&value);
    ASSERT_EQ(str, ""); // not answered yet

    ASSERT_FALSE(thread1.isFinished());

    // Answer
    m_engine->questionAnswered()("test answer");
    ASSERT_TRUE(sprite->bubble()->text().empty());

    value = thread2.runReporter();
    value_toString(&value, &str);
    value_free(&value);
    ASSERT_EQ(str, "test answer");

    thread1.run();
    ASSERT_TRUE(thread1.isFinished());
}

TEST_F(SensingBlocksTest, AskAndWait_VisibleSpriteBefore)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    // Build
    ScriptBuilder builder1(m_extension.get(), m_engine, sprite);
    builder1.addBlock("sensing_askandwait");
    builder1.addValueInput("QUESTION", "test");
    Block *block1 = builder1.currentBlock();

    Compiler compiler1(&m_engineMock, sprite.get());
    auto code1 = compiler1.compile(block1);
    Script script1(sprite.get(), block1, &m_engineMock);
    script1.setCode(code1);
    Thread thread1(sprite.get(), &m_engineMock, &script1);

    ScriptBuilder builder2(m_extension.get(), m_engine, sprite);
    builder2.addBlock("sensing_answer");
    Block *block2 = builder2.currentBlock();

    Compiler compiler2(m_engine, sprite.get());
    auto code2 = compiler2.compile(block2, Compiler::CodeType::Reporter);
    Script script2(sprite.get(), block2, m_engine);
    script2.setCode(code2);
    Thread thread2(sprite.get(), m_engine, &script2);

    // Run
    QuestionSpy spy;
    auto asked = std::bind(&QuestionSpy::asked, &spy, std::placeholders::_1);
    sigslot::signal<const std::string &> askedSignal;
    askedSignal.connect(asked);

    sprite->bubble()->setType(TextBubble::Type::Think);
    sprite->setVisible(true);

    EXPECT_CALL(m_engineMock, questionAsked()).WillOnce(ReturnRef(askedSignal));
    EXPECT_CALL(spy, asked("")); // visible => empty question
    thread1.run();
    ASSERT_EQ(sprite->bubble()->type(), TextBubble::Type::Say);
    ASSERT_EQ(sprite->bubble()->text(), "test");

    ASSERT_FALSE(thread1.isFinished());

    // Answer
    sprite->setVisible(false);
    m_engine->questionAnswered()("test answer");
    ASSERT_TRUE(sprite->bubble()->text().empty()); // sprite was visible when asked

    ValueData value = thread2.runReporter();
    std::string str;
    value_toString(&value, &str);
    value_free(&value);
    ASSERT_EQ(str, "test answer");

    thread1.run();
    ASSERT_TRUE(thread1.isFinished());
}

TEST_F(SensingBlocksTest, AskAndWait_InvisibleSprite)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    // Build
    ScriptBuilder builder1(m_extension.get(), m_engine, sprite);
    builder1.addBlock("sensing_askandwait");
    builder1.addValueInput("QUESTION", "test");
    Block *block1 = builder1.currentBlock();

    Compiler compiler1(&m_engineMock, sprite.get());
    auto code1 = compiler1.compile(block1);
    Script script1(sprite.get(), block1, &m_engineMock);
    script1.setCode(code1);
    Thread thread1(sprite.get(), &m_engineMock, &script1);

    ScriptBuilder builder2(m_extension.get(), m_engine, sprite);
    builder2.addBlock("sensing_answer");
    Block *block2 = builder2.currentBlock();

    Compiler compiler2(m_engine, sprite.get());
    auto code2 = compiler2.compile(block2, Compiler::CodeType::Reporter);
    Script script2(sprite.get(), block2, m_engine);
    script2.setCode(code2);
    Thread thread2(sprite.get(), m_engine, &script2);

    // Run
    QuestionSpy spy;
    auto asked = std::bind(&QuestionSpy::asked, &spy, std::placeholders::_1);
    sigslot::signal<const std::string &> askedSignal;
    askedSignal.connect(asked);

    sprite->bubble()->setType(TextBubble::Type::Think);
    sprite->bubble()->setText("hello");
    sprite->setVisible(false);

    EXPECT_CALL(m_engineMock, questionAsked()).WillOnce(ReturnRef(askedSignal));
    EXPECT_CALL(spy, asked("test"));
    thread1.run();
    ASSERT_EQ(sprite->bubble()->type(), TextBubble::Type::Think);
    ASSERT_EQ(sprite->bubble()->text(), "hello");

    ASSERT_FALSE(thread1.isFinished());

    // Answer
    m_engine->questionAnswered()("test answer");
    ASSERT_EQ(sprite->bubble()->type(), TextBubble::Type::Think);
    ASSERT_EQ(sprite->bubble()->text(), "hello");

    ValueData value = thread2.runReporter();
    std::string str;
    value_toString(&value, &str);
    value_free(&value);
    ASSERT_EQ(str, "test answer");

    thread1.run();
    ASSERT_TRUE(thread1.isFinished());
}

TEST_F(SensingBlocksTest, AskAndWait_InvisibleSpriteBefore)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    // Build
    ScriptBuilder builder1(m_extension.get(), m_engine, sprite);
    builder1.addBlock("sensing_askandwait");
    builder1.addValueInput("QUESTION", "test");
    Block *block1 = builder1.currentBlock();

    Compiler compiler1(&m_engineMock, sprite.get());
    auto code1 = compiler1.compile(block1);
    Script script1(sprite.get(), block1, &m_engineMock);
    script1.setCode(code1);
    Thread thread1(sprite.get(), &m_engineMock, &script1);

    ScriptBuilder builder2(m_extension.get(), m_engine, sprite);
    builder2.addBlock("sensing_answer");
    Block *block2 = builder2.currentBlock();

    Compiler compiler2(m_engine, sprite.get());
    auto code2 = compiler2.compile(block2, Compiler::CodeType::Reporter);
    Script script2(sprite.get(), block2, m_engine);
    script2.setCode(code2);
    Thread thread2(sprite.get(), m_engine, &script2);

    // Run
    QuestionSpy spy;
    auto asked = std::bind(&QuestionSpy::asked, &spy, std::placeholders::_1);
    sigslot::signal<const std::string &> askedSignal;
    askedSignal.connect(asked);

    sprite->bubble()->setType(TextBubble::Type::Think);
    sprite->bubble()->setText("hello");
    sprite->setVisible(false);

    EXPECT_CALL(m_engineMock, questionAsked()).WillOnce(ReturnRef(askedSignal));
    EXPECT_CALL(spy, asked("test"));
    thread1.run();
    ASSERT_EQ(sprite->bubble()->type(), TextBubble::Type::Think);
    ASSERT_EQ(sprite->bubble()->text(), "hello");

    ASSERT_FALSE(thread1.isFinished());

    // Answer
    sprite->setVisible(true);
    m_engine->questionAnswered()("test answer");
    ASSERT_EQ(sprite->bubble()->type(), TextBubble::Type::Think);
    ASSERT_EQ(sprite->bubble()->text(), "hello"); // sprite was invisible when asked

    ValueData value = thread2.runReporter();
    std::string str;
    value_toString(&value, &str);
    ASSERT_EQ(str, "test answer");
    value_free(&value);

    thread1.run();
    ASSERT_TRUE(thread1.isFinished());
}

TEST_F(SensingBlocksTest, AskAndWait_Stage)
{
    auto stage = std::make_shared<Stage>();
    stage->setEngine(&m_engineMock);

    // Build
    ScriptBuilder builder1(m_extension.get(), m_engine, stage);
    builder1.addBlock("sensing_askandwait");
    builder1.addValueInput("QUESTION", "test");
    Block *block1 = builder1.currentBlock();

    Compiler compiler1(&m_engineMock, stage.get());
    auto code1 = compiler1.compile(block1);
    Script script1(stage.get(), block1, &m_engineMock);
    script1.setCode(code1);
    Thread thread1(stage.get(), &m_engineMock, &script1);

    ScriptBuilder builder2(m_extension.get(), m_engine, stage);
    builder2.addBlock("sensing_answer");
    Block *block2 = builder2.currentBlock();

    Compiler compiler2(m_engine, stage.get());
    auto code2 = compiler2.compile(block2, Compiler::CodeType::Reporter);
    Script script2(stage.get(), block2, m_engine);
    script2.setCode(code2);
    Thread thread2(stage.get(), m_engine, &script2);

    ASSERT_FALSE(thread1.isFinished());

    // Run
    QuestionSpy spy;
    auto asked = std::bind(&QuestionSpy::asked, &spy, std::placeholders::_1);
    sigslot::signal<const std::string &> askedSignal;
    askedSignal.connect(asked);

    stage->bubble()->setType(TextBubble::Type::Think);
    stage->bubble()->setText("hello");

    EXPECT_CALL(m_engineMock, questionAsked()).WillOnce(ReturnRef(askedSignal));
    EXPECT_CALL(spy, asked("test"));
    thread1.run();
    ASSERT_EQ(stage->bubble()->type(), TextBubble::Type::Think);
    ASSERT_EQ(stage->bubble()->text(), "hello");

    // Answer
    m_engine->questionAnswered()("test answer");
    ASSERT_EQ(stage->bubble()->type(), TextBubble::Type::Think);
    ASSERT_EQ(stage->bubble()->text(), "hello");

    ValueData value = thread2.runReporter();
    std::string str;
    value_toString(&value, &str);
    value_free(&value);
    ASSERT_EQ(str, "test answer");

    thread1.run();
    ASSERT_TRUE(thread1.isFinished());
}

TEST_F(SensingBlocksTest, AskAndWait_MultipleQuestions)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    // Build
    ScriptBuilder builder1(m_extension.get(), m_engine, sprite);

    builder1.addBlock("sensing_askandwait");
    builder1.addValueInput("QUESTION", "test1");
    Block *block1 = builder1.currentBlock();

    builder1.addBlock("sensing_askandwait");
    builder1.addValueInput("QUESTION", "test2");
    builder1.currentBlock(); // force build

    Compiler compiler1(&m_engineMock, sprite.get());
    auto code1 = compiler1.compile(block1);
    Script script1(sprite.get(), block1, &m_engineMock);
    script1.setCode(code1);
    Thread thread1(sprite.get(), &m_engineMock, &script1);

    ScriptBuilder builder2(m_extension.get(), m_engine, sprite);
    builder2.addBlock("sensing_answer");
    Block *block2 = builder2.currentBlock();

    Compiler compiler2(m_engine, sprite.get());
    auto code2 = compiler2.compile(block2, Compiler::CodeType::Reporter);
    Script script2(sprite.get(), block2, m_engine);
    script2.setCode(code2);
    Thread thread2(sprite.get(), m_engine, &script2);

    // Run
    QuestionSpy spy;
    auto asked = std::bind(&QuestionSpy::asked, &spy, std::placeholders::_1);
    sigslot::signal<const std::string &> askedSignal;
    askedSignal.connect(asked);

    sprite->setVisible(false);

    // Ask (1/2)
    EXPECT_CALL(m_engineMock, questionAsked()).WillOnce(ReturnRef(askedSignal));
    EXPECT_CALL(spy, asked("test1"));
    thread1.run();
    ASSERT_FALSE(thread1.isFinished());

    // Answer (1/2)
    m_engine->questionAnswered()("test answer 1");

    ValueData value = thread2.runReporter();
    std::string str;
    value_toString(&value, &str);
    value_free(&value);
    ASSERT_EQ(str, "test answer 1");

    // Ask (2/2)
    EXPECT_CALL(m_engineMock, questionAsked()).WillOnce(ReturnRef(askedSignal));
    EXPECT_CALL(spy, asked("test2"));
    thread1.run();
    ASSERT_FALSE(thread1.isFinished());

    // Answer (2/2)
    m_engine->questionAnswered()("test answer 2");

    value = thread2.runReporter();
    value_toString(&value, &str);
    value_free(&value);
    ASSERT_EQ(str, "test answer 2");

    EXPECT_CALL(m_engineMock, questionAsked).Times(0);
    thread1.run();
    ASSERT_TRUE(thread1.isFinished());
}

TEST_F(SensingBlocksTest, AskAndWait_KillThread)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    // Build
    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("sensing_askandwait");
    builder.addValueInput("QUESTION", "test");
    Block *block = builder.currentBlock();

    Compiler compiler(&m_engineMock, sprite.get());
    auto code = compiler.compile(block);
    Script script(sprite.get(), block, &m_engineMock);
    script.setCode(code);
    Thread thread1(sprite.get(), &m_engineMock, &script);

    // Run
    QuestionSpy spy;
    auto asked = std::bind(&QuestionSpy::asked, &spy, std::placeholders::_1);
    sigslot::signal<const std::string &> askedSignal;
    askedSignal.connect(asked);

    sprite->bubble()->setText("hello");
    sprite->setVisible(false);

    EXPECT_CALL(m_engineMock, questionAsked()).WillOnce(ReturnRef(askedSignal));
    EXPECT_CALL(spy, asked);
    thread1.run();

    // Kill
    auto aborted = std::bind(&QuestionSpy::aborted, &spy);
    m_engine->questionAborted().connect(aborted);

    EXPECT_CALL(spy, aborted());
    m_engine->threadAboutToStop()(&thread1);
    thread1.kill();
    ASSERT_TRUE(sprite->bubble()->text().empty());

    // Running the script again should work because the question has been removed
    EXPECT_CALL(m_engineMock, questionAsked()).WillOnce(ReturnRef(askedSignal));
    EXPECT_CALL(spy, asked);
    Thread thread2(sprite.get(), &m_engineMock, &script);
    thread2.run();
}
