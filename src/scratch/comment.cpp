// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/comment.h>
#include <scratchcpp/block.h>

#include "comment_p.h"

using namespace libscratchcpp;

/*! Constructs Comment at the given position in the code area. */
Comment::Comment(const std::string &id, double x, double y) :
    Entity(id),
    impl(spimpl::make_unique_impl<CommentPrivate>(x, y))
{
}

/*! Returns the ID of the block the comment is attached to. */
const std::string &Comment::blockId() const
{
    return impl->blockId;
}

/*! Sets the ID of the block the comment is attached to. */
void Comment::setBlockId(const std::string id)
{
    impl->blockId = id;
    impl->block = nullptr;
}

/*! Returns the block the comment is attached to. */
std::shared_ptr<Block> Comment::block() const
{
    return impl->block;
}

/*! Sets the block the comment is attached to. */
void Comment::setBlock(std::shared_ptr<Block> block)
{
    impl->block = block;

    if (block)
        impl->blockId = block->id();
    else
        impl->blockId = "";
}

/*! Returns the x-coordinate of the comment in the code area. */
double Comment::x() const
{
    return impl->x;
}

/*! Sets the x-coordinate of the comment in the code area. */
void Comment::setX(double x)
{
    impl->x = x;
}

/*! Returns the y-coordinate of the comment in the code area. */
double Comment::y() const
{
    return impl->y;
}

/*! Sets the x-coordinate of the comment in the code area. */
void Comment::setY(double y)
{
    impl->y = y;
}

/*! Returns the width. */
double Comment::width() const
{
    return impl->width;
}

/*! Sets the width. */
void Comment::setWidth(double width)
{
    impl->width = width;
}

/*! Returns the height. */
double Comment::height() const
{
    return impl->height;
}

/*! Sets the height. */
void Comment::setHeight(double height)
{
    impl->height = height;
}

/*! Returns true if the comment is collapsed and false otherwise. */
bool Comment::minimized() const
{
    return impl->minimized;
}

/*! Sets whether the comment is collapsed. */
void Comment::setMinimized(bool minimized)
{
    impl->minimized = minimized;
}

/*! Returns the text. */
const std::string &Comment::text() const
{
    return impl->text;
}

/*! Sets the text. */
void Comment::setText(const std::string &text)
{
    impl->text = text;
}
