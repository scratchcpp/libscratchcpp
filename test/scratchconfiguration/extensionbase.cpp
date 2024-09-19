#include "extensionbase.h"

using namespace libscratchcpp;

std::string ExtensionBase::name() const
{
    return "";
}

std::string ExtensionBase::description() const
{
    return "";
}

void ExtensionBase::registerBlocks(IEngine *engine)
{
}
