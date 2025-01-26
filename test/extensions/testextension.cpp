#include <scratchcpp/iengine.h>

#include "testextension.h"

using namespace libscratchcpp;

std::string TestExtension::name() const
{
    return "Test";
}

std::string TestExtension::description() const
{
    return "Test extension";
}

Rgb TestExtension::color() const
{
    return rgb(0, 0, 0);
}

void TestExtension::registerBlocks(IEngine *engine)
{
    engine->clear();
}

void TestExtension::onInit(IEngine *engine)
{
    engine->clear();
}
