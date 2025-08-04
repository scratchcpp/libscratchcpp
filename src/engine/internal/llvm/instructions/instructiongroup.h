// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <llvm/IR/IRBuilder.h>

#include "processresult.h"

namespace libscratchcpp
{

class LLVMBuildUtils;

namespace llvmins
{

class InstructionGroup
{
    public:
        InstructionGroup(LLVMBuildUtils &utils);

        virtual ProcessResult process(LLVMInstruction *ins) = 0;

    protected:
        LLVMBuildUtils &m_utils;
        llvm::IRBuilder<> &m_builder;
};

} // namespace llvmins
} // namespace libscratchcpp
