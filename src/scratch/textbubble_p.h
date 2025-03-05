// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/textbubble.h>

namespace libscratchcpp
{

struct TextBubblePrivate
{
        TextBubble::Type type = TextBubble::Type::Say;
        std::string text;
        Thread *owner = nullptr;
        mutable sigslot::signal<TextBubble::Type> typeChanged;
        mutable sigslot::signal<const std::string &> textChanged;
};

} // namespace libscratchcpp
