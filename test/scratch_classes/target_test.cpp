#include <scratchcpp/target.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/block.h>
#include <scratchcpp/comment.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/sound.h>
#include <scratch/sound_p.h>
#include <enginemock.h>
#include <targetmock.h>
#include <graphicseffectmock.h>
#include <audiooutputmock.h>
#include <audioplayermock.h>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;

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

    target.setName("_mouse_");
    ASSERT_EQ(target.name(), "Test");

    target.setName("_stage_");
    ASSERT_EQ(target.name(), "Test");

    target.setName("_edge_");
    ASSERT_EQ(target.name(), "Test");

    target.setName("_myself_");
    ASSERT_EQ(target.name(), "Test");

    target.setName("_random_");
    ASSERT_EQ(target.name(), "Test");

    target.setName("Sprite1");
    ASSERT_EQ(target.name(), "Sprite1");
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

    ASSERT_EQ(v1->target(), &target);
    ASSERT_EQ(v2->target(), &target);
    ASSERT_EQ(v3->target(), &target);

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

    ASSERT_EQ(l1->target(), &target);
    ASSERT_EQ(l2->target(), &target);
    ASSERT_EQ(l3->target(), &target);

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

    TargetMock target;
    EXPECT_CALL(target, dataSource()).Times(18).WillRepeatedly(Return(nullptr));

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

    // Test with custom data source
    Target source;

    EXPECT_CALL(target, dataSource()).WillOnce(Return(&source));

    ASSERT_TRUE(target.blocks().empty());

    TargetMock target2;
    EXPECT_CALL(target2, dataSource()).Times(18).WillRepeatedly(Return(&source));

    ASSERT_EQ(target2.addBlock(b1), 0);
    ASSERT_EQ(target2.addBlock(b2), 1);
    ASSERT_EQ(target2.addBlock(b3), 2);
    ASSERT_EQ(target2.addBlock(b4), 3);
    ASSERT_EQ(target2.addBlock(b2), 1); // add existing block

    ASSERT_EQ(target2.blockAt(0), b1);
    ASSERT_EQ(target2.blockAt(1), b2);
    ASSERT_EQ(target2.blockAt(2), b3);
    ASSERT_EQ(target2.blockAt(3), b4);
    ASSERT_EQ(target2.blockAt(4), nullptr);
    ASSERT_EQ(target2.blockAt(-1), nullptr);

    ASSERT_EQ(target2.findBlock("e"), -1);
    ASSERT_EQ(target2.findBlock("a"), 0);
    ASSERT_EQ(target2.findBlock("b"), 1);
    ASSERT_EQ(target2.findBlock("c"), 2);
    ASSERT_EQ(target2.findBlock("d"), 3);

    ASSERT_EQ(target2.greenFlagBlocks(), std::vector<std::shared_ptr<Block>>({ b1, b4 }));

    ASSERT_EQ(target2.blocks(), source.blocks());

    auto b5 = std::make_shared<Block>("e", "data_setvariableto");
    ASSERT_EQ(source.addBlock(b5), 4);

    EXPECT_CALL(target2, dataSource()).WillOnce(Return(&source));
    ASSERT_EQ(target2.blocks(), source.blocks());

    ASSERT_EQ(source.blockAt(0), b1);
    ASSERT_EQ(source.blockAt(1), b2);
    ASSERT_EQ(source.blockAt(2), b3);
    ASSERT_EQ(source.blockAt(3), b4);
    ASSERT_EQ(source.blockAt(4), b5);
    ASSERT_EQ(source.blockAt(5), nullptr);
    ASSERT_EQ(source.blockAt(-1), nullptr);

    ASSERT_EQ(source.findBlock("f"), -1);
    ASSERT_EQ(source.findBlock("a"), 0);
    ASSERT_EQ(source.findBlock("b"), 1);
    ASSERT_EQ(source.findBlock("c"), 2);
    ASSERT_EQ(source.findBlock("d"), 3);
    ASSERT_EQ(source.findBlock("e"), 4);

    ASSERT_EQ(source.greenFlagBlocks(), std::vector<std::shared_ptr<Block>>({ b1, b4 }));
}

TEST(TargetTest, Comments)
{
    auto c1 = std::make_shared<Comment>("a");
    auto c2 = std::make_shared<Comment>("b");
    auto c3 = std::make_shared<Comment>("c");
    auto c4 = std::make_shared<Comment>("d");

    TargetMock target;
    EXPECT_CALL(target, dataSource()).Times(17).WillRepeatedly(Return(nullptr));

    ASSERT_EQ(target.addComment(c1), 0);
    ASSERT_EQ(target.addComment(c2), 1);
    ASSERT_EQ(target.addComment(c3), 2);
    ASSERT_EQ(target.addComment(c4), 3);
    ASSERT_EQ(target.addComment(c2), 1); // add existing block

    ASSERT_EQ(target.comments(), std::vector<std::shared_ptr<Comment>>({ c1, c2, c3, c4 }));
    ASSERT_EQ(target.commentAt(0), c1);
    ASSERT_EQ(target.commentAt(1), c2);
    ASSERT_EQ(target.commentAt(2), c3);
    ASSERT_EQ(target.commentAt(3), c4);
    ASSERT_EQ(target.commentAt(4), nullptr);
    ASSERT_EQ(target.commentAt(-1), nullptr);

    ASSERT_EQ(target.findComment("e"), -1);
    ASSERT_EQ(target.findComment("a"), 0);
    ASSERT_EQ(target.findComment("b"), 1);
    ASSERT_EQ(target.findComment("c"), 2);
    ASSERT_EQ(target.findComment("d"), 3);

    // Test with custom data source
    Target source;

    EXPECT_CALL(target, dataSource()).WillOnce(Return(&source));

    ASSERT_TRUE(target.comments().empty());

    TargetMock target2;
    EXPECT_CALL(target2, dataSource()).Times(17).WillRepeatedly(Return(&source));

    ASSERT_EQ(target2.addComment(c1), 0);
    ASSERT_EQ(target2.addComment(c2), 1);
    ASSERT_EQ(target2.addComment(c3), 2);
    ASSERT_EQ(target2.addComment(c4), 3);
    ASSERT_EQ(target2.addComment(c2), 1); // add existing block

    ASSERT_EQ(target2.commentAt(0), c1);
    ASSERT_EQ(target2.commentAt(1), c2);
    ASSERT_EQ(target2.commentAt(2), c3);
    ASSERT_EQ(target2.commentAt(3), c4);
    ASSERT_EQ(target2.commentAt(4), nullptr);
    ASSERT_EQ(target2.commentAt(-1), nullptr);

    ASSERT_EQ(target2.findComment("e"), -1);
    ASSERT_EQ(target2.findComment("a"), 0);
    ASSERT_EQ(target2.findComment("b"), 1);
    ASSERT_EQ(target2.findComment("c"), 2);
    ASSERT_EQ(target2.findComment("d"), 3);

    ASSERT_EQ(target2.comments(), source.comments());

    auto c5 = std::make_shared<Comment>("e");
    ASSERT_EQ(source.addComment(c5), 4);

    EXPECT_CALL(target2, dataSource()).WillOnce(Return(&source));
    ASSERT_EQ(target2.comments(), source.comments());

    ASSERT_EQ(source.commentAt(0), c1);
    ASSERT_EQ(source.commentAt(1), c2);
    ASSERT_EQ(source.commentAt(2), c3);
    ASSERT_EQ(source.commentAt(3), c4);
    ASSERT_EQ(source.commentAt(4), c5);
    ASSERT_EQ(source.commentAt(5), nullptr);
    ASSERT_EQ(source.commentAt(-1), nullptr);

    ASSERT_EQ(source.findComment("f"), -1);
    ASSERT_EQ(source.findComment("a"), 0);
    ASSERT_EQ(source.findComment("b"), 1);
    ASSERT_EQ(source.findComment("c"), 2);
    ASSERT_EQ(source.findComment("d"), 3);
    ASSERT_EQ(source.findComment("e"), 4);
}

TEST(TargetTest, CostumeIndex)
{
    Target target;
    ASSERT_EQ(target.costumeIndex(), -1);
    ASSERT_EQ(target.currentCostume(), nullptr);

    target.setCostumeIndex(0);
    ASSERT_EQ(target.costumeIndex(), -1);
    ASSERT_EQ(target.currentCostume(), nullptr);

    target.setCostumeIndex(1);
    ASSERT_EQ(target.costumeIndex(), -1);
    ASSERT_EQ(target.currentCostume(), nullptr);

    target.addCostume(std::make_shared<Costume>("", "", ""));
    ASSERT_EQ(target.costumeIndex(), -1);
    ASSERT_EQ(target.currentCostume(), nullptr);

    target.setCostumeIndex(0);
    ASSERT_EQ(target.costumeIndex(), 0);
    ASSERT_EQ(target.currentCostume(), target.costumeAt(0));

    target.setCostumeIndex(1);
    ASSERT_EQ(target.costumeIndex(), 0);
    ASSERT_EQ(target.currentCostume(), target.costumeAt(0));

    target.addCostume(std::make_shared<Costume>("", "", ""));
    ASSERT_EQ(target.costumeIndex(), 0);
    ASSERT_EQ(target.currentCostume(), target.costumeAt(0));

    target.setCostumeIndex(1);
    ASSERT_EQ(target.costumeIndex(), 1);
    ASSERT_EQ(target.currentCostume(), target.costumeAt(1));

    target.setCostumeIndex(2);
    ASSERT_EQ(target.costumeIndex(), 1);
    ASSERT_EQ(target.currentCostume(), target.costumeAt(1));
}

TEST(TargetTest, Costumes)
{
    auto c1 = std::make_shared<Costume>("costume1", "", "svg");
    auto c2 = std::make_shared<Costume>("costume2", "", "png");
    auto c3 = std::make_shared<Costume>("costume3", "", "svg");

    TargetMock target;
    EXPECT_CALL(target, dataSource()).Times(14).WillRepeatedly(Return(nullptr));

    ASSERT_EQ(target.addCostume(c1), 0);
    ASSERT_EQ(target.addCostume(c2), 1);
    ASSERT_EQ(target.addCostume(c3), 2);
    ASSERT_EQ(target.addCostume(c2), 1); // add existing costume

    ASSERT_EQ(target.costumes(), std::vector<std::shared_ptr<Costume>>({ c1, c2, c3 }));
    ASSERT_EQ(target.costumeAt(0), c1);
    ASSERT_EQ(target.costumeAt(1), c2);
    ASSERT_EQ(target.costumeAt(2), c3);
    ASSERT_EQ(target.costumeAt(3), nullptr);
    ASSERT_EQ(target.costumeAt(-1), nullptr);

    ASSERT_EQ(target.findCostume("invalid"), -1);
    ASSERT_EQ(target.findCostume("costume1"), 0);
    ASSERT_EQ(target.findCostume("costume2"), 1);
    ASSERT_EQ(target.findCostume("costume3"), 2);

    // Test with custom data source
    Target source;

    EXPECT_CALL(target, dataSource()).WillOnce(Return(&source));

    ASSERT_TRUE(target.costumes().empty());

    TargetMock target2;
    EXPECT_CALL(target2, dataSource()).Times(15).WillRepeatedly(Return(&source));

    ASSERT_EQ(target2.addCostume(c1), 0);
    ASSERT_EQ(target2.addCostume(c2), 1);
    ASSERT_EQ(target2.addCostume(c3), 2);
    ASSERT_EQ(target2.addCostume(c2), 1); // add existing costume

    ASSERT_EQ(target2.costumes(), std::vector<std::shared_ptr<Costume>>({ c1, c2, c3 }));
    ASSERT_EQ(target2.costumeAt(0), c1);
    ASSERT_EQ(target2.costumeAt(1), c2);
    ASSERT_EQ(target2.costumeAt(2), c3);
    ASSERT_EQ(target2.costumeAt(3), nullptr);
    ASSERT_EQ(target2.costumeAt(-1), nullptr);

    ASSERT_EQ(target2.findCostume("invalid"), -1);
    ASSERT_EQ(target2.findCostume("costume1"), 0);
    ASSERT_EQ(target2.findCostume("costume2"), 1);
    ASSERT_EQ(target2.findCostume("costume3"), 2);

    ASSERT_EQ(target2.costumes(), source.costumes());

    auto c4 = std::make_shared<Costume>("costume4", "", "png");
    ASSERT_EQ(source.addCostume(c4), 3);

    EXPECT_CALL(target2, dataSource()).WillOnce(Return(&source));
    ASSERT_EQ(target2.costumes(), source.costumes());

    ASSERT_EQ(source.costumes(), std::vector<std::shared_ptr<Costume>>({ c1, c2, c3, c4 }));
    ASSERT_EQ(source.costumeAt(0), c1);
    ASSERT_EQ(source.costumeAt(1), c2);
    ASSERT_EQ(source.costumeAt(2), c3);
    ASSERT_EQ(source.costumeAt(3), c4);
    ASSERT_EQ(source.costumeAt(4), nullptr);
    ASSERT_EQ(source.costumeAt(-1), nullptr);

    ASSERT_EQ(source.findCostume("invalid"), -1);
    ASSERT_EQ(source.findCostume("costume1"), 0);
    ASSERT_EQ(source.findCostume("costume2"), 1);
    ASSERT_EQ(source.findCostume("costume3"), 2);
    ASSERT_EQ(source.findCostume("costume4"), 3);
}

TEST(TargetTest, Sounds)
{
    AudioOutputMock factory;
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();
    SoundPrivate::audioOutput = &factory;
    EXPECT_CALL(factory, createAudioPlayer()).WillOnce(Return(player1)).WillOnce(Return(player2)).WillOnce(Return(player3));
    auto s1 = std::make_shared<Sound>("sound1", "", "mp3");
    auto s2 = std::make_shared<Sound>("sound2", "", "wav");
    auto s3 = std::make_shared<Sound>("sound3", "", "mp3");

    TargetMock target;
    EXPECT_CALL(target, dataSource()).Times(14).WillRepeatedly(Return(nullptr));

    EXPECT_CALL(*player1, setVolume(1));
    EXPECT_CALL(*player2, setVolume(1));
    EXPECT_CALL(*player3, setVolume(1));
    ASSERT_EQ(target.addSound(s1), 0);
    ASSERT_EQ(target.addSound(s2), 1);
    ASSERT_EQ(target.addSound(s3), 2);
    ASSERT_EQ(target.addSound(s2), 1); // add existing Sound

    ASSERT_EQ(target.sounds(), std::vector<std::shared_ptr<Sound>>({ s1, s2, s3 }));
    ASSERT_EQ(target.soundAt(0), s1);
    ASSERT_EQ(target.soundAt(1), s2);
    ASSERT_EQ(target.soundAt(2), s3);
    ASSERT_EQ(target.soundAt(3), nullptr);
    ASSERT_EQ(target.soundAt(-1), nullptr);

    ASSERT_EQ(target.findSound("invalid"), -1);
    ASSERT_EQ(target.findSound("sound1"), 0);
    ASSERT_EQ(target.findSound("sound2"), 1);
    ASSERT_EQ(target.findSound("sound3"), 2);

    // Test with custom data source
    Target source;

    EXPECT_CALL(target, dataSource()).WillOnce(Return(&source));

    ASSERT_TRUE(target.sounds().empty());

    TargetMock target2;
    EXPECT_CALL(target2, dataSource()).Times(15).WillRepeatedly(Return(&source));

    EXPECT_CALL(*player1, setVolume(1));
    EXPECT_CALL(*player2, setVolume(1));
    EXPECT_CALL(*player3, setVolume(1));
    ASSERT_EQ(target2.addSound(s1), 0);
    ASSERT_EQ(target2.addSound(s2), 1);
    ASSERT_EQ(target2.addSound(s3), 2);
    ASSERT_EQ(target2.addSound(s2), 1); // add existing Sound

    ASSERT_EQ(target2.sounds(), std::vector<std::shared_ptr<Sound>>({ s1, s2, s3 }));
    ASSERT_EQ(target2.soundAt(0), s1);
    ASSERT_EQ(target2.soundAt(1), s2);
    ASSERT_EQ(target2.soundAt(2), s3);
    ASSERT_EQ(target2.soundAt(3), nullptr);
    ASSERT_EQ(target2.soundAt(-1), nullptr);

    ASSERT_EQ(target2.findSound("invalid"), -1);
    ASSERT_EQ(target2.findSound("sound1"), 0);
    ASSERT_EQ(target2.findSound("sound2"), 1);
    ASSERT_EQ(target2.findSound("sound3"), 2);

    ASSERT_EQ(target2.sounds(), source.sounds());

    auto player4 = std::make_shared<AudioPlayerMock>();
    EXPECT_CALL(factory, createAudioPlayer()).WillOnce(Return(player4));
    auto s4 = std::make_shared<Sound>("sound4", "", "wav");

    EXPECT_CALL(*player4, setVolume(1));
    ASSERT_EQ(source.addSound(s4), 3);

    EXPECT_CALL(target2, dataSource()).WillOnce(Return(&source));
    ASSERT_EQ(target2.sounds(), source.sounds());

    ASSERT_EQ(source.sounds(), std::vector<std::shared_ptr<Sound>>({ s1, s2, s3, s4 }));
    ASSERT_EQ(source.soundAt(0), s1);
    ASSERT_EQ(source.soundAt(1), s2);
    ASSERT_EQ(source.soundAt(2), s3);
    ASSERT_EQ(source.soundAt(3), s4);
    ASSERT_EQ(source.soundAt(4), nullptr);
    ASSERT_EQ(source.soundAt(-1), nullptr);

    ASSERT_EQ(source.findSound("invalid"), -1);
    ASSERT_EQ(source.findSound("sound1"), 0);
    ASSERT_EQ(source.findSound("sound2"), 1);
    ASSERT_EQ(source.findSound("sound3"), 2);
    ASSERT_EQ(source.findSound("sound4"), 3);

    SoundPrivate::audioOutput = nullptr;
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

    target.setVolume(52.08);
    ASSERT_EQ(target.volume(), 52.08);

    target.setVolume(101.8);
    ASSERT_EQ(target.volume(), 100);

    target.setVolume(-4.2);
    ASSERT_EQ(target.volume(), 0);

    target.setVolume(64.9);
    ASSERT_EQ(target.volume(), 64.9);

    AudioOutputMock factory;
    auto player1 = std::make_shared<AudioPlayerMock>();
    auto player2 = std::make_shared<AudioPlayerMock>();
    auto player3 = std::make_shared<AudioPlayerMock>();
    SoundPrivate::audioOutput = &factory;
    EXPECT_CALL(factory, createAudioPlayer()).WillOnce(Return(player1)).WillOnce(Return(player2)).WillOnce(Return(player3));
    auto s1 = std::make_shared<Sound>("", "", "");
    auto s2 = std::make_shared<Sound>("", "", "");
    auto s3 = std::make_shared<Sound>("", "", "");

    EXPECT_CALL(*player1, setVolume(0.649));
    target.addSound(s1);

    EXPECT_CALL(*player2, setVolume(0.649));
    target.addSound(s2);

    EXPECT_CALL(*player3, setVolume(0.649));
    target.addSound(s3);

    EXPECT_CALL(*player1, setVolume(0.7692));
    EXPECT_CALL(*player2, setVolume(0.7692));
    EXPECT_CALL(*player3, setVolume(0.7692));
    target.setVolume(76.92);

    EXPECT_CALL(*player1, setVolume(1));
    EXPECT_CALL(*player2, setVolume(1));
    EXPECT_CALL(*player3, setVolume(1));
    target.setVolume(102);

    EXPECT_CALL(*player1, setVolume(0));
    EXPECT_CALL(*player2, setVolume(0));
    EXPECT_CALL(*player3, setVolume(0));
    target.setVolume(-0.5);

    SoundPrivate::audioOutput = nullptr;
}

TEST(TargetTest, GraphicsEffects)
{
    Target target;

    EngineMock engine;
    target.setEngine(&engine);

    GraphicsEffectMock effect1, effect2;
    target.setGraphicsEffectValue(&effect1, 48.21);
    ASSERT_EQ(target.graphicsEffectValue(&effect1), 48.21);
    ASSERT_EQ(target.graphicsEffectValue(&effect2), 0);

    target.setGraphicsEffectValue(&effect2, -107.08);
    ASSERT_EQ(target.graphicsEffectValue(&effect1), 48.21);
    ASSERT_EQ(target.graphicsEffectValue(&effect2), -107.08);

    target.clearGraphicsEffects();
    ASSERT_EQ(target.graphicsEffectValue(&effect1), 0);
    ASSERT_EQ(target.graphicsEffectValue(&effect2), 0);
}

TEST(TargetTest, BubbleType)
{
    Target target;
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);

    target.setBubbleType(Target::BubbleType::Think);
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Think);

    target.setBubbleType(Target::BubbleType::Say);
    ASSERT_EQ(target.bubbleType(), Target::BubbleType::Say);
}

TEST(TargetTest, BubbleText)
{
    Target target;
    ASSERT_TRUE(target.bubbleText().empty());

    target.setBubbleText("hello");
    ASSERT_EQ(target.bubbleText(), "hello");

    target.setBubbleText("world");
    ASSERT_EQ(target.bubbleText(), "world");

    // longstr.length = 384, should be limited to 330 in bubble text
    std::string longstr =
        "EY8OUNzAqwgh7NRGk5TzCP3dkAhJy9TX"
        "Y9mqKElPjdQpKddYqjyCwUk2hx6YgVZV"
        "6BOdmZGxDMs8Hjv8W9G6j4gTxAWdOkzs"
        "8Ih80xzEDbvLilWsDwoB6FxH2kVVI4xs"
        "IXOETNQ6QMsCKLWc5XjHk2BS9nYvDGpJ"
        "uEmp9zIzFGT1kRSrOlU3ZwnN1YtvqFx"
        "3hkWVNtJ71dQ0PJHhOVQPUy19V01SPu3"
        "KIIS2wdSUVAc4RYMzepSveghzWbdcizy"
        "Tm1KKAj4svu9YoL8b9vsolG8gKunvKO7"
        "MurRKSeUbECELnJEKV6683xCq7RvmjAu"
        "2djZ54apiQc1lTixWns5GoG0SVNuFzHl"
        "q97qUiqiMecjVFM51YVif7c1Stip52Hl";

    target.setBubbleText(longstr);
    ASSERT_EQ(target.bubbleText().length(), 330);
    ASSERT_EQ(target.bubbleText(), longstr.substr(0, 330));
}

TEST(TargetTest, Engine)
{
    Target target;
    ASSERT_EQ(target.engine(), nullptr);

    EngineMock engine;
    target.setEngine(&engine);
    ASSERT_EQ(target.engine(), &engine);
}

TEST(TargetTest, DataSource)
{
    TargetMock target;
    ASSERT_EQ(target.dataSourceBase(), nullptr);
}
