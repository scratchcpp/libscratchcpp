#include <scratchcpp/broadcast.h>
#include <scratchcpp/block.h>

#include "../common.h"
#include "testsection.h"
#include "engine/internal/engine.h"

using namespace libscratchcpp;

// NOTE: resolveIds() and compile() are tested in load_project_test

TEST(EngineTest, Clear)
{
    Engine engine;

    auto target1 = std::make_shared<Target>();
    auto target2 = std::make_shared<Target>();
    engine.setTargets({ target1, target2 });

    auto broadcast1 = std::make_shared<Broadcast>("", "");
    auto broadcast2 = std::make_shared<Broadcast>("", "");
    engine.setBroadcasts({ broadcast1, broadcast2 });

    auto section = std::make_shared<TestSection>();
    engine.registerSection(section);

    engine.clear();
    ASSERT_TRUE(engine.targets().empty());
    ASSERT_TRUE(engine.broadcasts().empty());
    ASSERT_TRUE(engine.registeredSections().empty());
}

TEST(EngineTest, BreakFrame)
{
    Engine engine;
    ASSERT_FALSE(engine.breakingCurrentFrame());

    engine.breakFrame();
    ASSERT_TRUE(engine.breakingCurrentFrame());
}

TEST(EngineTest, Sections)
{
    Engine engine;

    auto section1 = std::make_shared<TestSection>();
    engine.registerSection(section1);

    auto section2 = std::make_shared<TestSection>();
    engine.registerSection(section2);

    engine.registerSection(section1); // register existing section

    ASSERT_EQ(engine.registeredSections().size(), 2);
    if (engine.registeredSections()[0] == section1)
        ASSERT_EQ(engine.registeredSections()[1].get(), section2.get());
    else {
        ASSERT_EQ(engine.registeredSections()[0].get(), section2.get());
        ASSERT_EQ(engine.registeredSections()[1].get(), section1.get());
    }
}

unsigned int testFunction1(VirtualMachine *)
{
    return 0;
}

unsigned int testFunction2(VirtualMachine *)
{
    return 0;
}

TEST(EngineTest, Functions)
{
    Engine engine;

    ASSERT_EQ(engine.functionIndex(&testFunction1), 0);
    ASSERT_EQ(engine.functionIndex(&testFunction2), 1);
    ASSERT_EQ(engine.functionIndex(&testFunction1), 0);
    ASSERT_EQ(engine.functionIndex(&testFunction2), 1);
}

void compileTest1(Compiler *)
{
}

void compileTest2(Compiler *)
{
}

TEST(EngineTest, CompileFunctions)
{
    Engine engine;

    auto section1 = std::make_shared<TestSection>();
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<TestSection>();
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    TestSection section3;

    engine.addCompileFunction(section1.get(), "test1", &compileTest1);
    engine.addCompileFunction(section2.get(), "test2", &compileTest2);
    engine.addCompileFunction(section1.get(), "test1", &compileTest1);
    engine.addCompileFunction(&section3, "test1", &compileTest1);

    ASSERT_EQ(container1->resolveBlockCompileFunc("test1"), &compileTest1);
    ASSERT_EQ(container1->resolveBlockCompileFunc("test2"), nullptr);
    ASSERT_EQ(container2->resolveBlockCompileFunc("test1"), nullptr);
    ASSERT_EQ(container2->resolveBlockCompileFunc("test2"), &compileTest2);
}

TEST(EngineTest, HatBlocks)
{
    Engine engine;

    auto section1 = std::make_shared<TestSection>();
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<TestSection>();
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    TestSection section3;

    engine.addHatBlock(section1.get(), "test1");
    engine.addHatBlock(section2.get(), "test2");
    engine.addHatBlock(section1.get(), "test1");
    engine.addHatBlock(&section3, "test1");

    ASSERT_NE(container1->resolveBlockCompileFunc("test1"), nullptr);
    ASSERT_EQ(container1->resolveBlockCompileFunc("test2"), nullptr);
    ASSERT_EQ(container2->resolveBlockCompileFunc("test1"), nullptr);
    ASSERT_NE(container2->resolveBlockCompileFunc("test2"), nullptr);
}

TEST(EngineTest, Inputs)
{
    Engine engine;

    auto section1 = std::make_shared<TestSection>();
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<TestSection>();
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    TestSection section3;

    engine.addInput(section1.get(), "VALUE1", 1);
    engine.addInput(section2.get(), "VALUE2", 2);
    engine.addInput(section1.get(), "VALUE1", 3); // change ID of existing input
    engine.addInput(&section3, "VALUE3", 4);

    ASSERT_EQ(container1->resolveInput("VALUE1"), 3);
    ASSERT_EQ(container1->resolveInput("VALUE2"), -1);
    ASSERT_EQ(container1->resolveInput("VALUE3"), -1);
    ASSERT_EQ(container2->resolveInput("VALUE1"), -1);
    ASSERT_EQ(container2->resolveInput("VALUE2"), 2);
    ASSERT_EQ(container2->resolveInput("VALUE3"), -1);
}

TEST(EngineTest, Fields)
{
    Engine engine;

    auto section1 = std::make_shared<TestSection>();
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<TestSection>();
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    TestSection section3;

    engine.addField(section1.get(), "VALUE1", 1);
    engine.addField(section2.get(), "VALUE2", 2);
    engine.addField(section1.get(), "VALUE1", 3); // change ID of existing field
    engine.addField(&section3, "VALUE3", 4);

    ASSERT_EQ(container1->resolveField("VALUE1"), 3);
    ASSERT_EQ(container1->resolveField("VALUE2"), -1);
    ASSERT_EQ(container1->resolveField("VALUE3"), -1);
    ASSERT_EQ(container2->resolveField("VALUE1"), -1);
    ASSERT_EQ(container2->resolveField("VALUE2"), 2);
    ASSERT_EQ(container2->resolveField("VALUE3"), -1);
}

TEST(EngineTest, FieldValues)
{
    Engine engine;

    auto section1 = std::make_shared<TestSection>();
    engine.registerSection(section1);
    auto container1 = engine.blockSectionContainer(section1.get());

    auto section2 = std::make_shared<TestSection>();
    engine.registerSection(section2);
    auto container2 = engine.blockSectionContainer(section2.get());

    TestSection section3;

    engine.addFieldValue(section1.get(), "value1", 1);
    engine.addFieldValue(section2.get(), "value2", 2);
    engine.addFieldValue(section1.get(), "value1", 3); // change ID of existing field
    engine.addFieldValue(&section3, "value3", 4);

    ASSERT_EQ(container1->resolveFieldValue("value1"), 3);
    ASSERT_EQ(container1->resolveFieldValue("value2"), -1);
    ASSERT_EQ(container1->resolveFieldValue("value3"), -1);
    ASSERT_EQ(container2->resolveFieldValue("value1"), -1);
    ASSERT_EQ(container2->resolveFieldValue("value2"), 2);
    ASSERT_EQ(container2->resolveFieldValue("value3"), -1);
}

TEST(EngineTest, Broadcasts)
{
    Engine engine;
    ASSERT_TRUE(engine.broadcasts().empty());

    auto b1 = std::make_shared<Broadcast>("a", "message1");
    auto b2 = std::make_shared<Broadcast>("b", "message2");
    auto b3 = std::make_shared<Broadcast>("c", "Test");
    engine.setBroadcasts({ b1, b2, b3 });

    ASSERT_EQ(engine.broadcasts(), std::vector<std::shared_ptr<Broadcast>>({ b1, b2, b3 }));
    ASSERT_EQ(engine.broadcastAt(0), b1);
    ASSERT_EQ(engine.broadcastAt(1), b2);
    ASSERT_EQ(engine.broadcastAt(2), b3);
    ASSERT_EQ(engine.broadcastAt(3), nullptr);
    ASSERT_EQ(engine.broadcastAt(-1), nullptr);

    ASSERT_EQ(engine.findBroadcast("invalid"), -1);
    ASSERT_EQ(engine.findBroadcast("message1"), 0);
    ASSERT_EQ(engine.findBroadcast("message2"), 1);
    ASSERT_EQ(engine.findBroadcast("Test"), 2);

    ASSERT_EQ(engine.findBroadcastById("d"), -1);
    ASSERT_EQ(engine.findBroadcastById("a"), 0);
    ASSERT_EQ(engine.findBroadcastById("b"), 1);
    ASSERT_EQ(engine.findBroadcastById("c"), 2);
}

TEST(EngineTest, Targets)
{
    Engine engine;
    ASSERT_TRUE(engine.targets().empty());

    auto t1 = std::make_shared<Target>();
    t1->setName("Sprite1");
    auto t2 = std::make_shared<Target>();
    t2->setName("Sprite2");
    auto t3 = std::make_shared<Target>();
    t3->setName("Stage");
    engine.setTargets({ t1, t2, t3 });

    ASSERT_EQ(engine.targets(), std::vector<std::shared_ptr<Target>>({ t1, t2, t3 }));
    ASSERT_EQ(engine.targetAt(0), t1.get());
    ASSERT_EQ(engine.targetAt(1), t2.get());
    ASSERT_EQ(engine.targetAt(2), t3.get());
    ASSERT_EQ(engine.targetAt(3), nullptr);
    ASSERT_EQ(engine.targetAt(-1), nullptr);

    ASSERT_EQ(engine.findTarget("invalid"), -1);
    ASSERT_EQ(engine.findTarget("Sprite1"), 0);
    ASSERT_EQ(engine.findTarget("Sprite2"), 1);
    ASSERT_EQ(engine.findTarget("Stage"), 2);
}
