#include <scratchcpp/iengine.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/compilerconstant.h>
#include <scratchcpp/value.h>
#include <iostream>

#include "util.h"

namespace libscratchcpp
{

void registerBlocks(IEngine *engine, IExtension *extension)
{
    engine->addCompileFunction(extension, "test_print", [](Compiler *compiler) -> CompilerValue * {
        auto input = compiler->addInput("STRING");
        compiler->addFunctionCall("test_print", Compiler::StaticType::Void, { Compiler::StaticType::String }, { input });
        return nullptr;
    });

    engine->addCompileFunction(extension, "test_print_test", [](Compiler *compiler) -> CompilerValue * {
        auto input = compiler->addConstValue("test");
        compiler->addFunctionCall("test_print", Compiler::StaticType::Void, { Compiler::StaticType::String }, { input });
        return nullptr;
    });

    engine->addCompileFunction(extension, "test_print_test2", [](Compiler *compiler) -> CompilerValue * {
        auto input = compiler->addConstValue("test2");
        compiler->addFunctionCall("test_print", Compiler::StaticType::Void, { Compiler::StaticType::String }, { input });
        return nullptr;
    });

    engine->addCompileFunction(extension, "test_condition", [](Compiler *compiler) -> CompilerValue * { return compiler->addFunctionCall("test_condition", Compiler::StaticType::Bool); });
}

extern "C" void test_print(const char *str)
{
    std::cout << str << std::endl;
}

extern "C" bool test_condition()
{
    return conditionReturnValue;
}

} // namespace libscratchcpp
