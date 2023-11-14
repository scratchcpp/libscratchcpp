#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/scratchconfiguration.h>
#include <scratchcpp/rect.h>
#include <scratchcpp/project.h>
#include <enginemock.h>
#include <imageformatfactorymock.h>
#include <imageformatmock.h>
#include <graphicseffectmock.h>

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
    std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
    sprite->setName("Sprite1");
    auto var1 = std::make_shared<Variable>("a", "var1", "hello");
    auto var2 = std::make_shared<Variable>("b", "var2", "world");
    sprite->addVariable(var1);
    sprite->addVariable(var2);
    auto c1 = std::make_shared<Costume>("costume1", "", "svg");
    sprite->addCostume(c1);

    auto list1 = std::make_shared<List>("c", "list1");
    list1->push_back("item1");
    list1->push_back("item2");
    auto list2 = std::make_shared<List>("d", "list2");
    list2->push_back("test");
    sprite->addList(list1);
    sprite->addList(list2);

    sprite->addCostume(std::make_shared<Costume>("", "", ""));
    sprite->addCostume(std::make_shared<Costume>("", "", ""));
    sprite->setCostumeIndex(1);
    sprite->setLayerOrder(5);
    sprite->setVolume(50);

    sprite->setVisible(false);
    sprite->setX(100.25);
    sprite->setY(-45.43);
    sprite->setSize(54.121);
    sprite->setDirection(179.4);
    sprite->setDraggable(true);
    sprite->setRotationStyle(Sprite::RotationStyle::DoNotRotate);

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

    ASSERT_FALSE(sprite->isClone());
    ASSERT_EQ(sprite->cloneRoot(), nullptr);
    ASSERT_EQ(sprite->cloneParent(), nullptr);

    ASSERT_EQ(sprite->clone(), nullptr);
    ASSERT_FALSE(sprite->isClone());
    ASSERT_EQ(sprite->cloneRoot(), nullptr);
    ASSERT_EQ(sprite->cloneParent(), nullptr);

    EngineMock engine;
    sprite->setEngine(&engine);
    EXPECT_CALL(engine, requestRedraw()).Times(2);
    EXPECT_CALL(engine, cloneLimit()).Times(6).WillRepeatedly(Return(300)); // clone count limit is tested later

    Sprite *clone1;
    EXPECT_CALL(engine, cloneCount()).WillOnce(Return(0));
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone1));
    ASSERT_EQ(sprite->clone().get(), clone1);
    ASSERT_FALSE(sprite->isClone());
    ASSERT_EQ(sprite->cloneRoot(), nullptr);
    ASSERT_EQ(sprite->cloneParent(), nullptr);

    ASSERT_TRUE(clone1->isClone());
    ASSERT_EQ(clone1->cloneRoot(), sprite.get());
    ASSERT_EQ(clone1->cloneParent(), sprite.get());

    checkCloneData(clone1);

    // Modify root sprite data to make sure parent is used
    sprite->setLayerOrder(3);

    Sprite *clone2;
    EXPECT_CALL(engine, cloneCount()).WillOnce(Return(1));
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone2));
    ASSERT_EQ(clone1->clone().get(), clone2);
    ASSERT_TRUE(clone1->isClone());
    ASSERT_EQ(clone1->cloneRoot(), sprite.get());
    ASSERT_EQ(clone1->cloneParent(), sprite.get());
    ASSERT_TRUE(clone2->isClone());
    ASSERT_EQ(clone2->cloneRoot(), sprite.get());
    ASSERT_EQ(clone2->cloneParent(), clone1);

    checkCloneData(clone2);

    sprite->setVisible(true);

    Sprite *clone3;
    EXPECT_CALL(engine, cloneCount()).WillOnce(Return(2));
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone3));
    ASSERT_EQ(clone1->clone().get(), clone3);

    Sprite *clone4;
    EXPECT_CALL(engine, cloneLimit()).WillOnce(Return(-1));
    EXPECT_CALL(engine, initClone(_)).WillOnce(SaveArg<0>(&clone4));
    ASSERT_EQ(sprite->clone().get(), clone4);

    EXPECT_CALL(engine, cloneLimit()).Times(2).WillRepeatedly(Return(0));
    EXPECT_CALL(engine, cloneCount()).WillOnce(Return(0));
    ASSERT_EQ(sprite->clone(), nullptr);

    EXPECT_CALL(engine, cloneLimit()).Times(2).WillRepeatedly(Return(150));
    EXPECT_CALL(engine, cloneCount()).WillOnce(Return(150));
    ASSERT_EQ(sprite->clone(), nullptr);

    // children
    const auto &children1 = sprite->children();
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
    auto allChildren = sprite->allChildren();
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

    ASSERT_EQ(clone2->costumes(), sprite->costumes());
    auto c2 = std::make_shared<Costume>("costume2", "", "png");
    clone2->addCostume(c2);
    ASSERT_EQ(clone2->costumes(), sprite->costumes());

    EXPECT_CALL(engine, deinitClone(clone2));
    clone2->~Sprite();

    EXPECT_CALL(engine, deinitClone(clone3));
    clone3->~Sprite();

    EXPECT_CALL(engine, deinitClone(clone1)).Times(2);
    clone1->~Sprite();

    EXPECT_CALL(engine, deinitClone(clone4)).Times(2);
    clone4->~Sprite();

    sprite.reset();
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

    auto imageFormatFactory = std::make_shared<ImageFormatFactoryMock>();
    auto imageFormat = std::make_shared<ImageFormatMock>();

    ScratchConfiguration::registerImageFormat("test", imageFormatFactory);
    EXPECT_CALL(*imageFormatFactory, createInstance()).WillOnce(Return(imageFormat));
    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(0));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(0));
    auto costume = std::make_shared<Costume>("costume1", "a", "test");

    sprite.addCostume(costume);
    sprite.setCostumeIndex(0);

    static char data[5] = "abcd";
    EXPECT_CALL(*imageFormat, setData(5, data));
    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));

    EXPECT_CALL(*imageFormat, colorAt(0, 0, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));
    EXPECT_CALL(*imageFormat, colorAt(1, 0, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));
    EXPECT_CALL(*imageFormat, colorAt(2, 0, 1)).WillOnce(Return(rgba(0, 0, 0, 255)));
    EXPECT_CALL(*imageFormat, colorAt(3, 0, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));

    EXPECT_CALL(*imageFormat, colorAt(0, 1, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));
    EXPECT_CALL(*imageFormat, colorAt(1, 1, 1)).WillOnce(Return(rgba(0, 0, 0, 255)));
    EXPECT_CALL(*imageFormat, colorAt(2, 1, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));
    EXPECT_CALL(*imageFormat, colorAt(3, 1, 1)).WillOnce(Return(rgba(0, 0, 0, 255)));

    EXPECT_CALL(*imageFormat, colorAt(0, 2, 1)).WillOnce(Return(rgba(0, 0, 0, 255)));
    EXPECT_CALL(*imageFormat, colorAt(1, 2, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));
    EXPECT_CALL(*imageFormat, colorAt(2, 2, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));
    EXPECT_CALL(*imageFormat, colorAt(3, 2, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));
    costume->setData(5, data);

    sprite.setEngine(&engine);
    sprite.setDirection(34.45);

    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));
    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    sprite.setX(230);
    ASSERT_EQ(sprite.x(), 230);

    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));
    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    sprite.setX(-230);
    ASSERT_EQ(sprite.x(), -230);

    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));
    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    sprite.setX(250);
    ASSERT_EQ(sprite.x(), 241);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(false));
    sprite.setX(250);
    ASSERT_EQ(sprite.x(), 250);

    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));
    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    sprite.setX(-250);
    ASSERT_EQ(sprite.x(), -241);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(false));
    sprite.setX(-250);
    ASSERT_EQ(sprite.x(), -250);

    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));
    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    sprite.setY(170);
    ASSERT_EQ(sprite.y(), 170);

    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));
    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    sprite.setY(-170);
    ASSERT_EQ(sprite.y(), -170);

    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));
    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    sprite.setY(190);
    ASSERT_EQ(sprite.y(), 181);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(false));
    sprite.setY(190);
    ASSERT_EQ(sprite.y(), 190);

    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));
    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(true));
    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(480));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(360));
    sprite.setY(-190);
    ASSERT_EQ(sprite.y(), -180);

    EXPECT_CALL(engine, spriteFencingEnabled()).WillOnce(Return(false));
    sprite.setY(-190);
    ASSERT_EQ(sprite.y(), -190);

    ScratchConfiguration::removeImageFormat("test");
}

TEST(SpriteTest, Fencing)
{
    Project p("sprite_fencing.sb3");
    ASSERT_TRUE(p.load());
    p.run();

    auto engine = p.engine();

    Stage *stage = engine->stage();
    ASSERT_TRUE(stage);

    ASSERT_VAR(stage, "maxX");
    ASSERT_EQ(GET_VAR(stage, "maxX")->value().toDouble(), 240);

    ASSERT_VAR(stage, "maxY");
    ASSERT_EQ(GET_VAR(stage, "maxY")->value().toDouble(), 205);

    ASSERT_VAR(stage, "minX");
    ASSERT_EQ(GET_VAR(stage, "minX")->value().toDouble(), -362);

    ASSERT_VAR(stage, "minY");
    ASSERT_EQ(GET_VAR(stage, "minY")->value().toDouble(), -213);
}

TEST(SpriteTest, Size)
{
    auto c1 = std::make_shared<Costume>("", "", "");
    auto c2 = std::make_shared<Costume>("", "", "");

    Sprite sprite;
    ASSERT_EQ(sprite.size(), 100);
    sprite.addCostume(c1);
    sprite.addCostume(c2);
    sprite.setCostumeIndex(0);

    ASSERT_EQ(c1->scale(), 1);

    EngineMock engine;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, requestRedraw()).Times(4);

    sprite.setSize(63.724);
    ASSERT_EQ(sprite.size(), 63.724);
    ASSERT_EQ(std::round(c1->scale() * 100000) / 100000, 0.63724);

    sprite.setCostumeIndex(1);
    ASSERT_EQ(std::round(c2->scale() * 100000) / 100000, 0.63724);

    sprite.setSize(186.84);
    ASSERT_EQ(sprite.size(), 186.84);
    ASSERT_EQ(std::round(c2->scale() * 10000) / 10000, 1.8684);

    sprite.setCostumeIndex(0);
    ASSERT_EQ(std::round(c1->scale() * 10000) / 10000, 1.8684);
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
    auto c1 = std::make_shared<Costume>("", "", "");
    auto c2 = std::make_shared<Costume>("", "", "");

    Sprite sprite;
    ASSERT_EQ(sprite.rotationStyle(), Sprite::RotationStyle::AllAround); // default
    ASSERT_EQ(sprite.rotationStyleStr(), "all around");
    sprite.addCostume(c1);
    sprite.addCostume(c2);
    sprite.setCostumeIndex(0);

    ASSERT_EQ(c1->mirrorHorizontally(), false);

    EngineMock engine;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, requestRedraw()).Times(8);

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

    sprite.setCostumeIndex(1);
    sprite.setRotationStyle(Sprite::RotationStyle::LeftRight);
    ASSERT_EQ(c2->mirrorHorizontally(), true);

    sprite.setCostumeIndex(0);
    sprite.setRotationStyle(Sprite::RotationStyle::AllAround);
    sprite.setCostumeIndex(1);
    ASSERT_EQ(c2->mirrorHorizontally(), false);

    sprite.setVisible(false);

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

TEST(SpriteTest, BoundingRect)
{
    auto imageFormatFactory = std::make_shared<ImageFormatFactoryMock>();
    auto imageFormat = std::make_shared<ImageFormatMock>();

    ScratchConfiguration::registerImageFormat("test", imageFormatFactory);
    EXPECT_CALL(*imageFormatFactory, createInstance()).WillOnce(Return(imageFormat));
    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(0));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(0));
    auto costume = std::make_shared<Costume>("costume1", "a", "test");

    Sprite sprite;
    sprite.addCostume(costume);
    sprite.setCostumeIndex(0);

    static char data[5] = "abcd";
    EXPECT_CALL(*imageFormat, setData(5, data));
    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));

    EXPECT_CALL(*imageFormat, colorAt(0, 0, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));
    EXPECT_CALL(*imageFormat, colorAt(1, 0, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));
    EXPECT_CALL(*imageFormat, colorAt(2, 0, 1)).WillOnce(Return(rgba(0, 0, 0, 255)));
    EXPECT_CALL(*imageFormat, colorAt(3, 0, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));

    EXPECT_CALL(*imageFormat, colorAt(0, 1, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));
    EXPECT_CALL(*imageFormat, colorAt(1, 1, 1)).WillOnce(Return(rgba(0, 0, 0, 255)));
    EXPECT_CALL(*imageFormat, colorAt(2, 1, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));
    EXPECT_CALL(*imageFormat, colorAt(3, 1, 1)).WillOnce(Return(rgba(0, 0, 0, 255)));

    EXPECT_CALL(*imageFormat, colorAt(0, 2, 1)).WillOnce(Return(rgba(0, 0, 0, 255)));
    EXPECT_CALL(*imageFormat, colorAt(1, 2, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));
    EXPECT_CALL(*imageFormat, colorAt(2, 2, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));
    EXPECT_CALL(*imageFormat, colorAt(3, 2, 1)).WillOnce(Return(rgba(0, 0, 0, 0)));
    costume->setData(5, data);

    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));
    Rect rect = sprite.boundingRect();
    ASSERT_EQ(rect.left(), -2);
    ASSERT_EQ(rect.top(), 0.5);
    ASSERT_EQ(rect.right(), 1);
    ASSERT_EQ(rect.bottom(), -1.5);

    sprite.setDirection(45);
    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));
    rect = sprite.boundingRect();
    ASSERT_EQ(std::round(rect.left() * 10000) / 10000, -1.7678);
    ASSERT_EQ(std::round(rect.top() * 10000) / 10000, 0.3536);
    ASSERT_EQ(std::round(rect.right() * 10000) / 10000, 1.0607);
    ASSERT_EQ(std::round(rect.bottom() * 10000) / 10000, -1.0607);

    sprite.setDirection(-160);
    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));
    rect = sprite.boundingRect();
    ASSERT_EQ(std::round(rect.left() * 10000) / 10000, -1.4095);
    ASSERT_EQ(std::round(rect.top() * 10000) / 10000, 1.7084);
    ASSERT_EQ(std::round(rect.right() * 10000) / 10000, 1.1539);
    ASSERT_EQ(std::round(rect.bottom() * 10000) / 10000, -0.7687);

    sprite.setX(86.48);
    sprite.setY(-147.16);
    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));
    rect = sprite.boundingRect();
    ASSERT_EQ(std::round(rect.left() * 10000) / 10000, 85.0705);
    ASSERT_EQ(std::round(rect.top() * 10000) / 10000, -145.4516);
    ASSERT_EQ(std::round(rect.right() * 10000) / 10000, 87.6339);
    ASSERT_EQ(std::round(rect.bottom() * 10000) / 10000, -147.9287);

    costume->setRotationCenterX(-4);
    costume->setRotationCenterY(8);
    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));
    rect = sprite.boundingRect();
    ASSERT_EQ(std::round(rect.left() * 10000) / 10000, 76.1848);
    ASSERT_EQ(std::round(rect.top() * 10000) / 10000, -146.4742);
    ASSERT_EQ(std::round(rect.right() * 10000) / 10000, 78.7483);
    ASSERT_EQ(std::round(rect.bottom() * 10000) / 10000, -148.9513);

    sprite.setDirection(90);
    sprite.setX(0);
    sprite.setY(0);
    EXPECT_CALL(*imageFormat, width()).WillOnce(Return(4));
    EXPECT_CALL(*imageFormat, height()).WillOnce(Return(3));
    rect = sprite.boundingRect();
    ASSERT_EQ(rect.left(), 2);
    ASSERT_EQ(rect.top(), -7.5);
    ASSERT_EQ(rect.right(), 5);
    ASSERT_EQ(rect.bottom(), -9.5);

    ScratchConfiguration::removeImageFormat("test");
}

TEST(SpriteTest, GraphicsEffects)
{
    auto c1 = std::make_shared<Costume>("", "", "");
    auto c2 = std::make_shared<Costume>("", "", "");

    Sprite sprite;

    EngineMock engine;
    sprite.setEngine(&engine);
    EXPECT_CALL(engine, requestRedraw()).Times(6);

    sprite.addCostume(c1);
    sprite.addCostume(c2);
    sprite.setCostumeIndex(0);

    GraphicsEffectMock effect1, effect2;
    sprite.setGraphicsEffectValue(&effect1, 48.21);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect1), 48.21);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect2), 0);
    ASSERT_EQ(c1->graphicsEffectValue(&effect1), 48.21);
    ASSERT_EQ(c1->graphicsEffectValue(&effect2), 0);

    sprite.setCostumeIndex(1);
    ASSERT_EQ(c1->graphicsEffectValue(&effect1), 48.21);
    ASSERT_EQ(c1->graphicsEffectValue(&effect2), 0);

    sprite.setGraphicsEffectValue(&effect2, -107.08);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect1), 48.21);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect2), -107.08);
    ASSERT_EQ(c2->graphicsEffectValue(&effect1), 48.21);
    ASSERT_EQ(c2->graphicsEffectValue(&effect2), -107.08);

    sprite.setCostumeIndex(0);
    ASSERT_EQ(c1->graphicsEffectValue(&effect1), 48.21);
    ASSERT_EQ(c1->graphicsEffectValue(&effect2), -107.08);

    sprite.clearGraphicsEffects();
    ASSERT_EQ(sprite.graphicsEffectValue(&effect1), 0);
    ASSERT_EQ(sprite.graphicsEffectValue(&effect2), 0);
    ASSERT_EQ(c1->graphicsEffectValue(&effect1), 0);
    ASSERT_EQ(c1->graphicsEffectValue(&effect2), 0);
}
