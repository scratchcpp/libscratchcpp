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

bool TestExtension::includeByDefault() const
{
    return IExtension::includeByDefault();
}

void TestExtension::registerSections(IEngine *engine)
{
    engine->registerSection(nullptr);
}
