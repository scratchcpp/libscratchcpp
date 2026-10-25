// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../instructionlist.h"

namespace libscratchcpp
{

struct LLVMInstruction;

using LLVMInstructionList = InstructionList<LLVMInstruction>;

} // namespace libscratchcpp
