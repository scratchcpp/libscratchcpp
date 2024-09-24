// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/textbubble.h>
#include <scratchcpp/value.h>
#include <scratchcpp/iengine.h>

#include "textbubble_p.h"

using namespace libscratchcpp;

TextBubble::TextBubble() :
    Drawable(),
    impl(spimpl::make_unique_impl<TextBubblePrivate>())
{
}

/*! Returns true. */
bool TextBubble::isTextBubble() const
{
    return true;
}

/*! Returns the type of the TextBubble (say or think). */
TextBubble::Type TextBubble::type() const
{
    return impl->type;
}

/*! Sets the type of the TextBubble (say or think). */
void TextBubble::setType(Type type)
{
    impl->type = type;
    impl->typeChanged(type);
}

/*! Emits when the type changes. */
sigslot::signal<TextBubble::Type> &TextBubble::typeChanged() const
{
    return impl->typeChanged;
}

/*!
 * Returns the text of the TextBubble.
 * \note If the text is an empty string, the TextBubble is supposed to be hidden.
 */
const std::string &TextBubble::text() const
{
    return impl->text;
}

/*!
 * Sets the text of the TextBubble.
 * \note If the text is an empty string, the TextBubble is supposed to be hidden.
 */
void TextBubble::setText(const std::string &text)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/7313ce5199f8a3da7850085d0f7f6a3ca2c89bf6/src/blocks/scratch3_looks.js#L251-L257
    const Value v(text);
    std::string converted = text;

    // Non-integers should be rounded to 2 decimal places (no more, no less), unless they're small enough that rounding would display them as 0.00.
    if (v.isValidNumber()) {
        const double num = v.toDouble();

        if (std::abs(num) >= 0.01 && (v % 1).toDouble() != 0)
            converted = Value(std::round(num * 100) / 100).toString();
    }

    // Limit the length of the string
    size_t limit = 330;
    impl->text = converted.substr(0, limit);

    impl->textChanged(impl->text);

    if (!impl->text.empty()) {
        IEngine *eng = engine();

        if (eng)
            eng->moveDrawableToFront(this);
    }
}

/*! Emits when the text changes. */
sigslot::signal<const std::string &> &TextBubble::textChanged() const
{
    return impl->textChanged;
}
