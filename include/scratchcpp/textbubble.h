// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "drawable.h"
#include "signal.h"

namespace libscratchcpp
{

class Thread;
class TextBubblePrivate;

/*! \brief The TextBubble class represents a text bubble created using say or think block. */
class LIBSCRATCHCPP_EXPORT TextBubble : public Drawable
{
    public:
        enum class Type
        {
            Say,
            Think
        };

        TextBubble();
        TextBubble(const TextBubble &) = delete;

        bool isTextBubble() const override final;

        Type type() const;
        virtual void setType(Type type);
        sigslot::signal<Type> &typeChanged() const;

        const std::string &text() const;
        virtual void setText(const std::string &text);
        virtual void setText(const std::string &text, Thread *owner);
        sigslot::signal<const std::string &> &textChanged() const;

        Thread *owner() const;

    private:
        spimpl::unique_impl_ptr<TextBubblePrivate> impl;
};

} // namespace libscratchcpp
