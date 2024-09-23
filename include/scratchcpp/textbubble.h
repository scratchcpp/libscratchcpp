// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "drawable.h"
#include "signal.h"

namespace libscratchcpp
{

class TextBubblePrivate;

/*! \brief The TextBubble class represents a text bubble created using say or think block. */
class TextBubble : public Drawable
{
    public:
        enum class Type
        {
            Say,
            Think
        };

        TextBubble();
        TextBubble(const TextBubble &) = delete;

        Type type() const;
        virtual void setType(Type type);
        sigslot::signal<Type> &typeChanged() const;

        const std::string &text() const;
        virtual void setText(const std::string &text);
        sigslot::signal<const std::string &> &textChanged() const;

    private:
        spimpl::unique_impl_ptr<TextBubblePrivate> impl;
};

} // namespace libscratchcpp
