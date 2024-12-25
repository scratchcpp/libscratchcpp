#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/test/scriptbuilder.h>
#include <enginemock.h>

#include "../common.h"
#include "dev/blocks/variableblocks.h"

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
