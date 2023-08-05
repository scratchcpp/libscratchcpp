// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/istagehandler.h>

#include "stage_p.h"

using namespace libscratchcpp;

StagePrivate::StagePrivate()
{
}

void StagePrivate::setCostumeData(const char *data)
{
    if (iface)
        iface->onCostumeChanged(data);
}
