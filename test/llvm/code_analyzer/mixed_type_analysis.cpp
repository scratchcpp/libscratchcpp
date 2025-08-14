#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <engine/internal/llvm/llvmcodeanalyzer.h>
#include <engine/internal/llvm/llvminstruction.h>
#include <engine/internal/llvm/llvminstructionlist.h>
#include <engine/internal/llvm/llvmconstantregister.h>
#include <gtest/gtest.h>

using namespace libscratchcpp;

TEST(LLVMCodeAnalyzer_MixedTypeAnalysis, EmptyScript)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;

    analyzer.analyzeScript(list);
}

TEST(LLVMCodeAnalyzer_MixedTypeAnalysis, NoOperations)
{
    LLVMCodeAnalyzer analyzer;
    LLVMInstructionList list;

    auto funcCall = std::make_shared<LLVMInstruction>(LLVMInstruction::Type::FunctionCall, false);
    list.addInstruction(funcCall);

    analyzer.analyzeScript(list);
}
