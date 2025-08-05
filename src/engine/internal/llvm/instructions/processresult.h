// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace libscratchcpp
{

class LLVMInstruction;

namespace llvmins
{

// Result of group processing
struct ProcessResult
{
        ProcessResult(bool match, LLVMInstruction *next) :
            match(match),
            next(next)
        {
        }

        bool match = false;              // whether the desired instruction was found
        LLVMInstruction *next = nullptr; // next instruction (state)
};

} // namespace llvmins
} // namespace libscratchcpp
