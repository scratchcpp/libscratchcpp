#include <scratchcpp/iengine.h>

#include "testextension.h"

using namespace libscratchcpp;

std::string TestExtension::name() const
{
    return "Test";
}

std::string TestExtension::description() const
{
    return "";
}

void TestExtension::registerBlocks(IEngine *engine)
{
}
