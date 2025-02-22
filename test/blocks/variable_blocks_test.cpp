#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/monitor.h>
#include <scratchcpp/block.h>
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
