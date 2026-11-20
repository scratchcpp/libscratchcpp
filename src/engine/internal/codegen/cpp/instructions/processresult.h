// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace libscratchcpp
{

class CppInstruction;

namespace cppins
{

// Result of group processing
struct ProcessResult
{
        ProcessResult(bool match, CppInstruction *next) :
            match(match),
            next(next)
        {
        }

        bool match = false;             // whether the desired instruction was found
        CppInstruction *next = nullptr; // next instruction (state)
};

} // namespace cppins
} // namespace libscratchcpp
