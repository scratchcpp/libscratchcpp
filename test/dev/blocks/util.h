#pragma once

namespace libscratchcpp
{

class IEngine;
class IExtension;

void registerBlocks(IEngine *engine, IExtension *extension);

} // namespace libscratchcpp
