#include <scratchcpp/project.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/field.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/block.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/sound.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/inputvalue.h>
#include <scratchcpp/comment.h>
#include <scratchcpp/broadcast.h>
#include <scratchcpp/monitor.h>
#include <projectdownloaderfactorymock.h>
#include <projectdownloadermock.h>

#include "project_p.h"
#include "../common.h"

using namespace libscratchcpp;

static const std::vector<std::string> fileExtensions = { ".sb3" };

using ::testing::Return;
using ::testing::ReturnRef;

TEST(LoadProjectTest, EmptyProject)
{
    int i = 0;
    for (auto ext : fileExtensions) {
        Project p("empty_project" + ext);

        ASSERT_TRUE(p.load());

        auto engine = p.engine();
        ASSERT_EQ(engine->targets().size(), 1);
        ASSERT_EQ(engine->extensions().size(), 0);
        ASSERT_EQ(engine->broadcasts().size(), 0);

        std::shared_ptr<Stage> stage = std::reinterpret_pointer_cast<Stage>(engine->targets().at(0));
        ASSERT_TRUE(stage->isStage());
        ASSERT_EQ(stage->name(), "Stage");
        ASSERT_EQ(stage->variables().size(), 0);
        ASSERT_EQ(stage->lists().size(), 0);
        ASSERT_EQ(stage->blocks().size(), 0);
        ASSERT_EQ(stage->costumes().size(), 1);
        ASSERT_TRUE(stage->comments().empty());
        ASSERT_EQ(stage->costumeIndex(), 0);
        ASSERT_EQ(stage->sounds().size(), 0);
        ASSERT_EQ(stage->layerOrder(), 0);
        ASSERT_EQ(stage->volume(), 100);
        ASSERT_EQ(stage->tempo(), 60);
        ASSERT_EQ(stage->videoState(), Stage::VideoState::On);
        ASSERT_EQ(stage->videoTransparency(), 50);
        ASSERT_TRUE(stage->textToSpeechLanguage().empty());

        auto backdrop = stage->costumeAt(0);
        ASSERT_EQ(backdrop->name(), "backdrop1");
        ASSERT_FALSE(backdrop->id().empty());
        ASSERT_EQ(backdrop->fileName(), backdrop->id() + ".svg");
        ASSERT_EQ(backdrop->dataFormat(), "svg");
        ASSERT_TRUE(backdrop->data());
        ASSERT_EQ(
            memcmp(
                backdrop->data(),
                "<svg version=\"1.1\" width=\"2\" height=\"2\" viewBox=\"-1 -1 2 2\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n  <!-- Exported by Scratch - "
                "http://scratch.mit.edu/ -->\n</svg>",
                backdrop->dataSize()),
            0);

        ASSERT_EQ(engine->userAgent(), "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Scratux/1.4.1 Chrome/76.0.3809.146 Electron/6.1.7 Safari/537.36");

        i++;
    }
}

TEST(LoadProjectTest, DownloadDefaultProject)
{
    ProjectDownloaderFactoryMock factory;
    auto downloader = std::make_shared<ProjectDownloaderMock>();
    ProjectPrivate::downloaderFactory = &factory;

    EXPECT_CALL(factory, create()).WillOnce(Return(downloader));
    ProjectPrivate p;
    p.fileName = "https://scratch.mit.edu/projects/ABCDEFGH"; // NOTE: This is a fake URL
    ProjectPrivate::downloaderFactory = nullptr;

    std::string data = readFileStr("default_project.json");
    static const std::vector<std::string> assets = {
        "cd21514d0531fdffb22204e0ec5ed84a.svg",
        "83a9787d4cb6f3b7632b4ddfebf74367.wav",
        "b7853f557e4426412e64bb3da6531a99.svg",
        "e6ddc55a6ddd9cc9d84fe0b4c21e016f.svg",
        "83c36d806dc92327b9e7049a565c6bff.wav"
    };
    static const std::vector<std::string> assetData = { "a", "b", "c", "d", "e" };

    EXPECT_CALL(*downloader, downloadJson("ABCDEFGH")).WillOnce(Return(true));
    EXPECT_CALL(*downloader, json()).WillOnce(ReturnRef(data));
    EXPECT_CALL(*downloader, downloadAssets(assets)).WillOnce(Return(true));
    EXPECT_CALL(*downloader, assets()).WillOnce(ReturnRef(assetData));
    ASSERT_TRUE(p.load());

    auto engine = p.engine;
    ASSERT_EQ(engine->targets().size(), 2);
    ASSERT_EQ(engine->extensions().size(), 0);
    ASSERT_EQ(engine->broadcasts().size(), 0);

    std::shared_ptr<Stage> stage = std::reinterpret_pointer_cast<Stage>(engine->targets().at(0));
    ASSERT_EQ(stage->name(), "Stage");
    ASSERT_EQ(stage->costumes().size(), 1);
    ASSERT_EQ(stage->sounds().size(), 1);

    auto backdrop = stage->costumeAt(0);
    ASSERT_EQ(backdrop->name(), "backdrop1");
    ASSERT_TRUE(backdrop->data());
    ASSERT_EQ(memcmp(backdrop->data(), "a", backdrop->dataSize()), 0);

    auto sound = stage->soundAt(0);
    ASSERT_EQ(sound->name(), "pop");
    ASSERT_TRUE(sound->data());
    ASSERT_EQ(memcmp(sound->data(), "b", sound->dataSize()), 0);

    std::shared_ptr<Sprite> sprite = std::reinterpret_pointer_cast<Sprite>(engine->targets().at(1));
    ASSERT_EQ(sprite->name(), "Sprite1");
    ASSERT_EQ(sprite->costumes().size(), 2);
    ASSERT_EQ(sprite->sounds().size(), 1);

    auto costume = sprite->costumeAt(1);
    ASSERT_EQ(costume->name(), "costume2");
    ASSERT_TRUE(costume->data());
    ASSERT_EQ(memcmp(costume->data(), "d", costume->dataSize()), 0);

    sound = sprite->soundAt(0);
    ASSERT_EQ(sound->name(), "Meow");
    ASSERT_TRUE(sound->data());
    ASSERT_EQ(memcmp(sound->data(), "e", sound->dataSize()), 0);
}

TEST(LoadProjectTest, DownloadDefaultProjectFailure)
{
    ProjectDownloaderFactoryMock factory;
    auto downloader = std::make_shared<ProjectDownloaderMock>();
    ProjectPrivate::downloaderFactory = &factory;

    EXPECT_CALL(factory, create()).WillOnce(Return(downloader));
    ProjectPrivate p;
    p.fileName = "https://scratch.mit.edu/projects/ABCDEFGH"; // NOTE: This is a fake URL
    ProjectPrivate::downloaderFactory = nullptr;

    std::string data = readFileStr("default_project.json");
    static const std::vector<std::string> assets = {
        "cd21514d0531fdffb22204e0ec5ed84a.svg",
        "83a9787d4cb6f3b7632b4ddfebf74367.wav",
        "b7853f557e4426412e64bb3da6531a99.svg",
        "e6ddc55a6ddd9cc9d84fe0b4c21e016f.svg",
        "83c36d806dc92327b9e7049a565c6bff.wav"
    };
    static const std::vector<std::string> assetData = { "a", "b", "c", "d", "e" };

    EXPECT_CALL(*downloader, downloadJson("ABCDEFGH")).WillOnce(Return(false));
    ASSERT_FALSE(p.load());

    EXPECT_CALL(*downloader, downloadJson("ABCDEFGH")).WillOnce(Return(false));
    ASSERT_FALSE(p.load());

    EXPECT_CALL(*downloader, downloadJson("ABCDEFGH")).WillOnce(Return(true));
    EXPECT_CALL(*downloader, json()).WillOnce(ReturnRef(data));
    EXPECT_CALL(*downloader, downloadAssets(assets)).WillOnce(Return(false));
    ASSERT_FALSE(p.load());

    EXPECT_CALL(*downloader, downloadJson("ABCDEFGH")).WillOnce(Return(true));
    EXPECT_CALL(*downloader, json()).WillOnce(ReturnRef(data));
    EXPECT_CALL(*downloader, downloadAssets(assets)).WillOnce(Return(true));
    EXPECT_CALL(*downloader, assets()).WillOnce(ReturnRef(assetData));
    ASSERT_TRUE(p.load());
}

#ifndef USE_LLVM
TEST(LoadProjectTest, LoadTestProject)
{
    int i = 0;
    for (auto ext : fileExtensions) {
        Project p("load_test" + ext);

        ASSERT_TRUE(p.load());

        auto engine = p.engine();
        ASSERT_EQ(engine->targets().size(), 3);
        ASSERT_EQ(engine->extensions().size(), 0);
        ASSERT_EQ(engine->broadcasts().size(), 1);

        // Stage
        Stage *stage = engine->stage();
        ASSERT_EQ(stage->comments().size(), 1);
        ASSERT_EQ(stage->variables().size(), 4);
        ASSERT_EQ(stage->lists().size(), 1);

        // Stage: comments
        auto comment = stage->commentAt(0);
        ASSERT_EQ(comment->id(), "y");
        ASSERT_EQ(comment->blockId(), "");
        ASSERT_EQ(comment->block(), nullptr);
        ASSERT_EQ(comment->x(), 202.96296296296296);
        ASSERT_EQ(comment->y(), 293.3333333333335);
        ASSERT_EQ(comment->width(), 124.4444580078125);
        ASSERT_EQ(comment->height(), 114.0740966796875);
        ASSERT_EQ(comment->minimized(), false);
        ASSERT_EQ(comment->text(), "TEST");

        // Stage: variables
        ASSERT_VAR(stage, "var1");
        ASSERT_EQ(GET_VAR(stage, "var1")->value().toString(), "Hello, world!");
        ASSERT_VAR(stage, "var3");
        ASSERT_EQ(GET_VAR(stage, "var3")->value().toDouble(), std::pow(10, 50));

        // Stage: lists
        ASSERT_LIST(stage, "list1");
        ASSERT_EQ(GET_LIST(stage, "list1")->size(), 10);
        ASSERT_EQ(GET_LIST(stage, "list1")->toString(), "3148767724");

        // Sprite1
        ASSERT_NE(engine->findTarget("Sprite1"), -1);
        Sprite *sprite1 = dynamic_cast<Sprite *>(engine->targetAt(engine->findTarget("Sprite1")));
        ASSERT_FALSE(sprite1->isStage());
        ASSERT_EQ(sprite1->name(), "Sprite1");
        ASSERT_EQ(sprite1->variables().size(), 1);
        ASSERT_EQ(sprite1->lists().size(), 1);
        ASSERT_EQ(sprite1->blocks().size(), 19);
        ASSERT_EQ(sprite1->costumes().size(), 2);
        ASSERT_EQ(sprite1->comments().size(), 1);
        ASSERT_EQ(sprite1->costumeIndex(), 1);
        ASSERT_EQ(sprite1->sounds().size(), 1);
        ASSERT_TRUE(sprite1->visible());
        ASSERT_EQ(sprite1->x(), 0);
        ASSERT_EQ(sprite1->y(), 0);
        ASSERT_EQ(sprite1->size(), 100);
        ASSERT_EQ(sprite1->direction(), 90);
        ASSERT_FALSE(sprite1->draggable());
        ASSERT_EQ(sprite1->rotationStyleStr(), "all around");
        ASSERT_EQ(sprite1->rotationStyle(), Sprite::RotationStyle::AllAround);

        // Sprite1: comments
        comment = sprite1->commentAt(0);
        ASSERT_EQ(comment->id(), "z");
        ASSERT_EQ(comment->blockId(), "");
        ASSERT_EQ(comment->block(), nullptr);
        ASSERT_EQ(comment->x(), 509.6296296296298);
        ASSERT_EQ(comment->y(), 386.66666666666674);
        ASSERT_EQ(comment->width(), 171.85186767578125);
        ASSERT_EQ(comment->height(), 97.77777099609375);
        ASSERT_EQ(comment->minimized(), false);
        ASSERT_EQ(comment->text(), "Hello, world!");

        // Sprite1: sounds
        auto meowData = readFileStr("Meow.wav");
        auto sound = sprite1->soundAt(0);
        ASSERT_EQ(sound->name(), "Meow");
        ASSERT_FALSE(sound->id().empty());
        ASSERT_EQ(sound->fileName(), sound->id() + ".wav");
        ASSERT_EQ(sound->dataFormat(), "wav");
        ASSERT_TRUE(sound->data());
        ASSERT_EQ(memcmp(sound->data(), meowData.c_str(), sound->dataSize()), 0);

        // Sprite1: variables
        ASSERT_VAR(sprite1, "var2");
        ASSERT_EQ(GET_VAR(sprite1, "var2")->value().toDouble(), 0.5);

        // Sprite1: lists
        ASSERT_LIST(sprite1, "list2");
        ASSERT_EQ(GET_LIST(sprite1, "list2")->size(), 10);
        ASSERT_EQ(GET_LIST(sprite1, "list2")->toString(), "9638044297");

        // Sprite1: blocks
        ASSERT_EQ(sprite1->greenFlagBlocks().size(), 1);
        ASSERT_TRUE(sprite1->greenFlagBlocks()[0]);
        ASSERT_FALSE(sprite1->greenFlagBlocks()[0]->isTopLevelReporter());
        ASSERT_EQ(sprite1->greenFlagBlocks()[0]->opcode(), "event_whenflagclicked");
        ASSERT_EQ(sprite1->greenFlagBlocks()[0]->x(), 0);
        ASSERT_EQ(sprite1->greenFlagBlocks()[0]->y(), 0);
        auto block = sprite1->greenFlagBlocks()[0]->next();
        ASSERT_TRUE(block);
        ASSERT_EQ(block->parent(), sprite1->greenFlagBlocks()[0]);
        ASSERT_EQ(block->opcode(), "control_forever");
        ASSERT_FALSE(block->isTopLevelReporter());
        ASSERT_INPUT(block, "SUBSTACK");
        block = GET_INPUT(block, "SUBSTACK")->valueBlock();
        ASSERT_TRUE(block);

        ASSERT_EQ(block->opcode(), "motion_goto");
        ASSERT_INPUT(block, "TO");
        ASSERT_FALSE(block->isTopLevelReporter());
        auto inputBlock = GET_INPUT(block, "TO")->valueBlock();
        ASSERT_TRUE(inputBlock);
        ASSERT_EQ(inputBlock->opcode(), "motion_goto_menu");
        ASSERT_FIELD(inputBlock, "TO");
        ASSERT_EQ(GET_FIELD(inputBlock, "TO")->value().toString(), "_random_");
        block = block->next();
        ASSERT_TRUE(block);

        ASSERT_EQ(block->opcode(), "data_setvariableto");
        ASSERT_FIELD(block, "VARIABLE");
        ASSERT_EQ(GET_FIELD(block, "VARIABLE")->valuePtr(), GET_VAR(stage, "var1"));
        ASSERT_INPUT(block, "VALUE");
        ASSERT_EQ(GET_INPUT(block, "VALUE")->primaryValue()->value().toInt(), 0);
        ASSERT_TRUE(block->commentId().empty());
        ASSERT_EQ(block->comment(), nullptr);
        block = block->next();
        ASSERT_TRUE(block);

        ASSERT_INPUT(block, "VALUE");
        inputBlock = GET_INPUT(block, "VALUE")->valueBlock();
        ASSERT_TRUE(inputBlock);
        ASSERT_EQ(inputBlock->opcode(), "operator_random");
        ASSERT_FALSE(inputBlock->isTopLevelReporter());
        ASSERT_INPUT(inputBlock, "FROM");
        ASSERT_EQ(GET_INPUT(inputBlock, "FROM")->primaryValue()->value().toInt(), 1);
        ASSERT_INPUT(inputBlock, "TO");
        ASSERT_EQ(GET_INPUT(inputBlock, "TO")->primaryValue()->value().toInt(), 10);
        block = block->next();
        ASSERT_TRUE(block);

        ASSERT_EQ(block->opcode(), "procedures_call");
        auto prototype = block->mutationPrototype();
        ASSERT_TRUE(prototype->warp());
        ASSERT_EQ(prototype->argumentIds().size(), 2);
        ASSERT_EQ(prototype->procCode(), "custom block %s %b");
        ASSERT_INPUT(block, prototype->argumentIds()[0]);
        ASSERT_EQ(GET_INPUT(block, prototype->argumentIds()[0])->primaryValue()->value().toString(), "test");
        ASSERT_INPUT(block, prototype->argumentIds()[1]);
        ASSERT_TRUE(GET_INPUT(block, prototype->argumentIds()[1])->valueBlock());
        ASSERT_TRUE(block->commentId().empty());
        ASSERT_EQ(block->comment(), nullptr);

        ASSERT_FALSE(block->next());

        auto blocks = sprite1->blocks();
        std::shared_ptr<Block> defineBlock = nullptr;
        for (auto b : blocks) {
            if (b->opcode() == "procedures_definition") {
                defineBlock = b;
                break;
            }
        }
        ASSERT_TRUE(defineBlock);
        ASSERT_EQ(defineBlock->x(), 0);
        ASSERT_EQ(defineBlock->y(), 384);
        ASSERT_INPUT(defineBlock, "custom_block");
        auto blockPrototype = GET_INPUT(defineBlock, "custom_block")->valueBlock();
        ASSERT_TRUE(blockPrototype);
        prototype = blockPrototype->mutationPrototype();
        ASSERT_TRUE(prototype->warp());
        ASSERT_EQ(prototype->argumentIds().size(), 2);
        ASSERT_EQ(prototype->argumentNames(), std::vector<std::string>({ "num or text", "bool" }));
        ASSERT_EQ(prototype->argumentTypes(), std::vector<BlockPrototype::ArgType>({ BlockPrototype::ArgType::StringNum, BlockPrototype::ArgType::Bool }));
        ASSERT_EQ(prototype->argumentDefaults(), std::vector<Value>({ "", false }));
        ASSERT_EQ(prototype->procCode(), "custom block %s %b");
        ASSERT_INPUT(blockPrototype, prototype->argumentIds()[0]);
        auto argBlock = GET_INPUT(blockPrototype, prototype->argumentIds()[0])->valueBlock();
        ASSERT_TRUE(argBlock);
        ASSERT_EQ(argBlock->opcode(), "argument_reporter_string_number");
        ASSERT_INPUT(blockPrototype, prototype->argumentIds()[1]);
        argBlock = GET_INPUT(blockPrototype, prototype->argumentIds()[1])->valueBlock();
        ASSERT_TRUE(argBlock);
        ASSERT_EQ(argBlock->opcode(), "argument_reporter_boolean");
        block = defineBlock->next();
        ASSERT_TRUE(block);

        ASSERT_EQ(block->opcode(), "data_addtolist");
        ASSERT_FIELD(block, "LIST");
        ASSERT_EQ(GET_FIELD(block, "LIST")->valuePtr(), GET_LIST(stage, "list1"));
        ASSERT_INPUT(block, "ITEM");
        argBlock = GET_INPUT(block, "ITEM")->valueBlock();
        ASSERT_TRUE(argBlock);
        ASSERT_EQ(argBlock->opcode(), "argument_reporter_string_number");

        std::shared_ptr<Block> keyPressedBlock = nullptr;
        for (auto b : blocks) {
            if (b->opcode() == "event_whenkeypressed") {
                keyPressedBlock = b;
                break;
            }
        }
        ASSERT_TRUE(keyPressedBlock);
        ASSERT_EQ(keyPressedBlock->x(), 488);
        ASSERT_EQ(keyPressedBlock->y(), 152);

        // Balloon1
        ASSERT_NE(engine->findTarget("Balloon1"), -1);
        Sprite *sprite2 = dynamic_cast<Sprite *>(engine->targetAt(engine->findTarget("Balloon1")));
        ASSERT_FALSE(sprite2->isStage());
        ASSERT_EQ(sprite2->name(), "Balloon1");
        ASSERT_EQ(sprite2->variables().size(), 1);
        ASSERT_EQ(sprite2->lists().size(), 1);
        ASSERT_EQ(sprite2->blocks().size(), 5);
        ASSERT_EQ(sprite2->costumes().size(), 3);
        ASSERT_EQ(sprite2->comments().size(), 2);
        ASSERT_EQ(sprite2->sounds().size(), 1);
        ASSERT_FALSE(sprite2->visible());
        ASSERT_EQ(std::round(sprite2->x()), 143);
        ASSERT_EQ(std::round(sprite2->y()), 13);
        ASSERT_EQ(sprite2->size(), 75);
        ASSERT_EQ(sprite2->direction(), 45);
        ASSERT_TRUE(sprite2->draggable());
        ASSERT_EQ(sprite2->rotationStyleStr(), "don't rotate");
        ASSERT_EQ(sprite2->rotationStyle(), Sprite::RotationStyle::DoNotRotate);

        // Balloon1: comments
        comment = sprite2->commentAt(0);
        ASSERT_EQ(comment->id(), "A");
        ASSERT_EQ(comment->blockId(), "");
        ASSERT_EQ(comment->block(), nullptr);
        ASSERT_EQ(comment->x(), 459.25925925925924);
        ASSERT_EQ(comment->y(), -45.925925925925924);
        ASSERT_EQ(comment->width(), 200);
        ASSERT_EQ(comment->height(), 200);
        ASSERT_EQ(comment->minimized(), true);
        ASSERT_EQ(comment->text(), "test");

        comment = sprite2->commentAt(1);
        auto commentBlock = sprite2->blockAt(sprite2->findBlock("e"));
        ASSERT_EQ(comment->id(), "w");
        ASSERT_EQ(comment->blockId(), "e");
        ASSERT_EQ(comment->block(), commentBlock);
        ASSERT_EQ(comment->x(), 247.3981475830078);
        ASSERT_EQ(comment->y(), 208);
        ASSERT_EQ(comment->width(), 189.62969970703125);
        ASSERT_EQ(comment->height(), 93.33334350585938);
        ASSERT_EQ(comment->minimized(), false);
        ASSERT_EQ(comment->text(), "...");
        ASSERT_EQ(commentBlock->commentId(), "w");
        ASSERT_EQ(commentBlock->comment(), comment);

        // Balloon1: sounds
        auto popData = readFileStr("Pop.wav");
        sound = sprite2->soundAt(0);
        ASSERT_EQ(sound->name(), "Pop");
        ASSERT_FALSE(sound->id().empty());
        ASSERT_EQ(sound->fileName(), sound->id() + ".wav");
        ASSERT_EQ(sound->dataFormat(), "wav");
        ASSERT_TRUE(sound->data());
        ASSERT_EQ(memcmp(sound->data(), popData.c_str(), sound->dataSize()), 0);

        // Balloon1: variables
        ASSERT_VAR(sprite2, "var2");
        ASSERT_TRUE(GET_VAR(sprite2, "var2")->value().toBool());

        // Balloon1: lists
        ASSERT_LIST(sprite2, "list2");
        ASSERT_EQ(GET_LIST(sprite2, "list2")->size(), 10);
        ASSERT_EQ(GET_LIST(sprite2, "list2")->toString(), "0434156948");

        // Monitors
        const auto &monitors = engine->monitors();
        ASSERT_EQ(monitors.size(), 10);

        // list1
        auto monitor = monitors[0];
        ASSERT_EQ(monitor->id(), "/oB5:^t}UfL_Moa]OVbD");
        ASSERT_EQ(monitor->mode(), Monitor::Mode::List);
        ASSERT_EQ(monitor->opcode(), "data_listcontents");
        block = monitor->block();
        ASSERT_EQ(block->fields().size(), 1);
        auto field = block->fieldAt(0);
        ASSERT_EQ(field->name(), "LIST");
        ASSERT_EQ(field->value(), "list1");
        ASSERT_EQ(field->valuePtr(), GET_LIST(stage, "list1"));
        ASSERT_EQ(GET_LIST(stage, "list1")->monitor(), monitor.get());
        ASSERT_EQ(monitor->sprite(), nullptr);
        ASSERT_EQ(monitor->width(), 167);
        ASSERT_EQ(monitor->height(), 93);
        ASSERT_EQ(monitor->x(), 226);
        ASSERT_EQ(monitor->y(), 34);
        ASSERT_TRUE(monitor->visible());

        // Sprite1: list2
        monitor = monitors[1];
        ASSERT_EQ(monitor->id(), "-!=?$nO,[RI{7!^_:x=?");
        ASSERT_EQ(monitor->mode(), Monitor::Mode::List);
        ASSERT_EQ(monitor->opcode(), "data_listcontents");
        block = monitor->block();
        ASSERT_EQ(block->fields().size(), 1);
        field = block->fieldAt(0);
        ASSERT_EQ(field->name(), "LIST");
        ASSERT_EQ(field->value(), "list2");
        ASSERT_EQ(field->valuePtr(), GET_LIST(engine->targetAt(engine->findTarget("Sprite1")), "list2"));
        ASSERT_EQ(GET_LIST(engine->targetAt(engine->findTarget("Sprite1")), "list2")->monitor(), monitor.get());
        ASSERT_EQ(monitor->sprite(), dynamic_cast<Sprite *>(engine->targetAt(engine->findTarget("Sprite1"))));
        ASSERT_EQ(monitor->width(), 0);
        ASSERT_EQ(monitor->height(), 0);
        ASSERT_EQ(monitor->x(), 106);
        ASSERT_EQ(monitor->y(), 5);
        ASSERT_TRUE(monitor->visible());

        // Balloon1: list2
        monitor = monitors[2];
        ASSERT_EQ(monitor->id(), "$FebBLy/KiSa19@:b-iA");
        ASSERT_EQ(monitor->mode(), Monitor::Mode::List);
        ASSERT_EQ(monitor->opcode(), "data_listcontents");
        block = monitor->block();
        ASSERT_EQ(block->fields().size(), 1);
        field = block->fieldAt(0);
        ASSERT_EQ(field->name(), "LIST");
        ASSERT_EQ(field->value(), "list2");
        ASSERT_EQ(field->valuePtr(), GET_LIST(engine->targetAt(engine->findTarget("Balloon1")), "list2"));
        ASSERT_EQ(GET_LIST(engine->targetAt(engine->findTarget("Balloon1")), "list2")->monitor(), monitor.get());
        ASSERT_EQ(monitor->sprite(), dynamic_cast<Sprite *>(engine->targetAt(engine->findTarget("Balloon1"))));
        ASSERT_EQ(monitor->width(), 0);
        ASSERT_EQ(monitor->height(), 0);
        ASSERT_EQ(monitor->x(), 255);
        ASSERT_EQ(monitor->y(), 5);
        ASSERT_FALSE(monitor->visible());

        // var1
        monitor = monitors[3];
        ASSERT_EQ(monitor->id(), "`jEk@4|i[#Fk?(8x)AV.-my variable");
        ASSERT_EQ(monitor->mode(), Monitor::Mode::Default);
        ASSERT_EQ(monitor->opcode(), "data_variable");
        block = monitor->block();
        ASSERT_EQ(block->fields().size(), 1);
        field = block->fieldAt(0);
        ASSERT_EQ(field->name(), "VARIABLE");
        ASSERT_EQ(field->value(), "var1");
        ASSERT_EQ(field->valuePtr(), GET_VAR(stage, "var1"));
        ASSERT_EQ(GET_VAR(stage, "var1")->monitor(), monitor.get());
        ASSERT_EQ(monitor->sprite(), nullptr);
        ASSERT_EQ(monitor->width(), 0);
        ASSERT_EQ(monitor->height(), 0);
        ASSERT_EQ(monitor->x(), 5);
        ASSERT_EQ(monitor->y(), 212);
        ASSERT_TRUE(monitor->visible());

        // Sprite1: var2
        monitor = monitors[4];
        ASSERT_EQ(monitor->id(), "eN~@jd#CfxXM(wA6qcyt");
        ASSERT_EQ(monitor->mode(), Monitor::Mode::Default);
        ASSERT_EQ(monitor->opcode(), "data_variable");
        block = monitor->block();
        ASSERT_EQ(block->fields().size(), 1);
        field = block->fieldAt(0);
        ASSERT_EQ(field->name(), "VARIABLE");
        ASSERT_EQ(field->value(), "var2");
        ASSERT_EQ(field->valuePtr(), GET_VAR(engine->targetAt(engine->findTarget("Sprite1")), "var2"));
        ASSERT_EQ(GET_VAR(engine->targetAt(engine->findTarget("Sprite1")), "var2")->monitor(), monitor.get());
        ASSERT_EQ(monitor->sprite(), dynamic_cast<Sprite *>(engine->targetAt(engine->findTarget("Sprite1"))));
        ASSERT_EQ(monitor->width(), 0);
        ASSERT_EQ(monitor->height(), 0);
        ASSERT_EQ(monitor->x(), 5);
        ASSERT_EQ(monitor->y(), 212);
        ASSERT_FALSE(monitor->visible());

        // Balloon1: var2
        monitor = monitors[5];
        ASSERT_EQ(monitor->id(), "g2][rYmi~u[wE@o$jckr");
        ASSERT_EQ(monitor->mode(), Monitor::Mode::Large);
        ASSERT_EQ(monitor->opcode(), "data_variable");
        block = monitor->block();
        ASSERT_EQ(block->fields().size(), 1);
        field = block->fieldAt(0);
        ASSERT_EQ(field->name(), "VARIABLE");
        ASSERT_EQ(field->value(), "var2");
        ASSERT_EQ(field->valuePtr(), GET_VAR(engine->targetAt(engine->findTarget("Balloon1")), "var2"));
        ASSERT_EQ(GET_VAR(engine->targetAt(engine->findTarget("Balloon1")), "var2")->monitor(), monitor.get());
        ASSERT_EQ(monitor->sprite(), dynamic_cast<Sprite *>(engine->targetAt(engine->findTarget("Balloon1"))));
        ASSERT_EQ(monitor->width(), 0);
        ASSERT_EQ(monitor->height(), 0);
        ASSERT_EQ(monitor->x(), 146);
        ASSERT_EQ(monitor->y(), 267);
        ASSERT_TRUE(monitor->visible());

        // loudness
        monitor = monitors[6];
        ASSERT_EQ(monitor->id(), "loudness");
        ASSERT_EQ(monitor->mode(), Monitor::Mode::Default);
        ASSERT_EQ(monitor->opcode(), "sensing_loudness");
        block = monitor->block();
        ASSERT_TRUE(block->fields().empty());
        ASSERT_EQ(monitor->sprite(), nullptr);
        ASSERT_EQ(monitor->width(), 0);
        ASSERT_EQ(monitor->height(), 0);
        ASSERT_EQ(monitor->x(), 5);
        ASSERT_EQ(monitor->y(), 239);
        ASSERT_TRUE(monitor->visible());

        // current year
        monitor = monitors[7];
        ASSERT_EQ(monitor->id(), "current_year");
        ASSERT_EQ(monitor->mode(), Monitor::Mode::Default);
        ASSERT_EQ(monitor->opcode(), "sensing_current");
        block = monitor->block();
        ASSERT_EQ(block->fields().size(), 1);
        field = block->fieldAt(0);
        ASSERT_EQ(field->name(), "CURRENTMENU");
        ASSERT_EQ(field->value(), "YEAR");
        ASSERT_EQ(monitor->sprite(), nullptr);
        ASSERT_EQ(monitor->width(), 0);
        ASSERT_EQ(monitor->height(), 0);
        ASSERT_EQ(monitor->x(), 5);
        ASSERT_EQ(monitor->y(), 5);
        ASSERT_TRUE(monitor->visible());

        // var4
        monitor = monitors[8];
        ASSERT_EQ(monitor->id(), "k^cUO5^Pcq!9lY|dxlg+");
        ASSERT_EQ(monitor->mode(), Monitor::Mode::Slider);
        ASSERT_EQ(monitor->opcode(), "data_variable");
        block = monitor->block();
        ASSERT_EQ(block->fields().size(), 1);
        field = block->fieldAt(0);
        ASSERT_EQ(field->name(), "VARIABLE");
        ASSERT_EQ(field->value(), "var4");
        ASSERT_EQ(field->valuePtr(), GET_VAR(stage, "var4"));
        ASSERT_EQ(GET_VAR(stage, "var4")->monitor(), monitor.get());
        ASSERT_EQ(monitor->sprite(), nullptr);
        ASSERT_EQ(monitor->width(), 0);
        ASSERT_EQ(monitor->height(), 0);
        ASSERT_EQ(monitor->x(), 305);
        ASSERT_EQ(monitor->y(), 234);
        ASSERT_TRUE(monitor->visible());

        // var5
        monitor = monitors[9];
        ASSERT_EQ(monitor->id(), "lWC9K6_lG+vq!)EX}npr");
        ASSERT_EQ(monitor->mode(), Monitor::Mode::Slider);
        ASSERT_EQ(monitor->opcode(), "data_variable");
        block = monitor->block();
        ASSERT_EQ(block->fields().size(), 1);
        field = block->fieldAt(0);
        ASSERT_EQ(field->name(), "VARIABLE");
        ASSERT_EQ(field->value(), "var5");
        ASSERT_EQ(field->valuePtr(), GET_VAR(stage, "var5"));
        ASSERT_EQ(GET_VAR(stage, "var5")->monitor(), monitor.get());
        ASSERT_EQ(monitor->sprite(), nullptr);
        ASSERT_EQ(monitor->width(), 0);
        ASSERT_EQ(monitor->height(), 0);
        ASSERT_EQ(monitor->x(), 304);
        ASSERT_EQ(monitor->y(), 280);
        ASSERT_TRUE(monitor->visible());

        // User agent
        ASSERT_TRUE(engine->userAgent().empty());

        i++;
    }
}
#endif // USE_LLVM

TEST(LoadProjectTest, LoadTopLevelReporterProject)
{
    int i = 0;
    for (auto ext : fileExtensions) {
        Project p("top_level_reporter" + ext);

        ASSERT_TRUE(p.load());

        auto engine = p.engine();
        ASSERT_EQ(engine->targets().size(), 2);

        // Sprite1
        ASSERT_NE(engine->findTarget("Sprite1"), -1);
        Sprite *sprite1 = dynamic_cast<Sprite *>(engine->targetAt(engine->findTarget("Sprite1")));
        ASSERT_EQ(sprite1->blocks().size(), 2);

        // Sprite1: blocks
        auto block1 = sprite1->blockAt(0);
        ASSERT_TRUE(block1);
        ASSERT_TRUE(block1->isTopLevelReporter());
        ASSERT_EQ(block1->x(), 335);
        ASSERT_EQ(block1->y(), 335);
        InputValue *reporterInfo = block1->topLevelReporterInfo();
        ASSERT_TRUE(reporterInfo);
        ASSERT_EQ(reporterInfo->type(), InputValue::Type::Variable);
        ASSERT_EQ(reporterInfo->value(), "var");
        ASSERT_EQ(reporterInfo->valueId(), "b[Pq/qD6PJ+hpWtz;X`G");
        ASSERT_EQ(reporterInfo->valuePtr(), sprite1->variableAt(0));

        auto block2 = sprite1->blockAt(1);
        ASSERT_TRUE(block2);
        ASSERT_TRUE(block2->isTopLevelReporter());
        ASSERT_EQ(block2->x(), 313);
        ASSERT_EQ(block2->y(), 435);
        reporterInfo = block2->topLevelReporterInfo();
        ASSERT_TRUE(reporterInfo);
        ASSERT_EQ(reporterInfo->type(), InputValue::Type::List);
        ASSERT_EQ(reporterInfo->value(), "list");
        ASSERT_EQ(reporterInfo->valueId(), "*BW*~d~DgcZee0q=v*T2");
        ASSERT_EQ(reporterInfo->valuePtr(), sprite1->listAt(0));

        i++;
    }
}

TEST(LoadProjectTest, ProjectTest)
{
    int i = 0;
    for (auto ext : fileExtensions) {
        std::string name = "load_test" + ext;
        Project p(name);
        ASSERT_EQ(p.fileName(), name);
        ASSERT_TRUE(p.load());

        auto engine = p.engine();

        ASSERT_EQ(engine->targets().size(), 3);
        ASSERT_EQ(engine->extensions().size(), 0);
        ASSERT_EQ(engine->broadcasts().size(), 1);

        auto broadcast = engine->broadcastAt(0);
        ASSERT_EQ(broadcast->name(), "message1");
        ASSERT_FALSE(broadcast->isBackdropBroadcast());

        i++;
    }
}

TEST(LoadProjectTest, LoadNullDimensionMonitor)
{
    // Regtest for #448
    std::string name = "regtest_projects/448_null_monitor_dimension.sb3";
    Project p(name);
    ASSERT_TRUE(p.load());
}

TEST(LoadProjectTest, LoadDoubleValue)
{
    // Regtest for #437
    std::string oldLocale = std::setlocale(LC_NUMERIC, nullptr);
    std::setlocale(LC_NUMERIC, "sk_SK.UTF-8");

    std::string name = "regtest_projects/437_load_double_values.sb3";
    Project p(name);
    ASSERT_TRUE(p.load());

    auto stage = p.engine()->stage();
    ASSERT_TRUE(stage);
    ASSERT_VAR(stage, "test");
    ASSERT_EQ(GET_VAR(stage, "test")->value().toDouble(), 5.6654);

    std::setlocale(LC_NUMERIC, oldLocale.c_str());
}

TEST(LoadProjectTest, LoadScientificNotationNumber)
{
    // Regtest for #488
    std::string name = "regtest_projects/488_scientific_notation_conversion.sb3";
    Project p(name);
    ASSERT_TRUE(p.load());

    auto stage = p.engine()->stage();
    ASSERT_TRUE(stage);
    ASSERT_VAR(stage, "test1");
    ASSERT_EQ(GET_VAR(stage, "test1")->value().toDouble(), 5000);

    ASSERT_VAR(stage, "test2");
    ASSERT_EQ(GET_VAR(stage, "test2")->value().toDouble(), 4e-323);
}
