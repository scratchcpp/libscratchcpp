// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unordered_map>
#include <scratchcpp/compiler.h>
#include <scratchcpp/inputvalue.h>

namespace libscratchcpp
{

struct CompilerPrivate
{
        CompilerPrivate(IEngine *engine, Target *target);
        CompilerPrivate(const CompilerPrivate &) = delete;

        void addInstruction(vm::Opcode opcode, std::initializer_list<unsigned int> args = {});

        unsigned int constIndex(InputValue *value, bool pointsToDropdownMenu = false, const std::string &selectedMenuItem = "");

        void substackEnd();

        IEngine *engine = nullptr;
        Target *target = nullptr;
        std::shared_ptr<Block> block;
        std::vector<std::pair<std::pair<std::shared_ptr<Block>, std::shared_ptr<Block>>, Compiler::SubstackType>> substackTree;

        bool initialized = false;

        std::vector<unsigned int> bytecode;
        std::vector<unsigned int> hatPredicateBytecode;
        std::vector<InputValue *> constValues;
        std::vector<std::unique_ptr<InputValue>> customConstValues;
        std::unordered_map<InputValue *, std::pair<bool, std::string>> constValueMenuInfo; // input value, <whether the input points to a dropdown menu, selected menu item>
        std::vector<Variable *> variables;
        std::vector<List *> lists;
        std::vector<std::string> procedures;
        std::unordered_map<std::string, std::vector<std::string>> procedureArgs;
        BlockPrototype *procedurePrototype = nullptr;
        bool warp = false;
};

} // namespace libscratchcpp
