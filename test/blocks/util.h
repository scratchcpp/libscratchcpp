#pragma once

namespace libscratchcpp
{

class IEngine;
class IExtension;

bool conditionReturnValue = false;

void registerBlocks(IEngine *engine, IExtension *extension);

} // namespace libscratchcpp
