#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/list.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/test/scriptbuilder.h>
#include <enginemock.h>

#include "../common.h"
#include "dev/blocks/operatorblocks.h"

using namespace libscratchcpp;
using namespace libscratchcpp::test;

class OperatorBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<OperatorBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
        }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
};

TEST_F(OperatorBlocksTest, Add)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_add");
    builder.addValueInput("NUM1", 5.7);
    builder.addValueInput("NUM2", 2.5);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 1);
    ASSERT_EQ(Value(values[0]), 8.2);
}

TEST_F(OperatorBlocksTest, Subtract)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_subtract");
    builder.addValueInput("NUM1", 5.7);
    builder.addValueInput("NUM2", 2.5);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 1);
    ASSERT_EQ(Value(values[0]), 3.2);
}

TEST_F(OperatorBlocksTest, Multiply)
{
    auto target = std::make_shared<Sprite>();
    ScriptBuilder builder(m_extension.get(), m_engine, target);

    builder.addBlock("operator_multiply");
    builder.addValueInput("NUM1", 5.7);
    builder.addValueInput("NUM2", 2.5);
    builder.captureBlockReturnValue();

    builder.build();
    builder.run();

    List *valueList = builder.capturedValues();
    ValueData *values = valueList->data();
    ASSERT_EQ(valueList->size(), 1);
    ASSERT_EQ(Value(values[0]), 14.25);
}
