#pragma once

namespace libscratchcpp
{

class IEngine;
class IExtension;

void registerBlocks(IEngine *engine, IExtension *extension);
void setConditionReturnValue(bool newValue);

} // namespace libscratchcpp
