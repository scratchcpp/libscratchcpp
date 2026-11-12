// SPDX-License-Identifier: Apache-2.0

#include "functions.h"
#include "../cppinstruction.h"
#include "../cppcodegenerator.h"

using namespace libscratchcpp;
using namespace libscratchcpp::cppins;

ProcessResult Functions::process(CppInstruction *ins)
{
    ProcessResult ret(true, ins);

    switch (ins->type) {
        case CppInstruction::Type::FunctionCall:
            ret.next = buildFunctionCall(ins);
            break;

        default:
            ret.match = false;
            break;
    }

    return ret;
}

CppInstruction *Functions::buildFunctionCall(CppInstruction *ins)
{
    // TODO: Synchronize variables
    std::vector<const CppIdentifier *> args;

    const auto &insArgs = ins->getArgs();

    for (auto &arg : insArgs) {
        args.push_back(m_codeGen.castValue(arg.second, arg.first));
    }

    if (ins->getFunctionReturnReg()) {
        CppIdentifier::Type retType = m_codeGen.getType(ins->getFunctionReturnReg()->type());
        m_codeGen.createCall(ins->functionName, retType, ins->getFunctionReturnReg(), ins->functionCtxArg, ins->functionTargetArg, args);
    } else {
        m_codeGen.createCall(ins->functionName, ins->functionCtxArg, ins->functionTargetArg, args);
    }

    return ins->getNext();
}
