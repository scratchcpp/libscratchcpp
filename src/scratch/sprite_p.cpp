// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/ispritehandler.h>

#include "sprite_p.h"

using namespace libscratchcpp;

SpritePrivate::SpritePrivate()
{
}

void SpritePrivate::setCostumeData(const char *data)
{
    if (iface)
        iface->onCostumeChanged(data);
}
