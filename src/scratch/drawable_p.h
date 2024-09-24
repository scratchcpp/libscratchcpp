// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace libscratchcpp
{

class IEngine;

struct DrawablePrivate
{
        int layerOrder = 0;
        IEngine *engine = nullptr;
};

} // namespace libscratchcpp
