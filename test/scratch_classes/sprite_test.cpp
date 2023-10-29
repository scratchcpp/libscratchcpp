#include <scratchcpp/sprite.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/costume.h>
#include <enginemock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::_;
using ::testing::SaveArg;

TEST(SpriteTest, IsStage)
{
    Sprite sprite;
    ASSERT_FALSE(sprite.isStage());
}

TEST(SpriteTest, Visible)
{
    Sprite sprite;
    ASSERT_TRUE(sprite.visible());
    sprite.setVisible(false);
    ASSERT_FALSE(sprite.visible());
}

TEST(SpriteTest, Clone)
{
    Sprite sprite;
    sprite.setName("Sprite1");
    auto var1 = std::make_shared<Variable>("a", "var1", "hello");
    auto var2 = std::make_shared<Variable>("b", "var2", "world");
    sprite.addVariable(var1);
    sprite.addVariable(var2);
    auto c1 = std::make_shared<Costume>("costume1", "", "svg");
    sprite.addCostume(c1);

    auto list1 = std::make_shared<List>("c", "list1");
    list1->push_back("item1");
    list1->push_back("item2");
    auto list2 = std::make_shared<List>("d", "list2");
    list2->push_back("test");
    sprite.addList(list1);
    sprite.addList(list2);

    sprite.setCurrentCostume(2);
    sprite.setLayerOrder(5);
    sprite.setVolume(50);

    sprite.setVisible(false);
    sprite.setX(100.25);
    sprite.setY(-45.43);
    sprite.setSize(54.121);
    sprite.setDirection(179.4);
    sprite.setDraggable(true);
    sprite.setRotationStyle(Sprite::RotationStyle::DoNotRotate);

    auto checkCloneData = [](Sprite *clone) {
        ASSERT_TRUE(clone);
        Sprite *root = clone->cloneRoot();

        ASSERT_EQ(clone->name(), "Sprite1");
        ASSERT_EQ(clone->variables().size(), 2);
        ASSERT_NE(clone->variables(), root->variables());
        ASSERT_EQ(clone->variableAt(0)->id(), "a");
        ASSERT_EQ(clone->variableAt(0)->name(), "var1");
        ASSERT_EQ(clone->variableAt(0)->value().toString(), "hello");
        ASSERT_EQ(clone->variableAt(1)->id(), "b");
        ASSERT_EQ(clone->variableAt(1)->name(), "var2");
        ASSERT_EQ(clone->variableAt(1)->value().toString(), "world");
        ASSERT_EQ(clone->variableAt(1)->target(), clone);

        ASSERT_EQ(clone->lists().size(), 2);
        ASSERT_NE(clone->lists(), root->lists());
        ASSERT_EQ(clone->listAt(0)->id(), "c");
        ASSERT_EQ(clone->listAt(0)->name(), "list1");
        ASSERT_EQ(*clone->listAt(0), std::deque<Value>({ "item1", "item2" }));
        ASSERT_EQ(clone->listAt(1)->id(), "d");
        ASSERT_EQ(clone->listAt(1)->name(), "list2");
        ASSERT_EQ(*clone->listAt(1), std::deque<Value>({ "test" }));
        ASSERT_EQ(clone->listAt(1)->target(), clone);

        ASSERT_EQ(clone->currentCostume(), 2);
        ASSERT_EQ(clone->layerOrder(), 5);
        ASSERT_EQ(clone->volume(), 50);
        ASSERT_EQ(clone->engine(), root->engine());

        ASSERT_EQ(clone->visible(), false);
        ASSERT_EQ(clone->x(), 100.25);
        ASSERT_EQ(clone->y(), -45.43);
        ASSERT_EQ(clone->size(), 54.121);
        ASSERT_EQ(clone->direction(), 179.4);
        ASSERT_EQ(clone->draggable(), true);
        ASSERT_EQ(clone->rotationStyle(), Sprite::RotationStyle::DoNotRotate);
    };

    ASSERT_FALSE(sprite.isClone());
    ASSERT_EQ(sprite.cloneRoot(), nullptr);
    ASSERT_EQ(sprite.cloneParent(), nullptr);

    ASSERT_EQ(sprite.clone(), nullptr);
    ASSERT_FALSE(sprite.isClone());
    ASSERT_EQ(sprite.cloneRoot(), nullptr);
    ASSERT_EQ(sprite.cloneParent(), nullptr);

    EngineMock engine;
    sprite.setEngine(&engine);

    Sprite *clone1;
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone1));
    ASSERT_EQ(sprite.clone().get(), clone1);
    ASSERT_FALSE(sprite.isClone());
    ASSERT_EQ(sprite.cloneRoot(), nullptr);
    ASSERT_EQ(sprite.cloneParent(), nullptr);

    ASSERT_TRUE(clone1->isClone());
    ASSERT_EQ(clone1->cloneRoot(), &sprite);
    ASSERT_EQ(clone1->cloneParent(), &sprite);

    checkCloneData(clone1);

    // Modify root sprite data to make sure parent is used
    sprite.setLayerOrder(3);

    Sprite *clone2;
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone2));
    ASSERT_EQ(clone1->clone().get(), clone2);
    ASSERT_TRUE(clone1->isClone());
    ASSERT_EQ(clone1->cloneRoot(), &sprite);
    ASSERT_EQ(clone1->cloneParent(), &sprite);
    ASSERT_TRUE(clone2->isClone());
    ASSERT_EQ(clone2->cloneRoot(), &sprite);
    ASSERT_EQ(clone2->cloneParent(), clone1);

    checkCloneData(clone2);

    Sprite *clone3;
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone3));
    ASSERT_EQ(clone1->clone().get(), clone3);

    Sprite *clone4;
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone4));
    ASSERT_EQ(sprite.clone().get(), clone4);

    // children
    const auto &children1 = sprite.children();
    ASSERT_EQ(children1.size(), 2);
    ASSERT_EQ(children1[0].get(), clone1);
    ASSERT_EQ(children1[1].get(), clone4);

    const auto &children2 = clone1->children();
    ASSERT_EQ(children2.size(), 2);
    ASSERT_EQ(children2[0].get(), clone2);
    ASSERT_EQ(children2[1].get(), clone3);

    ASSERT_TRUE(clone2->children().empty());

    ASSERT_TRUE(clone3->children().empty());

    ASSERT_TRUE(clone4->children().empty());

    // allChildren
    auto allChildren = sprite.allChildren();
    ASSERT_EQ(allChildren.size(), 4);
    ASSERT_EQ(allChildren[0].get(), clone1);
    ASSERT_EQ(allChildren[1].get(), clone2);
    ASSERT_EQ(allChildren[2].get(), clone3);
    ASSERT_EQ(allChildren[3].get(), clone4);

    allChildren = clone1->allChildren();
    ASSERT_EQ(allChildren.size(), 2);
    ASSERT_EQ(allChildren[0].get(), clone2);
    ASSERT_EQ(allChildren[1].get(), clone3);

    ASSERT_TRUE(clone2->allChildren().empty());

    ASSERT_TRUE(clone3->allChildren().empty());

    ASSERT_TRUE(clone4->allChildren().empty());

    ASSERT_EQ(clone2->costumes(), sprite.costumes());
    auto c2 = std::make_shared<Costume>("costume2", "", "png");
    clone2->addCostume(c2);
    ASSERT_EQ(clone2->costumes(), sprite.costumes());
}

TEST(SpriteTest, X)
{
    Sprite sprite;
    ASSERT_EQ(sprite.x(), 0);

    sprite.setX(-53.25);
    ASSERT_EQ(sprite.x(), -53.25);

    sprite.setX(239.999);
    ASSERT_EQ(sprite.x(), 239.999);
}

TEST(SpriteTest, Y)
{
    Sprite sprite;
    ASSERT_EQ(sprite.y(), 0);

    sprite.setY(-53.25);
    ASSERT_EQ(sprite.y(), -53.25);

    sprite.setY(179.999);
    ASSERT_EQ(sprite.y(), 179.999);
}

TEST(SpriteTest, Size)
{
    auto c1 = std::make_shared<Costume>("", "", "");
    auto c2 = std::make_shared<Costume>("", "", "");

    Sprite sprite;
    ASSERT_EQ(sprite.size(), 100);
    sprite.addCostume(c1);
    sprite.addCostume(c2);
    sprite.setCurrentCostume(1);

    ASSERT_EQ(c1->scale(), 1);

    sprite.setSize(63.724);
    ASSERT_EQ(sprite.size(), 63.724);
    ASSERT_EQ(std::round(c1->scale() * 100000) / 100000, 0.63724);

    sprite.setCurrentCostume(2);
    ASSERT_EQ(std::round(c2->scale() * 100000) / 100000, 0.63724);

    sprite.setSize(186.84);
    ASSERT_EQ(sprite.size(), 186.84);
    ASSERT_EQ(std::round(c2->scale() * 10000) / 10000, 1.8684);

    sprite.setCurrentCostume(1);
    ASSERT_EQ(std::round(c1->scale() * 10000) / 10000, 1.8684);
}

TEST(SpriteTest, Direction)
{
    Sprite sprite;
    ASSERT_EQ(sprite.direction(), 90);

    sprite.setDirection(-42.75);
    ASSERT_EQ(sprite.direction(), -42.75);

    sprite.setDirection(145.2);
    ASSERT_EQ(sprite.direction(), 145.2);

    sprite.setDirection(182.5);
    ASSERT_EQ(sprite.direction(), -177.5);

    sprite.setDirection(270);
    ASSERT_EQ(sprite.direction(), -90);

    sprite.setDirection(360);
    ASSERT_EQ(sprite.direction(), 0);

    sprite.setDirection(500);
    ASSERT_EQ(sprite.direction(), 140);

    sprite.setDirection(-182.5);
    ASSERT_EQ(sprite.direction(), 177.5);

    sprite.setDirection(-270);
    ASSERT_EQ(sprite.direction(), 90);

    sprite.setDirection(-360);
    ASSERT_EQ(sprite.direction(), 0);

    sprite.setDirection(-500);
    ASSERT_EQ(sprite.direction(), -140);
}

TEST(SpriteTest, Draggable)
{
    Sprite sprite;
    ASSERT_FALSE(sprite.draggable());
    sprite.setDraggable(true);
    ASSERT_TRUE(sprite.draggable());
}

TEST(SpriteTest, RotationStyle)
{
    auto c1 = std::make_shared<Costume>("", "", "");
    auto c2 = std::make_shared<Costume>("", "", "");

    Sprite sprite;
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround); // default
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");
    sprite.addCostume(c1);
    sprite.addCostume(c2);
    sprite.setCurrentCostume(1);

    ASSERT_EQ(c1->mirrorHorizontally(), false);

    sprite.setRotationStyle(Sprite::RotationStyle::DoNotRotate);
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::DoNotRotate);
    ASSERT_EQ(sprite.rotationStyleStr(), "don't rotate");
    ASSERT_EQ(c1->mirrorHorizontally(), false);

    sprite.setRotationStyle(Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(sprite.rotationStyleStr(), "left-right");
    ASSERT_EQ(c1->mirrorHorizontally(), true);

    sprite.setRotationStyle(Sprite::RotationStyle::AllAround);
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround);
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");
    ASSERT_EQ(c1->mirrorHorizontally(), false);

    sprite.setRotationStyle("invalid");
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround); // shouldn't change
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");
    ASSERT_EQ(c1->mirrorHorizontally(), false);

    sprite.setCurrentCostume(2);
    sprite.setRotationStyle(Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(c2->mirrorHorizontally(), true);

    sprite.setCurrentCostume(1);
    sprite.setRotationStyle(Sprite::RotationStyle::AllAround);
    sprite.setCurrentCostume(2);
    ASSERT_EQ(c2->mirrorHorizontally(), false);

    sprite.setRotationStyle("don't rotate");
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::DoNotRotate);
    ASSERT_EQ(sprite.rotationStyleStr(), "don't rotate");
    ASSERT_EQ(c2->mirrorHorizontally(), false);

    sprite.setRotationStyle("left-right");
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(sprite.rotationStyleStr(), "left-right");
    ASSERT_EQ(c2->mirrorHorizontally(), true);

    sprite.setRotationStyle("all around");
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround);
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");
    ASSERT_EQ(c2->mirrorHorizontally(), false);

    sprite.setRotationStyle(std::string("invalid"));
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround); // shouldn't change
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");
    ASSERT_EQ(c2->mirrorHorizontally(), false);

    sprite.setRotationStyle(std::string("don't rotate"));
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::DoNotRotate);
    ASSERT_EQ(sprite.rotationStyleStr(), "don't rotate");
    ASSERT_EQ(c2->mirrorHorizontally(), false);

    sprite.setRotationStyle(std::string("left-right"));
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(sprite.rotationStyleStr(), "left-right");
    ASSERT_EQ(c2->mirrorHorizontally(), true);

    sprite.setRotationStyle(std::string("all around"));
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround);
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");
    ASSERT_EQ(c2->mirrorHorizontally(), false);
}
