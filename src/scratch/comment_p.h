// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <memory>

namespace libscratchcpp
{

class Block;

struct CommentPrivate
{
        CommentPrivate(double x, double y);
        CommentPrivate(const CommentPrivate &) = delete;

        std::string blockId;
        Block *block = nullptr;
        double x = 0;
        double y = 0;
        double width = 200;
        double height = 200;
        bool minimized = false;
        std::string text;
};

} // namespace libscratchcpp
