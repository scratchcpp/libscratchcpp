#include "scratchproject.h"
#include "scratch/stage.h"
#include "scratch/sprite.h"
#include "../common.h"

using namespace libscratchcpp;

TEST(CompilerTest, EmptyProject)
{
    ScratchProject p("empty_project.sb3", ScratchProject::Version::Scratch3);
    ASSERT_TRUE(p.load());
    const Engine *engine = p.engine();
    ASSERT_EQ(engine->targets().size(), 1);
    ASSERT_EQ(engine->extensions().size(), 0);
    ASSERT_EQ(engine->broadcasts().size(), 0);
    Stage *stage = dynamic_cast<Stage *>(engine->targetAt(0));
    ASSERT_EQ(stage->blocks().size(), 0);
}

TEST(CompilerTest, ResolveIds)
{
    ScratchProject p("resolve_id_test.sb3", ScratchProject::Version::Scratch3);
    ASSERT_TRUE(p.load());
    const Engine *engine = p.engine();
    ASSERT_EQ(engine->targets().size(), 2);
    ASSERT_EQ(engine->extensions().size(), 0);
    ASSERT_EQ(engine->broadcasts().size(), 1);

    // Stage
    ASSERT_NE(engine->findTarget("Stage"), -1);
    Stage *stage = dynamic_cast<Stage *>(engine->targetAt(engine->findTarget("Stage")));
    ASSERT_TRUE(stage);
    ASSERT_EQ(stage->blocks().size(), 2);
    auto block = stage->greenFlagBlocks().at(0);
    ASSERT_TRUE(block);
    ASSERT_EQ(block->parentId(), "");
    ASSERT_FALSE(block->parent());
    std::string greenFlagClickedId = block->id();
    std::string id = block->nextId();

    block = block->next();
    ASSERT_EQ(block->parentId(), greenFlagClickedId);
    ASSERT_TRUE(block);
    ASSERT_EQ(block->id(), id);
    ASSERT_EQ(block->nextId(), "");
    ASSERT_FALSE(block->next());

    // Sprite1
    ASSERT_NE(engine->findTarget("Sprite1"), -1);
    Sprite *sprite1 = dynamic_cast<Sprite *>(engine->targetAt(engine->findTarget("Sprite1")));
    ASSERT_TRUE(sprite1);
    ASSERT_EQ(sprite1->blocks().size(), 5);
    block = sprite1->greenFlagBlocks().at(0);
    ASSERT_TRUE(block);
    ASSERT_EQ(block->parentId(), "");
    ASSERT_FALSE(block->parent());
    greenFlagClickedId = block->id();
    id = block->nextId();

    block = block->next();
    ASSERT_EQ(block->parentId(), greenFlagClickedId);
    ASSERT_TRUE(block);
    ASSERT_EQ(block->id(), id);
    id = block->nextId();

    block = block->next();
    ASSERT_TRUE(block);
    ASSERT_EQ(block->id(), id);
    ASSERT_INPUT(block, "BROADCAST_INPUT");
    auto v = GET_INPUT(block, "BROADCAST_INPUT")->primaryValue();
    ASSERT_EQ(v->valuePtr()->id(), v->valueId());

    block = block->next();
    ASSERT_TRUE(block);
    ASSERT_FIELD(block, "VARIABLE");
    auto field = GET_FIELD(block, "VARIABLE");
    ASSERT_EQ(field->valuePtr()->id(), field->valueId());
    ASSERT_VAR(stage, "my variable");
    auto var = GET_VAR(stage, "my variable");
    ASSERT_EQ(var->id(), field->valueId());
    ASSERT_EQ(var, field->valuePtr());

    block = block->next();
    ASSERT_TRUE(block);
    ASSERT_FIELD(block, "LIST");
    field = GET_FIELD(block, "LIST");
    ASSERT_EQ(field->valuePtr()->id(), field->valueId());
    ASSERT_LIST(stage, "list");
    auto list = GET_LIST(stage, "list");
    ASSERT_EQ(list->id(), field->valueId());
    ASSERT_EQ(list, field->valuePtr());
    ASSERT_EQ(block->nextId(), "");
    ASSERT_FALSE(block->next());
}
