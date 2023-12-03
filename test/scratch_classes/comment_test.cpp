#include <scratchcpp/comment.h>
#include <scratchcpp/block.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(CommentTest, Constructors)
{
    {
        Comment comment("abc");
        ASSERT_EQ(comment.id(), "abc");
        ASSERT_EQ(comment.x(), 0);
        ASSERT_EQ(comment.y(), 0);
    }

    {
        Comment comment("def", 46.09, -64.12);
        ASSERT_EQ(comment.id(), "def");
        ASSERT_EQ(comment.x(), 46.09);
        ASSERT_EQ(comment.y(), -64.12);
    }
}

TEST(CommentTest, Block)
{
    Comment comment("a");
    ASSERT_EQ(comment.block(), nullptr);
    ASSERT_TRUE(comment.blockId().empty());

    auto block = std::make_shared<Block>("abc", "");
    comment.setBlock(block);
    ASSERT_EQ(comment.block(), block);
    ASSERT_EQ(comment.blockId(), "abc");

    comment.setBlock(nullptr);
    ASSERT_EQ(comment.block(), nullptr);
    ASSERT_TRUE(comment.blockId().empty());

    comment.setBlockId("hello");
    ASSERT_EQ(comment.blockId(), "hello");
    ASSERT_EQ(comment.block(), nullptr);
}

TEST(CommentTest, X)
{
    Comment comment("a");
    ASSERT_EQ(comment.x(), 0);

    comment.setX(-76.29);
    ASSERT_EQ(comment.x(), -76.29);
}

TEST(CommentTest, Y)
{
    Comment comment("a");
    ASSERT_EQ(comment.y(), 0);

    comment.setY(38.16);
    ASSERT_EQ(comment.y(), 38.16);
}

TEST(CommentTest, Width)
{
    Comment comment("a");
    ASSERT_EQ(comment.width(), 200);

    comment.setWidth(64.49);
    ASSERT_EQ(comment.width(), 64.49);
}

TEST(CommentTest, Height)
{
    Comment comment("a");
    ASSERT_EQ(comment.height(), 200);

    comment.setHeight(-89.65);
    ASSERT_EQ(comment.height(), -89.65);
}

TEST(CommentTest, Minimized)
{
    Comment comment("a");
    ASSERT_FALSE(comment.minimized());

    comment.setMinimized(true);
    ASSERT_TRUE(comment.minimized());

    comment.setMinimized(false);
    ASSERT_FALSE(comment.minimized());
}

TEST(CommentTest, Text)
{
    Comment comment("a");
    ASSERT_TRUE(comment.text().empty());

    comment.setText("Hello, world!");
    ASSERT_EQ(comment.text(), "Hello, world!");
}
