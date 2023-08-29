#include <scratchcpp/target.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/block.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/sound.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(TargetTest, IsStage)
{
    Target target;
    ASSERT_FALSE(target.isStage());
}

TEST(TargetTest, Name)
{
    Target target;
    target.setName("Test");
    ASSERT_EQ(target.name(), "Test");
}

TEST(TargetTest, Variables)
{
    auto v1 = std::make_shared<Variable>("a", "var1");
    auto v2 = std::make_shared<Variable>("b", "var2");
    auto v3 = std::make_shared<Variable>("c", "var3");

    Target target;
    ASSERT_EQ(target.addVariable(v1), 0);
    ASSERT_EQ(target.addVariable(v2), 1);
    ASSERT_EQ(target.addVariable(v3), 2);
    ASSERT_EQ(target.addVariable(v2), 1); // add existing variable

    ASSERT_EQ(target.variables(), std::vector<std::shared_ptr<Variable>>({ v1, v2, v3 }));
    ASSERT_EQ(target.variableAt(0), v1);
    ASSERT_EQ(target.variableAt(1), v2);
    ASSERT_EQ(target.variableAt(2), v3);
    ASSERT_EQ(target.variableAt(3), nullptr);
    ASSERT_EQ(target.variableAt(-1), nullptr);

    ASSERT_EQ(target.findVariable("invalid"), -1);
    ASSERT_EQ(target.findVariable("var1"), 0);
    ASSERT_EQ(target.findVariable("var2"), 1);
    ASSERT_EQ(target.findVariable("var3"), 2);

    ASSERT_EQ(target.findVariableById("d"), -1);
    ASSERT_EQ(target.findVariableById("a"), 0);
    ASSERT_EQ(target.findVariableById("b"), 1);
    ASSERT_EQ(target.findVariableById("c"), 2);
}

TEST(TargetTest, Lists)
{
    auto l1 = std::make_shared<List>("a", "list1");
    auto l2 = std::make_shared<List>("b", "list2");
    auto l3 = std::make_shared<List>("c", "list3");

    Target target;
    ASSERT_EQ(target.addList(l1), 0);
    ASSERT_EQ(target.addList(l2), 1);
    ASSERT_EQ(target.addList(l3), 2);
    ASSERT_EQ(target.addList(l2), 1); // add existing list

    ASSERT_EQ(target.lists(), std::vector<std::shared_ptr<List>>({ l1, l2, l3 }));
    ASSERT_EQ(target.listAt(0), l1);
    ASSERT_EQ(target.listAt(1), l2);
    ASSERT_EQ(target.listAt(2), l3);
    ASSERT_EQ(target.listAt(3), nullptr);
    ASSERT_EQ(target.listAt(-1), nullptr);

    ASSERT_EQ(target.findList("invalid"), -1);
    ASSERT_EQ(target.findList("list1"), 0);
    ASSERT_EQ(target.findList("list2"), 1);
    ASSERT_EQ(target.findList("list3"), 2);

    ASSERT_EQ(target.findListById("d"), -1);
    ASSERT_EQ(target.findListById("a"), 0);
    ASSERT_EQ(target.findListById("b"), 1);
    ASSERT_EQ(target.findListById("c"), 2);
}

TEST(TargetTest, Blocks)
{
    auto b1 = std::make_shared<Block>("a", "event_whenflagclicked");
    auto b2 = std::make_shared<Block>("b", "motion_gotoxy");
    auto b3 = std::make_shared<Block>("c", "motion_ifonedgebounce");
    auto b4 = std::make_shared<Block>("d", "event_whenflagclicked");

    Target target;
    ASSERT_EQ(target.addBlock(b1), 0);
    ASSERT_EQ(target.addBlock(b2), 1);
    ASSERT_EQ(target.addBlock(b3), 2);
    ASSERT_EQ(target.addBlock(b4), 3);
    ASSERT_EQ(target.addBlock(b2), 1); // add existing block

    ASSERT_EQ(target.blocks(), std::vector<std::shared_ptr<Block>>({ b1, b2, b3, b4 }));
    ASSERT_EQ(target.blockAt(0), b1);
    ASSERT_EQ(target.blockAt(1), b2);
    ASSERT_EQ(target.blockAt(2), b3);
    ASSERT_EQ(target.blockAt(3), b4);
    ASSERT_EQ(target.blockAt(4), nullptr);
    ASSERT_EQ(target.blockAt(-1), nullptr);

    ASSERT_EQ(target.findBlock("e"), -1);
    ASSERT_EQ(target.findBlock("a"), 0);
    ASSERT_EQ(target.findBlock("b"), 1);
    ASSERT_EQ(target.findBlock("c"), 2);
    ASSERT_EQ(target.findBlock("d"), 3);

    ASSERT_EQ(target.greenFlagBlocks(), std::vector<std::shared_ptr<Block>>({ b1, b4 }));
}

TEST(TargetTest, CurrentCostume)
{
    Target target;
    ASSERT_EQ(target.currentCostume(), 1);
    target.setCurrentCostume(5);
    ASSERT_EQ(target.currentCostume(), 5);
}

TEST(TargetTest, Costumes)
{
    auto c1 = std::make_shared<Costume>("costume1", "", "svg");
    auto c2 = std::make_shared<Costume>("costume2", "", "png");
    auto c3 = std::make_shared<Costume>("costume3", "", "svg");

    Target target;
    ASSERT_EQ(target.addCostume(c1), 0);
    ASSERT_EQ(target.addCostume(c2), 1);
    ASSERT_EQ(target.addCostume(c3), 2);
    ASSERT_EQ(target.addCostume(c2), 1); // add existing costume

    // TODO: Use shared_ptr for assets
    // ASSERT_EQ(target.costumes(), std::vector<std::shared_ptr<Costume>>({ c1, c2, c3 }));
    ASSERT_EQ(target.costumes().size(), 3);
    ASSERT_EQ(target.costumes()[0]->name(), c1->name());
    ASSERT_EQ(target.costumes()[1]->name(), c2->name());
    ASSERT_EQ(target.costumes()[2]->name(), c3->name());

    ASSERT_EQ(target.costumeAt(0)->name(), c1->name());
    ASSERT_EQ(target.costumeAt(1)->name(), c2->name());
    ASSERT_EQ(target.costumeAt(2)->name(), c3->name());

    ASSERT_EQ(target.findCostume("invalid"), -1);
    ASSERT_EQ(target.findCostume("costume1"), 0);
    ASSERT_EQ(target.findCostume("costume2"), 1);
    ASSERT_EQ(target.findCostume("costume3"), 2);
}

TEST(TargetTest, Sounds)
{
    auto s1 = std::make_shared<Sound>("sound1", "", "mp3");
    auto s2 = std::make_shared<Sound>("sound2", "", "wav");
    auto s3 = std::make_shared<Sound>("sound3", "", "mp3");

    Target target;
    ASSERT_EQ(target.addSound(s1), 0);
    ASSERT_EQ(target.addSound(s2), 1);
    ASSERT_EQ(target.addSound(s3), 2);
    ASSERT_EQ(target.addSound(s2), 1); // add existing Sound

    // TODO: Use shared_ptr for assets
    // ASSERT_EQ(target.sounds(), std::vector<std::shared_ptr<Sound>>({ s1, s2, s3 }));
    ASSERT_EQ(target.sounds().size(), 3);
    ASSERT_EQ(target.sounds()[0]->name(), s1->name());
    ASSERT_EQ(target.sounds()[1]->name(), s2->name());
    ASSERT_EQ(target.sounds()[2]->name(), s3->name());

    ASSERT_EQ(target.soundAt(0)->name(), s1->name());
    ASSERT_EQ(target.soundAt(1)->name(), s2->name());
    ASSERT_EQ(target.soundAt(2)->name(), s3->name());

    ASSERT_EQ(target.findSound("invalid"), -1);
    ASSERT_EQ(target.findSound("sound1"), 0);
    ASSERT_EQ(target.findSound("sound2"), 1);
    ASSERT_EQ(target.findSound("sound3"), 2);
}

TEST(TargetTest, LayerOrder)
{
    Target target;
    ASSERT_EQ(target.layerOrder(), 0);
    target.setLayerOrder(2);
    ASSERT_EQ(target.layerOrder(), 2);
}

TEST(TargetTest, Volume)
{
    Target target;
    ASSERT_EQ(target.volume(), 100);
    target.setVolume(50);
    ASSERT_EQ(target.volume(), 50);
}
