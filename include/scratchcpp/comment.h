// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "entity.h"

namespace libscratchcpp
{

class Block;
class CommentPrivate;

/*! \brief The Comment class represents a comment in the code area. */
class LIBSCRATCHCPP_EXPORT Comment : public Entity
{
    public:
        Comment(const std::string &id, double x = 0, double y = 0);
        Comment(const Comment &) = delete;

        const std::string &blockId() const;
        void setBlockId(const std::string id);

        std::shared_ptr<Block> block() const;
        void setBlock(std::shared_ptr<Block> block);

        double x() const;
        void setX(double x);

        double y() const;
        void setY(double y);

        double width() const;
        void setWidth(double width);

        double height() const;
        void setHeight(double height);

        bool minimized() const;
        void setMinimized(bool minimized);

        const std::string &text() const;
        void setText(const std::string &text);

    private:
        spimpl::unique_impl_ptr<CommentPrivate> impl;
};

} // namespace libscratchcpp
