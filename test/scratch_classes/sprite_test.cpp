#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/rect.h>
#include <scratchcpp/project.h>
#include <enginemock.h>
#include <graphicseffectmock.h>
#include <spritehandlermock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::_;
using ::testing::SaveArg;
using ::testing::Return;

TEST(SpriteTest, IsStage)
{
    Sprite sprite;
    ASSERT_FALSE(sprite.isStage());
}

TEST(SpriteTest, Visible)
{
    Sprite sprite;
    ASSERT_TRUE(sprite.visible());
    EngineMock engine;
    sprite.setEngine(&engine);

    sprite.setVisible(false);
    ASSERT_FALSE(sprite.visible());

    EXPECT_CALL(engine, requestRedraw());
    sprite.setVisible(true);
    ASSERT_TRUE(sprite.visible());
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

    sprite.addCostume(std::make_shared<Costume>("", "", ""));
    sprite.addCostume(std::make_shared<Costume>("", "", ""));
    sprite.setCostumeIndex(1);
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
        Sprite *root = clone->cloneSprite();

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

        ASSERT_EQ(clone->costumeIndex(), 1);
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
    ASSERT_EQ(sprite.cloneSprite(), nullptr);

    ASSERT_EQ(sprite.clone(), nullptr);
    ASSERT_FALSE(sprite.isClone());
    ASSERT_EQ(sprite.cloneSprite(), nullptr);

    EngineMock engine;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, requestRedraw()).Times(2);
    EXPECT_CALL(engine, cloneLimit()).Times(6).WillRepeatedly(Return(300)); // clone count limit is tested later

    std::shared_ptr<Sprite> clone1;
    Sprite *clone1_2;
    EXPECT_CALL(engine, cloneCount()).WillOnce(Return(0));
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone1));
    EXPECT_CALL(engine, moveSpriteBehindOther(_, &sprite)).WillOnce(SaveArg<0>(&clone1_2));
    ASSERT_EQ(sprite.clone(), clone1);
    ASSERT_EQ(clone1.get(), clone1_2);
    ASSERT_FALSE(sprite.isClone());
    ASSERT_EQ(sprite.cloneSprite(), nullptr);

    ASSERT_TRUE(clone1->isClone());
    ASSERT_EQ(clone1->cloneSprite(), &sprite);

    checkCloneData(clone1.get());

    // Modify root sprite data to make sure parent is used
    sprite.setLayerOrder(3);

    std::shared_ptr<Sprite> clone2;
    Sprite *clone2_2;
    EXPECT_CALL(engine, cloneCount()).WillOnce(Return(1));
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone2));
    EXPECT_CALL(engine, moveSpriteBehindOther(_, clone1.get())).WillOnce(SaveArg<0>(&clone2_2));
    ASSERT_EQ(clone1->clone(), clone2);
    ASSERT_EQ(clone2.get(), clone2_2);
    ASSERT_TRUE(clone1->isClone());
    ASSERT_EQ(clone1->cloneSprite(), &sprite);
    ASSERT_TRUE(clone2->isClone());
    ASSERT_EQ(clone2->cloneSprite(), &sprite);

    checkCloneData(clone2.get());

    sprite.setVisible(true);

    std::shared_ptr<Sprite> clone3;
    Sprite *clone3_2;
    EXPECT_CALL(engine, cloneCount()).WillOnce(Return(2));
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone3));
    EXPECT_CALL(engine, moveSpriteBehindOther(_, clone1.get())).WillOnce(SaveArg<0>(&clone3_2));
    ASSERT_EQ(clone1->clone(), clone3);
    ASSERT_EQ(clone3.get(), clone3_2);

    std::shared_ptr<Sprite> clone4;
    Sprite *clone4_2;
    EXPECT_CALL(engine, cloneLimit()).WillOnce(Return(-1));
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone4));
    EXPECT_CALL(engine, moveSpriteBehindOther(_, &sprite)).WillOnce(SaveArg<0>(&clone4_2));
    ASSERT_EQ(sprite.clone(), clone4);
    ASSERT_EQ(clone4.get(), clone4_2);

    EXPECT_CALL(engine, cloneLimit()).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(engine, cloneCount()).WillOnce(Return(0));
    ASSERT_EQ(sprite.clone(), nullptr);

    EXPECT_CALL(engine, cloneLimit()).Times(2).WillRepeatedly(Return(150));
    EXPECT_CALL(engine, cloneCount()).WillOnce(Return(150));
    ASSERT_EQ(sprite.clone(), nullptr);

    // clones
    const auto &clones = sprite.clones();
    ASSERT_EQ(clones.size(), 4);
    ASSERT_EQ(clones[0], clone1);
    ASSERT_EQ(clones[1], clone2);
    ASSERT_EQ(clones[2], clone3);
    ASSERT_EQ(clones[3], clone4);

    ASSERT_EQ(clone1->clones(), clones);
    ASSERT_EQ(clone2->clones(), clones);
    ASSERT_EQ(clone3->clones(), clones);
    ASSERT_EQ(clone4->clones(), clones);

    ASSERT_EQ(clone2->costumes(), sprite.costumes());
    auto c2 = std::make_shared<Costume>("costume2", "", "png");
    clone2->addCostume(c2);
    ASSERT_EQ(clone2->costumes(), sprite.costumes());

    // Delete
    EXPECT_CALL(engine, deinitClone(clone1));
    clone1->deleteClone();

    ASSERT_EQ(clones.size(), 3);
    ASSERT_EQ(clones[0], clone2);
    ASSERT_EQ(clones[1], clone3);
    ASSERT_EQ(clones[2], clone4);

    EXPECT_CALL(engine, deinitClone(clone3));
    clone3->deleteClone();

    ASSERT_EQ(clones.size(), 2);
    ASSERT_EQ(clones[0], clone2);
    ASSERT_EQ(clones[1], clone4);

    EXPECT_CALL(engine, deinitClone(clone2));
    clone2->deleteClone();

    ASSERT_EQ(clones.size(), 1);
    ASSERT_EQ(clones[0], clone4);

    EXPECT_CALL(engine, deinitClone(clone4));
    clone4->deleteClone();
    ASSERT_TRUE(clones.empty());
}

TEST(SpriteTest, XY)
{
    Sprite sprite;
    ASSERT_EQ(sprite.x(), 0);
    ASSERT_EQ(sprite.y(), 0);

    EngineMock engine;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, requestRedraw()).Times(17);
    EXPECT_CALL(engine, spriteFencingEnabled()).Times(4).WillRepeatedly(Return(false));

    sprite.setX(-53.25);
    ASSERT_EQ(sprite.x(), -53.25);

    sprite.setX(259.999);
    ASSERT_EQ(sprite.x(), 259.999);

    sprite.setY(-53.25);
    ASSERT_EQ(sprite.y(), -53.25);

    sprite.setY(189.999);
    ASSERT_EQ(sprite.y(), 189.999);

    static const Rect rect(-44.6, 89.1, 20.5, -0.48);
    SpriteHandlerMock handler;
    EXPECT_CALL(handler, init);
    sprite.setInterface(&handler);

    sprite.setEngine(&engine);
    EXPECT_CALL(handler, onDirectionChanged);
    sprite.setDirection(34.45);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, boundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onXChanged);
    sprite.setX(319);
    ASSERT_EQ(sprite.x(), 319);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, boundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onXChanged);
    sprite.setX(75);
    ASSERT_EQ(sprite.x(), 75);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, boundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onXChanged);
    sprite.setX(400);
    ASSERT_EQ(sprite.x(), 344);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(false));
    EXPECT_CALL(handler, onXChanged);
    sprite.setX(400);
    ASSERT_EQ(sprite.x(), 400);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, boundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onXChanged);
    sprite.setX(-400);
    ASSERT_EQ(sprite.x(), 155);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(false));
    EXPECT_CALL(handler, onXChanged);
    sprite.setX(-400);
    ASSERT_EQ(sprite.x(), -400);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, boundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onYChanged);
    sprite.setY(150);
    ASSERT_EQ(sprite.y(), 150);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, boundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onYChanged);
    sprite.setY(-103);
    ASSERT_EQ(sprite.y(), -103);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, boundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onYChanged);
    sprite.setY(340);
    ASSERT_EQ(sprite.y(), 62);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(false));
    EXPECT_CALL(handler, onYChanged);
    sprite.setY(340);
    ASSERT_EQ(sprite.y(), 340);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, boundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onYChanged);
    sprite.setY(-340);
    ASSERT_EQ(sprite.y(), 86);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(false));
    EXPECT_CALL(handler, onYChanged);
    sprite.setY(-340);
    ASSERT_EQ(sprite.y(), -340);
}

TEST(SpriteTest, Size)
{
    Sprite sprite;
    ASSERT_EQ(sprite.size(), 100);

    EngineMock engine;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, requestRedraw()).Times(4);

    sprite.setSize(63.724);
    ASSERT_EQ(sprite.size(), 63.724);

    sprite.setCostumeIndex(1);

    sprite.setSize(186.84);
    ASSERT_EQ(sprite.size(), 186.84);

    sprite.setCostumeIndex(0);
}

TEST(SpriteTest, CostumeIndex)
{
    Sprite sprite;
    EngineMock engine;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, requestRedraw()).Times(2);

    auto c1 = std::make_shared<Costume>("", "", "");
    auto c2 = std::make_shared<Costume>("", "", "");
    sprite.addCostume(c1);
    sprite.addCostume(c2);

    sprite.setCostumeIndex(0);
    ASSERT_EQ(sprite.costumeIndex(), 0);

    sprite.setCostumeIndex(1);
    ASSERT_EQ(sprite.costumeIndex(), 1);
}

TEST(SpriteTest, Direction)
{
    Sprite sprite;
    ASSERT_EQ(sprite.direction(), 90);

    EngineMock engine;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, requestRedraw()).Times(10);

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
    Sprite sprite;
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround); // default
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");

    EngineMock engine;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, requestRedraw()).Times(8);

    sprite.setRotationStyle(Sprite::RotationStyle::DoNotRotate);
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::DoNotRotate);
    ASSERT_EQ(sprite.rotationStyleStr(), "don't rotate");

    sprite.setRotationStyle(Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(sprite.rotationStyleStr(), "left-right");

    sprite.setRotationStyle(Sprite::RotationStyle::AllAround);
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround);
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");

    sprite.setRotationStyle("invalid");
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround); // shouldn't change
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");

    sprite.setCostumeIndex(1);
    sprite.setRotationStyle(Sprite::RotationStyle::LeftRight);

    sprite.setCostumeIndex(0);
    sprite.setRotationStyle(Sprite::RotationStyle::AllAround);
    sprite.setCostumeIndex(1);

    sprite.setVisible(false);

    sprite.setRotationStyle("don't rotate");
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::DoNotRotate);
    ASSERT_EQ(sprite.rotationStyleStr(), "don't rotate");

    sprite.setRotationStyle("left-right");
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(sprite.rotationStyleStr(), "left-right");

    sprite.setRotationStyle("all around");
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround);
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");

    sprite.setRotationStyle(std::string("invalid"));
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround); // shouldn't change
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");

    sprite.setRotationStyle(std::string("don't rotate"));
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::DoNotRotate);
    ASSERT_EQ(sprite.rotationStyleStr(), "don't rotate");

    sprite.setRotationStyle(std::string("left-right"));
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(sprite.rotationStyleStr(), "left-right");

    sprite.setRotationStyle(std::string("all around"));
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround);
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");
}

TEST(SpriteTest, KeepInFence)
{
    Sprite sprite;
    SpriteHandlerMock handler;
    EXPECT_CALL(handler, init);
    sprite.setInterface(&handler);

    double fencedX = -1, fencedY = -1;
    EngineMock engine;
    sprite.setEngine(&engine);

    static const Rect rect(-44.6, 89.1, 20.5, -0.48);
    EXPECT_CALL(engine, requestRedraw());
    EXPECT_CALL(handler, onDirectionChanged);
    sprite.setDirection(45);
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, boundingRect()).WillOnce(Return(rect));
    sprite.keepInFence(0, 0, &fencedX, &fencedY);
    ASSERT_EQ(fencedX, 0);
    ASSERT_EQ(fencedY, 0);

    EXPECT_CALL(engine, requestRedraw()).Times(2);
    EXPECT_CALL(engine, spriteFencingEnabled()).Times(2).WillRepeatedly(Return(false));
    EXPECT_CALL(handler, onXChanged);
    EXPECT_CALL(handler, onYChanged);
    sprite.setX(100);
    sprite.setY(60);
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, boundingRect()).WillOnce(Return(rect));
    sprite.keepInFence(400, 340, &fencedX, &fencedY);
    ASSERT_EQ(std::round(fencedX * 100) / 100, 319.5);
    ASSERT_EQ(std::round(fencedY * 100) / 100, 150.9);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, boundingRect()).WillOnce(Return(rect));
    sprite.keepInFence(400, -340, &fencedX, &fencedY);
    ASSERT_EQ(std::round(fencedX * 100) / 100, 319.5);
    ASSERT_EQ(std::round(fencedY * 100) / 100, -119.52);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, boundingRect()).WillOnce(Return(rect));
    sprite.keepInFence(-400, -340, &fencedX, &fencedY);
    ASSERT_EQ(std::round(fencedX * 100) / 100, -95.4);
    ASSERT_EQ(std::round(fencedY * 100) / 100, -119.52);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, boundingRect()).WillOnce(Return(rect));
    sprite.keepInFence(-400, 340, &fencedX, &fencedY);
    ASSERT_EQ(std::round(fencedX * 100) / 100, -95.4);
    ASSERT_EQ(std::round(fencedY * 100) / 100, 150.9);
}

TEST(SpriteTest, GraphicsEffects)
{
    Sprite sprite;

    EngineMock engine;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, requestRedraw()).Times(3);

    GraphicsEffectMock effect1, effect2;
    sprite.setGraphicsEffectValue(&effect1, 48.21);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect1), 48.21);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect2), 0);

    sprite.setGraphicsEffectValue(&effect2, -107.08);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect1), 48.21);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect2), -107.08);

    sprite.clearGraphicsEffects();
    ASSERT_EQ(sprite.graphicsEffectValue(&effect1), 0);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect2), 0);

    sprite.setVisible(false);
    sprite.setGraphicsEffectValue(&effect2, -107.08);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect1), 0);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect2), -107.08);

    sprite.clearGraphicsEffects();
    ASSERT_EQ(sprite.graphicsEffectValue(&effect1), 0);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect2), 0);
}
