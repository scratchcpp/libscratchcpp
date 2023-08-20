#include <scratchcpp/iengine.h>

#include "testsection.h"

using namespace libscratchcpp;

std::string TestSection::name() const
{
    return "Test";
}

bool TestSection::categoryVisible() const
{
    return IBlockSection::categoryVisible();
}

void TestSection::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "", nullptr);
}
