// SPDX-License-Identifier: Apache-2.0

#include "instructiongroup.h"
#include "../llvmbuildutils.h"

using namespace libscratchcpp::llvmins;

InstructionGroup::InstructionGroup(LLVMBuildUtils &utils) :
    m_utils(utils),
    m_builder(utils.builder())
{
}
