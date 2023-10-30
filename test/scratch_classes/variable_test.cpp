#include <scratchcpp/variable.h>
#include <scratchcpp/target.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(VariableTest, Constructors)
{
    Variable var1("abc", "var1");
    ASSERT_EQ(var1.id(), "abc");
    ASSERT_EQ(var1.name(), "var1");
    ASSERT_EQ(var1.value(), Value());
    ASSERT_FALSE(var1.isCloudVariable());

    Variable var2("abc", "var2", "test");
    ASSERT_EQ(var2.id(), "abc");
    ASSERT_EQ(var2.name(), "var2");
    ASSERT_EQ(var2.value().toString(), "test");
    ASSERT_FALSE(var2.isCloudVariable());

    Variable var3("abc", "var3", "test", true);
    ASSERT_EQ(var3.id(), "abc");
    ASSERT_EQ(var3.name(), "var3");
    ASSERT_EQ(var3.value().toString(), "test");
    ASSERT_TRUE(var3.isCloudVariable());

    Variable var4("abc", "var4", "test", false);
    ASSERT_EQ(var4.id(), "abc");
    ASSERT_EQ(var4.name(), "var4");
    ASSERT_EQ(var4.value().toString(), "test");
    ASSERT_FALSE(var4.isCloudVariable());
}

TEST(VariableTest, Value)
{
    Variable var("", "");

    var.setValue("hello");
    ASSERT_EQ(var.value().toString(), "hello");
}

TEST(VariableTest, ValuePtr)
{
    Variable var("", "");
    ASSERT_TRUE(var.valuePtr());
    ASSERT_EQ(*var.valuePtr(), Value());

    var.setValue("Hello, world!");
    ASSERT_EQ(var.valuePtr()->toString(), "Hello, world!");
}

TEST(VariableTest, IsCloudVariable)
{
    Variable var("", "");

    var.setIsCloudVariable(true);
    ASSERT_TRUE(var.isCloudVariable());
}

TEST(VariableTest, Target)
{
    Variable var("", "");
    ASSERT_EQ(var.target(), nullptr);

    Target target;
    var.setTarget(&target);
    ASSERT_EQ(var.target(), &target);
}

TEST(VariableTest, Clone)
{
    std::shared_ptr<Variable> clone;
    std::vector<std::shared_ptr<Variable>> vars;

    vars.push_back(std::make_shared<Variable>("abc", "var1"));
    vars.push_back(std::make_shared<Variable>("abc", "var2", "test"));
    vars.push_back(std::make_shared<Variable>("abc", "var3", "test", true));
    vars.push_back(std::make_shared<Variable>("abc", "var4", "test", false));

    for (auto var : vars) {
        clone = var->clone();
        ASSERT_TRUE(clone);
        ASSERT_EQ(clone->id(), var->id());
        ASSERT_EQ(clone->name(), var->name());
        ASSERT_EQ(clone->value(), var->value());
        ASSERT_EQ(clone->isCloudVariable(), var->isCloudVariable());
    }
}
