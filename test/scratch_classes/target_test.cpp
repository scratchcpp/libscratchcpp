#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>

#include "scratch/stage.h"
#include "../common.h"

using namespace libscratchcpp;

TEST(TargetTest, FindCostume)
{
    Stage stage;
    Costume c1("costume1", "", "svg");
    Costume c2("costume2", "", "png");
    Costume c3("costume3", "", "svg");
    stage.addCostume(c1);
    stage.addCostume(c2);
    stage.addCostume(c3);

    ASSERT_EQ(stage.findCostume("invalid"), -1);
    ASSERT_EQ(stage.findCostume("costume1"), 0);
    ASSERT_EQ(stage.findCostume("costume2"), 1);
    ASSERT_EQ(stage.findCostume("costume3"), 2);
}

TEST(TargetTest, FindSound)
{
    Stage stage;
    Sound s1("sound1", "", "svg");
    Sound s2("sound2", "", "png");
    Sound s3("sound3", "", "svg");
    stage.addSound(s1);
    stage.addSound(s2);
    stage.addSound(s3);

    ASSERT_EQ(stage.findSound("invalid"), -1);
    ASSERT_EQ(stage.findSound("sound1"), 0);
    ASSERT_EQ(stage.findSound("sound2"), 1);
    ASSERT_EQ(stage.findSound("sound3"), 2);
}

TEST(TargetTest, FindVariable)
{
    Stage stage;
    auto v1 = std::make_shared<Variable>("a", "variable1");
    auto v2 = std::make_shared<Variable>("b", "variable2");
    auto v3 = std::make_shared<Variable>("c", "variable3");
    stage.addVariable(v1);
    stage.addVariable(v2);
    stage.addVariable(v3);

    ASSERT_EQ(stage.findVariable("invalid"), -1);
    ASSERT_EQ(stage.findVariable("variable1"), 0);
    ASSERT_EQ(stage.findVariable("variable2"), 1);
    ASSERT_EQ(stage.findVariable("variable3"), 2);

    ASSERT_EQ(stage.findVariableById("d"), -1);
    ASSERT_EQ(stage.findVariableById("a"), 0);
    ASSERT_EQ(stage.findVariableById("b"), 1);
    ASSERT_EQ(stage.findVariableById("c"), 2);
}

TEST(TargetTest, FindList)
{
    Stage stage;
    auto l1 = std::make_shared<List>("a", "list1");
    auto l2 = std::make_shared<List>("b", "list2");
    auto l3 = std::make_shared<List>("c", "list3");
    stage.addList(l1);
    stage.addList(l2);
    stage.addList(l3);

    ASSERT_EQ(stage.findList("invalid"), -1);
    ASSERT_EQ(stage.findList("list1"), 0);
    ASSERT_EQ(stage.findList("list2"), 1);
    ASSERT_EQ(stage.findList("list3"), 2);

    ASSERT_EQ(stage.findListById("d"), -1);
    ASSERT_EQ(stage.findListById("a"), 0);
    ASSERT_EQ(stage.findListById("b"), 1);
    ASSERT_EQ(stage.findListById("c"), 2);
}
