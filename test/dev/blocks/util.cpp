#include <scratchcpp/iengine.h>
#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/compilerconstant.h>
#include <scratchcpp/value.h>
#include <scratchcpp/field.h>
#include <scratchcpp/variable.h>
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

    engine->addCompileFunction(extension, "test_input", [](Compiler *compiler) -> CompilerValue * { return compiler->addInput("INPUT"); });

    engine->addCompileFunction(extension, "test_const_string", [](Compiler *compiler) -> CompilerValue * {
        auto input = compiler->addInput("STRING");
        return compiler->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String }, { input });
    });

    engine->addCompileFunction(extension, "test_set_var", [](Compiler *compiler) -> CompilerValue * {
        Variable *var = static_cast<Variable *>(compiler->field("VARIABLE")->valuePtr().get());
        compiler->createVariableWrite(var, compiler->addInput("VALUE"));
        return nullptr;
    });
}

extern "C" void test_print(const char *str)
{
    std::cout << str << std::endl;
}

extern "C" bool test_condition()
{
    return conditionReturnValue;
}

extern "C" char *test_const_string(const char *str)
{
    char *ret = (char *)malloc((strlen(str) + 1) * sizeof(char));
    strcpy(ret, str);
    return ret;
}

} // namespace libscratchcpp
