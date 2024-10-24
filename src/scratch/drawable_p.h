// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/signal.h>

namespace libscratchcpp
{

class IEngine;

struct DrawablePrivate
{
        int layerOrder = 0;
        IEngine *engine = nullptr;
        mutable sigslot::signal<int> layerOrderChanged;
};

} // namespace libscratchcpp
