// SPDX-License-Identifier: Apache-2.0

#include "variableblocks.h"

using namespace libscratchcpp;

VariableBlocks::VariableBlocks()
{
    // Blocks
    addBlock("data_setvariableto", &VariableBlocks::setVariable);
    addBlock("data_changevariableby", &VariableBlocks::changeVariableBy);

    // Inputs
    addInput("VALUE", VALUE);

    // Fields
    addField("VARIABLE", VARIABLE);
}

std::string VariableBlocks::name() const
{
    return "Variables";
}

Value VariableBlocks::setVariable(const BlockArgs &args)
{
    auto variable = std::static_pointer_cast<Variable>(args.field(VARIABLE)->valuePtr());
    variable->setValue(args.input(VALUE)->value());
    return Value();
}

Value VariableBlocks::changeVariableBy(const BlockArgs &args)
{
    auto variable = std::static_pointer_cast<Variable>(args.field(VARIABLE)->valuePtr());
    variable->setValue(variable->value().toNumber() + args.input(VALUE)->value().toNumber());
    return Value();
}
