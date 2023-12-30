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
#include <projectdownloaderfactorymock.h>
#include <projectdownloadermock.h>

#include "project_p.h"
#include "../common.h"

using namespace libscratchcpp;

static const std::vector<ScratchVersion> scratchVersions = { ScratchVersion::Scratch3 };
static const std::vector<std::string> fileExtensions = { ".sb3" };

using ::testing::Return;
using ::testing::ReturnRef;

TEST(LoadProjectTest, EmptyProject)
{
    int i = 0;
    for (auto version : scratchVersions) {
        Project p("empty_project" + fileExtensions[i], version);

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
    p.detectScratchVersion();
    ASSERT_EQ(p.scratchVersion, ScratchVersion::Scratch3);

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
    p.detectScratchVersion();
    ASSERT_EQ(p.scratchVersion, ScratchVersion::Invalid);
    ASSERT_FALSE(p.load());

    EXPECT_CALL(*downloader, downloadJson("ABCDEFGH")).WillOnce(Return(true));
    EXPECT_CALL(*downloader, json()).WillOnce(ReturnRef(data));
    p.detectScratchVersion();
    ASSERT_EQ(p.scratchVersion, ScratchVersion::Scratch3);

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

TEST(LoadProjectTest, LoadTestProject)
{
    int i = 0;
    for (auto version : scratchVersions) {
        Project p("load_test" + fileExtensions[i], version);

        ASSERT_TRUE(p.load());

        auto engine = p.engine();
        ASSERT_EQ(engine->targets().size(), 3);
        ASSERT_EQ(engine->extensions().size(), 0);
        ASSERT_EQ(engine->broadcasts().size(), 1);

        // Stage
        Stage *stage = engine->stage();
        ASSERT_EQ(stage->comments().size(), 1);
        ASSERT_EQ(stage->variables().size(), 2);
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
        ASSERT_EQ(GET_LIST(stage, "list1")->toString(), "3 1 4 8 7 6 7 7 2 4");

        // Sprite1
        ASSERT_NE(engine->findTarget("Sprite1"), -1);
        Sprite *sprite1 = dynamic_cast<Sprite *>(engine->targetAt(engine->findTarget("Sprite1")));
        ASSERT_FALSE(sprite1->isStage());
        ASSERT_EQ(sprite1->name(), "Sprite1");
        ASSERT_EQ(sprite1->variables().size(), 1);
        ASSERT_EQ(sprite1->lists().size(), 1);
        ASSERT_EQ(sprite1->blocks().size(), 18);
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
        auto sound = sprite1->soundAt(0);
        ASSERT_EQ(sound->name(), "Meow");
        ASSERT_FALSE(sound->id().empty());
        ASSERT_EQ(sound->fileName(), sound->id() + ".wav");
        ASSERT_EQ(sound->dataFormat(), "wav");

        // Sprite1: variables
        ASSERT_VAR(sprite1, "var2");
        ASSERT_EQ(GET_VAR(sprite1, "var2")->value().toDouble(), 0.5);

        // Sprite1: lists
        ASSERT_LIST(sprite1, "list2");
        ASSERT_EQ(GET_LIST(sprite1, "list2")->toString(), "9 6 3 8 0 4 4 2 9 7");

        // Sprite1: blocks
        ASSERT_EQ(sprite1->greenFlagBlocks().size(), 1);
        ASSERT_TRUE(sprite1->greenFlagBlocks()[0]);
        ASSERT_FALSE(sprite1->greenFlagBlocks()[0]->isTopLevelReporter());
        ASSERT_EQ(sprite1->greenFlagBlocks()[0]->opcode(), "event_whenflagclicked");
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
        sound = sprite2->soundAt(0);
        ASSERT_EQ(sound->name(), "Pop");
        ASSERT_FALSE(sound->id().empty());
        ASSERT_EQ(sound->fileName(), sound->id() + ".wav");
        ASSERT_EQ(sound->dataFormat(), "wav");

        // Balloon1: variables
        ASSERT_VAR(sprite2, "var2");
        ASSERT_TRUE(GET_VAR(sprite2, "var2")->value().toBool());

        // Balloon1: lists
        ASSERT_LIST(sprite2, "list2");
        ASSERT_EQ(GET_LIST(sprite2, "list2")->toString(), "0 4 3 4 1 5 6 9 4 8");

        i++;
    }
}

TEST(LoadProjectTest, LoadTopLevelReporterProject)
{
    int i = 0;
    for (auto version : scratchVersions) {
        Project p("top_level_reporter" + fileExtensions[i], version);

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
        InputValue *reporterInfo = block1->topLevelReporterInfo();
        ASSERT_TRUE(reporterInfo);
        ASSERT_EQ(reporterInfo->type(), InputValue::Type::Variable);
        ASSERT_EQ(reporterInfo->value(), "var");
        ASSERT_EQ(reporterInfo->valueId(), "b[Pq/qD6PJ+hpWtz;X`G");
        ASSERT_EQ(reporterInfo->valuePtr(), sprite1->variableAt(0));

        auto block2 = sprite1->blockAt(1);
        ASSERT_TRUE(block2);
        ASSERT_TRUE(block2->isTopLevelReporter());
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
    for (auto version : scratchVersions) {
        std::string name = "load_test" + fileExtensions[i];
        Project p(name, version);
        ASSERT_EQ(p.fileName(), name);
        ASSERT_EQ(p.scratchVersion(), version);
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

TEST(LoadProjectTest, ProjectInvalidTest)
{
    std::string name = "load_test.sb3";
    Project p(name, ScratchVersion::Invalid);
    ASSERT_EQ(p.fileName(), name);
    ASSERT_EQ(p.scratchVersion(), ScratchVersion::Invalid);
    ASSERT_FALSE(p.load());
}
