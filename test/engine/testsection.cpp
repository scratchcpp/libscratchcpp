#include <scratchcpp/iengine.h>

#include "testsection.h"

using namespace libscratchcpp;

std::string TestSection::name() const
{
    return "Test";
}

void TestSection::registerBlocks(IEngine *engine)
{
}
