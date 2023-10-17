// SPDX-License-Identifier: Apache-2.0

#include <unordered_map>
#include <algorithm>

#include "keyevent_p.h"

namespace libscratchcpp
{

static std::unordered_map<KeyEvent::Type, std::string> KEY_NAME = {
    { KeyEvent::Type::Space, "space" },       { KeyEvent::Type::Left, "left arrow" }, { KeyEvent::Type::Up, "up arrow" },
    { KeyEvent::Type::Right, "right arrow" }, { KeyEvent::Type::Down, "down arrow" }, { KeyEvent::Type::Enter, "enter" }
};

KeyEventPrivate::KeyEventPrivate(KeyEvent::Type type) :
    type(type)
{
    auto it = KEY_NAME.find(type);

    if (it == KEY_NAME.cend())
        this->type = KeyEvent::Type::Any;
    else
        name = it->second;
}

KeyEventPrivate::KeyEventPrivate(const std::string &name) :
    name(name)
{
    // See https://github.com/scratchfoundation/scratch-vm/blob/2a00145b8a968b04f278808858f79fa6d6e79946/src/io/keyboard.js#L93

    // Convert from ASCII if a number was dropped in
    try {
        double numberDouble = std::stod(name);
        long number = static_cast<long>(numberDouble);

        if (numberDouble == number) {

            if (number >= 48 && number <= 90) {
                this->name = static_cast<char>(number);
                convertNameToLowercase();

                return;
            }

            switch (number) {
                case 32:
                    type = KeyEvent::Type::Space;
                    break;

                case 37:
                    type = KeyEvent::Type::Left;
                    break;

                case 38:
                    type = KeyEvent::Type::Up;
                    break;

                case 39:
                    type = KeyEvent::Type::Right;
                    break;

                case 40:
                    type = KeyEvent::Type::Down;
                    break;

                default:
                    break;
            }

            if (type != KeyEvent::Type::Any) {
                this->name = KEY_NAME[type];
                return;
            }
        }
    } catch (...) {
    }

    // Check for a special key
    for (const auto &[key, value] : KEY_NAME) {
        if (value == name) {
            type = key;
            return;
        }
    }

    // Use only the first character
    if (this->name.size() > 1)
        this->name = this->name[0];

    // Check for the space character
    if (this->name == " ") {
        type = KeyEvent::Type::Space;
        this->name = KEY_NAME[type];
    }

    convertNameToLowercase();
}

void KeyEventPrivate::convertNameToLowercase()
{
    std::transform(this->name.begin(), this->name.end(), this->name.begin(), ::tolower);
}

} // namespace libscratchcpp
