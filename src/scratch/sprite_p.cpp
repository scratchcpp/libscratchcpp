// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/ispritehandler.h>

#include "sprite_p.h"

using namespace libscratchcpp;

SpritePrivate::SpritePrivate()
{
}

void SpritePrivate::removeClone(Sprite *clone)
{
    int index = 0;
    for (const auto &child : childClones) {
        if (child.get() == clone) {
            childClones.erase(childClones.begin() + index);
            return;
        }

        index++;
    }
}

void SpritePrivate::setCostumeData(const char *data)
{
    if (iface)
        iface->onCostumeChanged(data);
}
