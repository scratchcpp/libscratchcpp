#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/monitor.h>
#include <scratchcpp/block.h>
#include <scratchcpp/field.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <enginemock.h>

#include "../common.h"
#include "blocks/variableblocks.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

class VariableBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<VariableBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
        }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
};

TEST_F(VariableBlocksTest, Variable)
{
    auto target = std::make_shared<Sprite>();
    auto var1 = std::make_shared<Variable>("", "", 835.21);
    target->addVariable(var1);
    auto var2 = std::make_shared<Variable>("", "", "Hello world");
    target->addVariable(var2);
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("data_variable");
    builder.addEntityField("VARIABLE", var1);
    builder.captureBlockReturnValue();

    builder.addBlock("data_variable");
    builder.addEntityField("VARIABLE", var2);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 2);
    ASSERT_EQ(Value(values[0]), 835.21);
    ASSERT_EQ(Value(values[1]), "Hello world");
}

TEST_F(VariableBlocksTest, VariableMonitor)
{
    auto target = std::make_shared<Sprite>();
    auto var1 = std::make_shared<Variable>("", "var1", 835.21);
    target->addVariable(var1);
    auto var2 = std::make_shared<Variable>("", "var2", "Hello world");
    target->addVariable(var2);

    auto monitor1 = std::make_shared<Monitor>("monitor", "data_variable");
    auto monitor2 = std::make_shared<Monitor>("monitor", "data_variable");
    monitor1->block()->setTarget(target.get());
    monitor2->block()->setTarget(target.get());
    m_engine->setMonitors({ monitor1, monitor2 });

    ScriptBuilder builder1(m_extension.get(), m_engine, target);
    builder1.addBlock(monitor1->block());
    builder1.addEntityField("VARIABLE", var1);

    ScriptBuilder builder2(m_extension.get(), m_engine, target);
    builder2.addBlock(monitor2->block());
    builder2.addEntityField("VARIABLE", var2);

    m_engine->compile();
    ASSERT_EQ(monitor1->name(), var1->name());
    ASSERT_EQ(monitor2->name(), var2->name());

    monitor1->changeValue("test");
    ASSERT_EQ(var1->value().toString(), "test");
    ASSERT_EQ(var2->value().toString(), "Hello world");

    monitor2->changeValue(-0.25);
    ASSERT_EQ(var1->value().toString(), "test");
    ASSERT_EQ(var2->value().toDouble(), -0.25);
}

TEST_F(VariableBlocksTest, SetVariableTo)
{
    auto target = std::make_shared<Sprite>();
    auto var1 = std::make_shared<Variable>("", "", 835.21);
    target->addVariable(var1);
    auto var2 = std::make_shared<Variable>("", "", "Hello world");
    target->addVariable(var2);
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("data_setvariableto");
    builder.addEntityField("VARIABLE", var1);
    builder.addValueInput("VALUE", "test");

    builder.addBlock("data_setvariableto");
    builder.addEntityField("VARIABLE", var2);
    builder.addValueInput("VALUE", 123);

    builder.build();
    builder.run();
    ASSERT_EQ(var1->value(), "test");
    ASSERT_EQ(var2->value(), 123);
}

TEST_F(VariableBlocksTest, ChangeVariableBy)
{
    auto target = std::make_shared<Sprite>();
    auto var1 = std::make_shared<Variable>("", "", 835.21);
    target->addVariable(var1);
    auto var2 = std::make_shared<Variable>("", "", "Hello world");
    target->addVariable(var2);
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("data_changevariableby");
    builder.addEntityField("VARIABLE", var1);
    builder.addValueInput("VALUE", "5.12");

    builder.addBlock("data_changevariableby");
    builder.addEntityField("VARIABLE", var2);
    builder.addValueInput("VALUE", -2.5);

    builder.build();

    builder.run();
    ASSERT_EQ(var1->value(), 840.33);
    ASSERT_EQ(var2->value(), -2.5);

    builder.run();
    ASSERT_EQ(var1->value(), 845.45);
    ASSERT_EQ(var2->value(), -5);

    builder.run();
    ASSERT_EQ(var1->value(), 850.57);
    ASSERT_EQ(var2->value(), -7.5);
}

TEST_F(VariableBlocksTest, ShowVariable_Global_Existent)
{
    auto stage = std::make_shared<Stage>();
    auto var1 = std::make_shared<Variable>("a", "var1", 835.21);
    stage->addVariable(var1);
    auto var2 = std::make_shared<Variable>("b", "var2", "Hello world");
    stage->addVariable(var2);

    auto monitor1 = std::make_shared<Monitor>("monitor", "data_variable");
    monitor1->block()->addField(std::make_shared<Field>("VARIABLE", var1->name(), var1));
    monitor1->setVisible(true);

    auto monitor2 = std::make_shared<Monitor>("monitor", "data_variable");
    monitor2->block()->addField(std::make_shared<Field>("VARIABLE", var2->name(), var2));
    monitor2->setVisible(false);

    m_engine->setMonitors({ monitor1, monitor2 });
    var1->setMonitor(monitor1.get());
    var2->setMonitor(monitor2.get());

    m_engine->setTargets({ stage });

    ScriptBuilder builder1(m_extension.get(), m_engine, stage);
    builder1.addBlock("data_showvariable");
    builder1.addEntityField("VARIABLE", var1);

    ScriptBuilder builder2(m_extension.get(), m_engine, stage);
    builder2.addBlock("data_showvariable");
    builder2.addEntityField("VARIABLE", var2);

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });

    builder1.run();
    ASSERT_TRUE(monitor1->visible());
    ASSERT_FALSE(monitor2->visible());

    builder2.run();
    ASSERT_TRUE(monitor1->visible());
    ASSERT_TRUE(monitor2->visible());
}

TEST_F(VariableBlocksTest, ShowVariable_Global_Nonexistent)
{
    auto stage = std::make_shared<Stage>();
    auto var1 = std::make_shared<Variable>("a", "var1", 835.21);
    stage->addVariable(var1);
    auto var2 = std::make_shared<Variable>("b", "var2", "Hello world");
    stage->addVariable(var2);

    m_engine->setTargets({ stage });

    ScriptBuilder builder1(m_extension.get(), m_engine, stage);
    builder1.addBlock("data_showvariable");
    builder1.addEntityField("VARIABLE", var1);

    ScriptBuilder builder2(m_extension.get(), m_engine, stage);
    builder2.addBlock("data_showvariable");
    builder2.addEntityField("VARIABLE", var2);

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });

    // Missing monitors should be created
    builder1.run();

    Monitor *monitor1 = var1->monitor();
    ASSERT_TRUE(monitor1);
    ASSERT_TRUE(monitor1->visible());

    builder2.run();

    Monitor *monitor2 = var2->monitor();

    ASSERT_TRUE(monitor2);
    ASSERT_TRUE(monitor1->visible());
    ASSERT_TRUE(monitor2->visible());
}

TEST_F(VariableBlocksTest, ShowVariable_Local_Existent)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();

    auto var1 = std::make_shared<Variable>("a", "var1", 835.21);
    sprite->addVariable(var1);
    auto var2 = std::make_shared<Variable>("b", "var2", "Hello world");
    sprite->addVariable(var2);

    auto monitor1 = std::make_shared<Monitor>("monitor", "data_variable");
    monitor1->block()->addField(std::make_shared<Field>("VARIABLE", var1->name(), var1));
    monitor1->setVisible(true);

    auto monitor2 = std::make_shared<Monitor>("monitor", "data_variable");
    monitor2->block()->addField(std::make_shared<Field>("VARIABLE", var2->name(), var2));
    monitor2->setVisible(false);

    m_engine->setMonitors({ monitor1, monitor2 });
    var1->setMonitor(monitor1.get());
    var2->setMonitor(monitor2.get());

    m_engine->setTargets({ stage, sprite });

    ScriptBuilder builder1(m_extension.get(), m_engine, sprite);
    builder1.addBlock("data_showvariable");
    builder1.addEntityField("VARIABLE", var1);

    ScriptBuilder builder2(m_extension.get(), m_engine, sprite);
    builder2.addBlock("data_showvariable");
    builder2.addEntityField("VARIABLE", var2);

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });

    builder1.run();
    ASSERT_TRUE(monitor1->visible());
    ASSERT_FALSE(monitor2->visible());

    builder2.run();
    ASSERT_TRUE(monitor1->visible());
    ASSERT_TRUE(monitor2->visible());
}

TEST_F(VariableBlocksTest, ShowVariable_Local_Nonexistent)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();

    auto var1 = std::make_shared<Variable>("a", "var1", 835.21);
    sprite->addVariable(var1);
    auto var2 = std::make_shared<Variable>("b", "var2", "Hello world");
    sprite->addVariable(var2);

    m_engine->setTargets({ stage, sprite });

    ScriptBuilder builder1(m_extension.get(), m_engine, sprite);
    builder1.addBlock("data_showvariable");
    builder1.addEntityField("VARIABLE", var1);

    ScriptBuilder builder2(m_extension.get(), m_engine, sprite);
    builder2.addBlock("data_showvariable");
    builder2.addEntityField("VARIABLE", var2);

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });
    m_engine->run();

    // Missing monitors should be created
    builder1.run();

    Monitor *monitor1 = var1->monitor();
    ASSERT_TRUE(monitor1);
    ASSERT_TRUE(monitor1->visible());

    builder2.run();

    Monitor *monitor2 = var2->monitor();

    ASSERT_TRUE(monitor2);
    ASSERT_TRUE(monitor1->visible());
    ASSERT_TRUE(monitor2->visible());
}

TEST_F(VariableBlocksTest, ShowVariable_Local_FromClone)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();

    auto var1 = std::make_shared<Variable>("a", "var1", 835.21);
    sprite->addVariable(var1);
    auto var2 = std::make_shared<Variable>("b", "var2", "Hello world");
    sprite->addVariable(var2);

    auto monitor1 = std::make_shared<Monitor>("monitor", "data_variable");
    monitor1->block()->addField(std::make_shared<Field>("VARIABLE", var1->name(), var1));
    monitor1->setVisible(true);

    auto monitor2 = std::make_shared<Monitor>("monitor", "data_variable");
    monitor2->block()->addField(std::make_shared<Field>("VARIABLE", var2->name(), var2));
    monitor2->setVisible(false);

    m_engine->setMonitors({ monitor1, monitor2 });
    var1->setMonitor(monitor1.get());
    var2->setMonitor(monitor2.get());

    sprite->setEngine(m_engine);
    auto clone = sprite->clone();

    m_engine->setTargets({ stage, sprite, clone });

    ScriptBuilder builder1(m_extension.get(), m_engine, clone);
    builder1.addBlock("data_showvariable");
    builder1.addEntityField("VARIABLE", var1);
    Block *hat1 = builder1.currentBlock()->parent();

    ScriptBuilder builder2(m_extension.get(), m_engine, clone);
    builder2.addBlock("data_showvariable");
    builder2.addEntityField("VARIABLE", var2);
    Block *hat2 = builder2.currentBlock()->parent();

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });

    // The clone root variables should be used
    builder1.run();
    ASSERT_TRUE(monitor1->visible());
    ASSERT_FALSE(monitor2->visible());

    builder2.run();
    ASSERT_TRUE(monitor1->visible());
    ASSERT_TRUE(monitor2->visible());

    ASSERT_FALSE(clone->variableAt(0)->monitor());
    ASSERT_FALSE(clone->variableAt(1)->monitor());
}

TEST_F(VariableBlocksTest, HideVariable_Global_Existent)
{
    auto stage = std::make_shared<Stage>();
    auto var1 = std::make_shared<Variable>("a", "var1", 835.21);
    stage->addVariable(var1);
    auto var2 = std::make_shared<Variable>("b", "var2", "Hello world");
    stage->addVariable(var2);

    auto monitor1 = std::make_shared<Monitor>("monitor", "data_variable");
    monitor1->block()->addField(std::make_shared<Field>("VARIABLE", var1->name(), var1));
    monitor1->setVisible(true);

    auto monitor2 = std::make_shared<Monitor>("monitor", "data_variable");
    monitor2->block()->addField(std::make_shared<Field>("VARIABLE", var2->name(), var2));
    monitor2->setVisible(false);

    m_engine->setMonitors({ monitor1, monitor2 });
    var1->setMonitor(monitor1.get());
    var2->setMonitor(monitor2.get());

    m_engine->setTargets({ stage });

    ScriptBuilder builder1(m_extension.get(), m_engine, stage);
    builder1.addBlock("data_hidevariable");
    builder1.addEntityField("VARIABLE", var1);

    ScriptBuilder builder2(m_extension.get(), m_engine, stage);
    builder2.addBlock("data_hidevariable");
    builder2.addEntityField("VARIABLE", var2);

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });

    builder1.run();
    ASSERT_FALSE(monitor1->visible());
    ASSERT_FALSE(monitor2->visible());

    builder2.run();
    ASSERT_FALSE(monitor1->visible());
    ASSERT_FALSE(monitor2->visible());
}

TEST_F(VariableBlocksTest, HideVariable_Global_Nonexistent)
{
    auto stage = std::make_shared<Stage>();
    auto var1 = std::make_shared<Variable>("a", "var1", 835.21);
    stage->addVariable(var1);
    auto var2 = std::make_shared<Variable>("b", "var2", "Hello world");
    stage->addVariable(var2);

    m_engine->setTargets({ stage });

    ScriptBuilder builder1(m_extension.get(), m_engine, stage);
    builder1.addBlock("data_hidevariable");
    builder1.addEntityField("VARIABLE", var1);

    ScriptBuilder builder2(m_extension.get(), m_engine, stage);
    builder2.addBlock("data_hidevariable");
    builder2.addEntityField("VARIABLE", var2);

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });

    // Missing monitors should NOT be created
    builder1.run();
    ASSERT_FALSE(var1->monitor());

    builder2.run();
    ASSERT_FALSE(var2->monitor());
}

TEST_F(VariableBlocksTest, HideVariable_Local_Existent)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();

    auto var1 = std::make_shared<Variable>("a", "var1", 835.21);
    sprite->addVariable(var1);
    auto var2 = std::make_shared<Variable>("b", "var2", "Hello world");
    sprite->addVariable(var2);

    auto monitor1 = std::make_shared<Monitor>("monitor", "data_variable");
    monitor1->block()->addField(std::make_shared<Field>("VARIABLE", var1->name(), var1));
    monitor1->setVisible(true);

    auto monitor2 = std::make_shared<Monitor>("monitor", "data_variable");
    monitor2->block()->addField(std::make_shared<Field>("VARIABLE", var2->name(), var2));
    monitor2->setVisible(false);

    m_engine->setMonitors({ monitor1, monitor2 });
    var1->setMonitor(monitor1.get());
    var2->setMonitor(monitor2.get());

    m_engine->setTargets({ stage, sprite });

    ScriptBuilder builder1(m_extension.get(), m_engine, sprite);
    builder1.addBlock("data_hidevariable");
    builder1.addEntityField("VARIABLE", var1);

    ScriptBuilder builder2(m_extension.get(), m_engine, sprite);
    builder2.addBlock("data_hidevariable");
    builder2.addEntityField("VARIABLE", var2);

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });

    builder1.run();
    ASSERT_FALSE(monitor1->visible());
    ASSERT_FALSE(monitor2->visible());

    builder2.run();
    ASSERT_FALSE(monitor1->visible());
    ASSERT_FALSE(monitor2->visible());
}

TEST_F(VariableBlocksTest, HideVariable_Local_Nonexistent)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();

    auto var1 = std::make_shared<Variable>("a", "var1", 835.21);
    sprite->addVariable(var1);
    auto var2 = std::make_shared<Variable>("b", "var2", "Hello world");
    sprite->addVariable(var2);

    m_engine->setTargets({ stage, sprite });

    ScriptBuilder builder1(m_extension.get(), m_engine, sprite);
    builder1.addBlock("data_hidevariable");
    builder1.addEntityField("VARIABLE", var1);

    ScriptBuilder builder2(m_extension.get(), m_engine, sprite);
    builder2.addBlock("data_hidevariable");
    builder2.addEntityField("VARIABLE", var2);

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });
    m_engine->run();

    // Missing monitors should NOT be created
    builder1.run();
    ASSERT_FALSE(var1->monitor());

    builder2.run();
    ASSERT_FALSE(var2->monitor());
}

TEST_F(VariableBlocksTest, HideVariable_Local_FromClone)
{
    auto stage = std::make_shared<Stage>();
    auto sprite = std::make_shared<Sprite>();

    auto var1 = std::make_shared<Variable>("a", "var1", 835.21);
    sprite->addVariable(var1);
    auto var2 = std::make_shared<Variable>("b", "var2", "Hello world");
    sprite->addVariable(var2);

    auto monitor1 = std::make_shared<Monitor>("monitor", "data_variable");
    monitor1->block()->addField(std::make_shared<Field>("VARIABLE", var1->name(), var1));
    monitor1->setVisible(true);

    auto monitor2 = std::make_shared<Monitor>("monitor", "data_variable");
    monitor2->block()->addField(std::make_shared<Field>("VARIABLE", var2->name(), var2));
    monitor2->setVisible(false);

    m_engine->setMonitors({ monitor1, monitor2 });
    var1->setMonitor(monitor1.get());
    var2->setMonitor(monitor2.get());

    sprite->setEngine(m_engine);
    auto clone = sprite->clone();

    m_engine->setTargets({ stage, sprite, clone });

    ScriptBuilder builder1(m_extension.get(), m_engine, clone);
    builder1.addBlock("data_hidevariable");
    builder1.addEntityField("VARIABLE", var1);
    Block *hat1 = builder1.currentBlock()->parent();

    ScriptBuilder builder2(m_extension.get(), m_engine, clone);
    builder2.addBlock("data_hidevariable");
    builder2.addEntityField("VARIABLE", var2);
    Block *hat2 = builder2.currentBlock()->parent();

    ScriptBuilder::buildMultiple({ &builder1, &builder2 });

    // The clone root variables should be used
    builder1.run();
    ASSERT_FALSE(monitor1->visible());
    ASSERT_FALSE(monitor2->visible());

    builder2.run();
    ASSERT_FALSE(monitor1->visible());
    ASSERT_FALSE(monitor2->visible());

    ASSERT_FALSE(clone->variableAt(0)->monitor());
    ASSERT_FALSE(clone->variableAt(1)->monitor());
}
