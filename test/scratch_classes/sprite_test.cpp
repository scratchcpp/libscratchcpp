#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/textbubble.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/sound.h>
#include <scratchcpp/rect.h>
#include <scratchcpp/project.h>
#include <enginemock.h>
#include <graphicseffectmock.h>
#include <spritehandlermock.h>
#include <targetmock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::_;
using ::testing::SaveArg;
using ::testing::Return;
using ::testing::WithArgs;
using ::testing::Invoke;
using ::testing::_;

TEST(SpriteTest, IsStage)
{
    Sprite sprite;
    ASSERT_FALSE(sprite.isStage());
}

TEST(SpriteTest, Interface)
{
    Sprite sprite;
    ASSERT_EQ(sprite.getInterface(), nullptr);

    SpriteHandlerMock handler;
    EXPECT_CALL(handler, init);
    sprite.setInterface(&handler);
    ASSERT_EQ(sprite.getInterface(), &handler);
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
    auto s1 = std::make_shared<Sound>("sound1", "a", "wav");
    auto s2 = std::make_shared<Sound>("sound2", "b", "wav");
    auto s3 = std::make_shared<Sound>("sound3", "c", "wav");
    sprite.addSound(s1);
    sprite.addSound(s2);
    sprite.addSound(s3);

    auto list1 = std::make_shared<List>("c", "list1");
    list1->append("item1");
    list1->append("item2");
    auto list2 = std::make_shared<List>("d", "list2");
    list2->append("test");
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
        ASSERT_EQ(clone->listAt(0)->size(), 2);
        ASSERT_EQ(clone->listAt(0)->toString(), "item1 item2");
        ASSERT_EQ(clone->listAt(1)->id(), "d");
        ASSERT_EQ(clone->listAt(1)->name(), "list2");
        ASSERT_EQ(clone->listAt(1)->size(), 1);
        ASSERT_EQ(clone->listAt(1)->toString(), "test");
        ASSERT_EQ(clone->listAt(1)->target(), clone);

        ASSERT_EQ(clone->sounds().size(), 3);
        ASSERT_EQ(clone->soundAt(0)->id(), "a");
        ASSERT_EQ(clone->soundAt(0)->name(), "sound1");
        ASSERT_EQ(clone->soundAt(0)->dataFormat(), "wav");
        ASSERT_EQ(clone->soundAt(2)->id(), "c");
        ASSERT_EQ(clone->soundAt(2)->name(), "sound3");
        ASSERT_EQ(clone->soundAt(2)->dataFormat(), "wav");

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
    Drawable *clone1_2;
    EXPECT_CALL(engine, cloneCount()).WillOnce(Return(0));
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone1));
    EXPECT_CALL(engine, moveDrawableBehindOther(_, &sprite)).WillOnce(SaveArg<0>(&clone1_2));
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
    Drawable *clone2_2;
    EXPECT_CALL(engine, cloneCount()).WillOnce(Return(1));
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone2));
    EXPECT_CALL(engine, moveDrawableBehindOther(_, clone1.get())).WillOnce(SaveArg<0>(&clone2_2));
    ASSERT_EQ(clone1->clone(), clone2);
    ASSERT_EQ(clone2.get(), clone2_2);
    ASSERT_TRUE(clone1->isClone());
    ASSERT_EQ(clone1->cloneSprite(), &sprite);
    ASSERT_TRUE(clone2->isClone());
    ASSERT_EQ(clone2->cloneSprite(), &sprite);

    checkCloneData(clone2.get());

    sprite.setVisible(true);

    std::shared_ptr<Sprite> clone3;
    Drawable *clone3_2;
    EXPECT_CALL(engine, cloneCount()).WillOnce(Return(2));
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone3));
    EXPECT_CALL(engine, moveDrawableBehindOther(_, clone1.get())).WillOnce(SaveArg<0>(&clone3_2));
    ASSERT_EQ(clone1->clone(), clone3);
    ASSERT_EQ(clone3.get(), clone3_2);

    std::shared_ptr<Sprite> clone4;
    Drawable *clone4_2;
    EXPECT_CALL(engine, cloneLimit()).WillOnce(Return(-1));
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone4));
    EXPECT_CALL(engine, moveDrawableBehindOther(_, &sprite)).WillOnce(SaveArg<0>(&clone4_2));
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
    EXPECT_CALL(engine, requestRedraw()).Times(18);
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
    EXPECT_CALL(handler, fastBoundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onXChanged);
    sprite.setX(319);
    ASSERT_EQ(sprite.x(), 319);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, fastBoundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onXChanged);
    sprite.setX(75);
    ASSERT_EQ(sprite.x(), 75);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, fastBoundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onXChanged);
    sprite.setX(400);
    ASSERT_EQ(sprite.x(), 344);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(false));
    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onXChanged);
    sprite.setX(400);
    ASSERT_EQ(sprite.x(), 400);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, fastBoundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onXChanged);
    sprite.setX(-400);
    ASSERT_EQ(sprite.x(), 155);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(false));
    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onXChanged);
    sprite.setX(-400);
    ASSERT_EQ(sprite.x(), -400);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, fastBoundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onYChanged);
    sprite.setY(150);
    ASSERT_EQ(sprite.y(), 150);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, fastBoundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onYChanged);
    sprite.setY(-103);
    ASSERT_EQ(sprite.y(), -103);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, fastBoundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onYChanged);
    sprite.setY(340);
    ASSERT_EQ(sprite.y(), 62);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(false));
    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onYChanged);
    sprite.setY(340);
    ASSERT_EQ(sprite.y(), 340);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    EXPECT_CALL(handler, fastBoundingRect()).WillOnce(Return(rect));
    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onYChanged);
    sprite.setY(-340);
    ASSERT_EQ(sprite.y(), 86);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(false));
    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onYChanged);
    sprite.setY(-340);
    ASSERT_EQ(sprite.y(), -340);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(false));
    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onXChanged);
    EXPECT_CALL(handler, onYChanged);
    sprite.setPosition(56, -23);
    ASSERT_EQ(sprite.x(), 56);
    ASSERT_EQ(sprite.y(), -23);
}

TEST(SpriteTest, Dragging)
{
    Sprite sprite;
    SpriteHandlerMock handler;
    EXPECT_CALL(handler, init);
    sprite.setInterface(&handler);
    ASSERT_FALSE(sprite.dragging());

    EngineMock engine;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, spriteFencingEnabled).WillRepeatedly(Return(false));
    EXPECT_CALL(engine, requestRedraw).WillRepeatedly(Return());

    EXPECT_CALL(engine, moveDrawableToFront(&sprite));
    sprite.startDragging();
    ASSERT_TRUE(sprite.dragging());

    sprite.setX(10);
    ASSERT_EQ(sprite.x(), 0);
    sprite.setY(-56);
    ASSERT_EQ(sprite.y(), 0);
    sprite.setPosition(-2.8, 54.1);
    ASSERT_EQ(sprite.x(), 0);
    ASSERT_EQ(sprite.y(), 0);

    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onXChanged);
    EXPECT_CALL(handler, onYChanged);
    sprite.dragToPosition(34.2, -89.7);
    ASSERT_EQ(sprite.x(), 34.2);
    ASSERT_EQ(sprite.y(), -89.7);
    ASSERT_TRUE(sprite.dragging());

    sprite.stopDragging();
    ASSERT_FALSE(sprite.dragging());

    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onXChanged);
    EXPECT_CALL(handler, onYChanged);
    sprite.setPosition(-2.8, 54.1);
    ASSERT_EQ(sprite.x(), -2.8);
    ASSERT_EQ(sprite.y(), 54.1);

    EXPECT_CALL(handler, onMoved);
    EXPECT_CALL(handler, onXChanged);
    EXPECT_CALL(handler, onYChanged);
    sprite.dragToPosition(-34.2, 89.7);
    ASSERT_EQ(sprite.x(), -34.2);
    ASSERT_EQ(sprite.y(), 89.7);
    ASSERT_TRUE(sprite.dragging());

    sprite.setPosition(-2.8, 54.1);
    ASSERT_EQ(sprite.x(), -34.2);
    ASSERT_EQ(sprite.y(), 89.7);

    sprite.stopDragging();
    ASSERT_FALSE(sprite.dragging());
}

TEST(SpriteTest, Size)
{
    Sprite sprite;
    SpriteHandlerMock handler;
    EXPECT_CALL(handler, init);
    sprite.setInterface(&handler);
    ASSERT_EQ(sprite.size(), 100);

    EngineMock engine;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, requestRedraw()).WillRepeatedly(Return());
    EXPECT_CALL(engine, stageWidth()).WillRepeatedly(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillRepeatedly(Return(360));
    EXPECT_CALL(handler, onSizeChanged).WillRepeatedly(Return());

    EXPECT_CALL(handler, costumeWidth()).WillRepeatedly(Return(50));
    EXPECT_CALL(handler, costumeHeight()).WillRepeatedly(Return(30));
    sprite.setSize(63.724);
    ASSERT_EQ(sprite.size(), 63.724);
    sprite.setSize(0);
    ASSERT_EQ(std::round(sprite.size() * 100) / 100, 16.67);
    sprite.setSize(2000);
    ASSERT_EQ(sprite.size(), 1440);

    EXPECT_CALL(handler, costumeWidth()).WillRepeatedly(Return(30));
    EXPECT_CALL(handler, costumeHeight()).WillRepeatedly(Return(50));
    sprite.setSize(0);
    ASSERT_EQ(std::round(sprite.size() * 100) / 100, 16.67);
    sprite.setSize(2000);
    ASSERT_EQ(sprite.size(), 1080);

    EXPECT_CALL(handler, costumeWidth()).WillRepeatedly(Return(88));
    EXPECT_CALL(handler, costumeHeight()).WillRepeatedly(Return(88));
    sprite.setSize(0);
    ASSERT_EQ(std::round(sprite.size() * 100) / 100, 5.68);
    sprite.setSize(2000);
    ASSERT_EQ(std::round(sprite.size() * 100) / 100, 613.64);

    sprite.setSize(186.84);
    ASSERT_EQ(sprite.size(), 186.84);
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

TEST(SpriteTest, LayerOrder)
{
    Sprite sprite;
    ASSERT_EQ(sprite.layerOrder(), 0);
    sprite.setLayerOrder(2);
    ASSERT_EQ(sprite.layerOrder(), 2);
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
    EXPECT_CALL(handler, onMoved).Times(2);
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

TEST(SpriteTest, CurrentCostumeWidth)
{
    Sprite sprite;
    SpriteHandlerMock handler;
    EXPECT_CALL(handler, init);
    sprite.setInterface(&handler);

    EXPECT_CALL(handler, costumeWidth()).WillOnce(Return(46));
    ASSERT_EQ(sprite.currentCostumeWidth(), 46);
}

TEST(SpriteTest, CurrentCostumeHeight)
{
    Sprite sprite;
    SpriteHandlerMock handler;
    EXPECT_CALL(handler, init);
    sprite.setInterface(&handler);

    EXPECT_CALL(handler, costumeHeight()).WillOnce(Return(24));
    ASSERT_EQ(sprite.currentCostumeHeight(), 24);
}

TEST(SpriteTest, DefaultBoundingRect)
{
    Sprite sprite;
    sprite.setX(65.5);
    sprite.setY(-45.01);
    Rect rect = sprite.boundingRect();
    ASSERT_EQ(rect.left(), 65.5);
    ASSERT_EQ(rect.top(), -45.01);
    ASSERT_EQ(rect.right(), 65.5);
    ASSERT_EQ(rect.bottom(), -45.01);
}

TEST(SpriteTest, DefaultFastBoundingRect)
{
    Sprite sprite;
    sprite.setX(65.5);
    sprite.setY(-45.01);
    Rect rect = sprite.fastBoundingRect();
    ASSERT_EQ(rect.left(), 65.5);
    ASSERT_EQ(rect.top(), -45.01);
    ASSERT_EQ(rect.right(), 65.5);
    ASSERT_EQ(rect.bottom(), -45.01);
}

TEST(SpriteTest, TouchingSprite)
{
    Sprite sprite;
    Sprite another;
    EngineMock engine;
    another.setEngine(&engine);
    ASSERT_FALSE(sprite.touchingSprite(nullptr));

    SpriteHandlerMock iface;
    EXPECT_CALL(iface, init);
    sprite.setInterface(&iface);

    EXPECT_CALL(engine, cloneLimit()).WillRepeatedly(Return(-1));
    EXPECT_CALL(engine, initClone).Times(3);
    EXPECT_CALL(engine, requestRedraw).Times(3);
    EXPECT_CALL(engine, moveDrawableBehindOther).Times(3);
    auto clone1 = another.clone();
    auto clone2 = another.clone();
    auto clone3 = another.clone();
    std::vector<Sprite *> clones = { &another, clone1.get(), clone2.get(), clone3.get() };
    std::vector<Sprite *> actualClones;

    EXPECT_CALL(iface, touchingClones(_)).WillOnce(WithArgs<0>(Invoke([&clones, &actualClones](const std::vector<Sprite *> &candidates) {
        actualClones = candidates;
        return false;
    })));
    ASSERT_FALSE(sprite.touchingSprite(&another));
    ASSERT_EQ(clones, actualClones);

    EXPECT_CALL(iface, touchingClones).WillOnce(Return(true));
    ASSERT_TRUE(sprite.touchingSprite(&another));

    SpriteHandlerMock iface2;
    EXPECT_CALL(iface2, init);
    another.setInterface(&iface2);
    clones.erase(clones.begin()); // the sprite cannot touch itself

    EXPECT_CALL(iface2, touchingClones(_)).WillOnce(WithArgs<0>(Invoke([&clones, &actualClones](const std::vector<Sprite *> &candidates) {
        actualClones = candidates;
        return false;
    })));
    ASSERT_FALSE(another.touchingSprite(&another));
    ASSERT_EQ(clones, actualClones);
    clones.insert(clones.begin(), &another);

    SpriteHandlerMock iface3;
    EXPECT_CALL(iface3, init);
    clone2->setInterface(&iface3);
    clones.erase(clones.begin() + 2); // the clone cannot touch itself, but can touch the sprite (clone root)
    EXPECT_CALL(iface3, touchingClones(_)).WillOnce(WithArgs<0>(Invoke([&clones, &actualClones](const std::vector<Sprite *> &candidates) {
        actualClones = candidates;
        return false;
    })));
    ASSERT_FALSE(clone2->touchingSprite(clone2.get()));
    ASSERT_EQ(clones, actualClones);

    // If the sprite is invisible, it cannot touch anything
    EXPECT_CALL(iface, onVisibleChanged).Times(2);
    sprite.setVisible(false);
    EXPECT_CALL(iface, touchingClones).Times(0);
    ASSERT_FALSE(sprite.touchingSprite(&another));

    sprite.setVisible(true);
    EXPECT_CALL(iface3, onVisibleChanged);
    clone2->setVisible(false);
    EXPECT_CALL(iface, touchingClones).Times(0);
    ASSERT_FALSE(clone2->touchingSprite(&another));
}

TEST(SpriteTest, TouchingPoint)
{
    Sprite sprite;
    ASSERT_FALSE(sprite.touchingPoint(0, 0));

    SpriteHandlerMock iface;
    EXPECT_CALL(iface, init);
    sprite.setInterface(&iface);

    EXPECT_CALL(iface, touchingPoint(51.4, -74.05)).WillOnce(Return(false));
    ASSERT_FALSE(sprite.touchingPoint(51.4, -74.05));

    EXPECT_CALL(iface, touchingPoint(-12.46, 120.72)).WillOnce(Return(true));
    ASSERT_TRUE(sprite.touchingPoint(-12.46, 120.72));
}

TEST(SpriteTest, TouchingColor)
{
    Sprite sprite;
    ASSERT_FALSE(sprite.touchingColor(0));
    ASSERT_FALSE(sprite.touchingColor(0, 0));

    SpriteHandlerMock iface;
    EXPECT_CALL(iface, init);
    sprite.setInterface(&iface);

    Rgb c1 = Value(4278228630).toRgba();
    Rgb c2 = Value("#FF00FA").toRgba();
    EXPECT_CALL(iface, touchingColor(c1)).WillOnce(Return(false));
    ASSERT_FALSE(sprite.touchingColor(c1));

    EXPECT_CALL(iface, touchingColor(c2)).WillOnce(Return(true));
    ASSERT_TRUE(sprite.touchingColor(c2));

    EXPECT_CALL(iface, touchingColor(c1, c2)).WillOnce(Return(false));
    ASSERT_FALSE(sprite.touchingColor(c1, c2));

    EXPECT_CALL(iface, touchingColor(c2, c1)).WillOnce(Return(true));
    ASSERT_TRUE(sprite.touchingColor(c2, c1));
}

TEST(SpriteTest, GraphicsEffects)
{
    Sprite sprite;

    EngineMock engine;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, requestRedraw()).Times(4);

    GraphicsEffectMock effect1, effect2;
    EXPECT_CALL(effect1, clamp(48.21)).WillOnce(Return(48.21));
    sprite.setGraphicsEffectValue(&effect1, 48.21);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect1), 48.21);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect2), 0);

    EXPECT_CALL(effect2, clamp(-107.08)).WillOnce(Return(-107.08));
    sprite.setGraphicsEffectValue(&effect2, -107.08);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect1), 48.21);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect2), -107.08);

    EXPECT_CALL(effect1, clamp(300)).WillOnce(Return(101.5));
    sprite.setGraphicsEffectValue(&effect1, 300);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect1), 101.5);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect2), -107.08);

    sprite.clearGraphicsEffects();
    ASSERT_EQ(sprite.graphicsEffectValue(&effect1), 0);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect2), 0);
}

TEST(SpriteTest, BubbleTypeRedraw)
{
    Sprite sprite;
    EngineMock engine;
    sprite.setEngine(&engine);

    EXPECT_CALL(engine, requestRedraw).Times(0);
    sprite.bubble()->setType(TextBubble::Type::Say);
    sprite.bubble()->setType(TextBubble::Type::Think);
}

TEST(SpriteTest, BubbleTextRedraw)
{
    Sprite sprite;
    EngineMock engine;
    sprite.setEngine(&engine);

    EXPECT_CALL(engine, requestRedraw());
    sprite.bubble()->setText("hello");

    sprite.setVisible(false);
    EXPECT_CALL(engine, requestRedraw).Times(0);
    sprite.bubble()->setText("world");

    EXPECT_CALL(engine, requestRedraw).Times(0);
    sprite.bubble()->setText("");

    EXPECT_CALL(engine, requestRedraw());
    sprite.setVisible(true);
    EXPECT_CALL(engine, requestRedraw).Times(0);
    sprite.bubble()->setText("");
}
