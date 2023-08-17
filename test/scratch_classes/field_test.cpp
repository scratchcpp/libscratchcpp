#include <scratchcpp/field.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>

#include "../common.h"

using namespace libscratchcpp;

class FieldTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_broadcast = std::make_shared<Broadcast>("broadcast", "");
            m_variable = std::make_shared<Variable>("variable", "");
            m_list = std::make_shared<List>("list", "");
        }

        std::shared_ptr<Broadcast> m_broadcast;
        std::shared_ptr<Variable> m_variable;
        std::shared_ptr<List> m_list;
};

TEST_F(FieldTest, Constructors)
{
    Field field1("VARIABLE", "");
    ASSERT_EQ(field1.name(), "VARIABLE");
    ASSERT_EQ(field1.value().toString(), "");
    ASSERT_EQ(field1.valuePtr(), nullptr);
    ASSERT_EQ(field1.valueId(), "");

    Field field2("VARIABLE", "variable1", m_variable);
    ASSERT_EQ(field2.name(), "VARIABLE");
    ASSERT_EQ(field2.value().toString(), "variable1");
    ASSERT_EQ(field2.valuePtr(), m_variable);
    ASSERT_EQ(field2.valueId(), "variable");

    Field field3("LIST", "list1", std::string("list"));
    ASSERT_EQ(field3.name(), "LIST");
    ASSERT_EQ(field3.value().toString(), "list1");
    ASSERT_EQ(field3.valuePtr(), nullptr);
    ASSERT_EQ(field3.valueId(), "list");

    Field field4("LIST", "list1", "list");
    ASSERT_EQ(field4.name(), "LIST");
    ASSERT_EQ(field4.value().toString(), "list1");
    ASSERT_EQ(field4.valuePtr(), nullptr);
    ASSERT_EQ(field4.valueId(), "list");
}

TEST_F(FieldTest, FieldId)
{
    Field field("", Value());
    ASSERT_EQ(field.fieldId(), -1);

    field.setFieldId(2);
    ASSERT_EQ(field.fieldId(), 2);
}

TEST_F(FieldTest, ValuePtr)
{
    Field field("", Value());

    field.setValuePtr(m_broadcast);
    ASSERT_EQ(field.valuePtr(), m_broadcast);
    ASSERT_EQ(field.valueId(), "broadcast");

    field.setValuePtr(nullptr);
    ASSERT_EQ(field.valuePtr(), nullptr);
    ASSERT_EQ(field.valueId(), "");
}

TEST_F(FieldTest, SpecialValueId)
{
    Field field("", Value());
    ASSERT_EQ(field.specialValueId(), -1);

    field.setSpecialValueId(5);
    ASSERT_EQ(field.specialValueId(), 5);
}
