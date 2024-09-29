// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unordered_map>
#include <scratchcpp/blockprototype.h>
#include <scratchcpp/inputvalue.h>

namespace libscratchcpp
{

class Block;
class IEngine;
class Target;
class Input;
class Field;
class Comment;

struct BlockPrivate
{
        BlockPrivate(const std::string &opcode);
        BlockPrivate(const BlockPrivate &) = delete;

        std::string opcode;
#ifndef USE_LLVM
        BlockComp compileFunction = nullptr;
        HatPredicateCompileFunc hatPredicateCompileFunction = nullptr;
#endif
        std::shared_ptr<Block> next = nullptr;
        std::string nextId;
        std::shared_ptr<Block> parent = nullptr;
        std::string parentId;
        std::vector<std::shared_ptr<Input>> inputs;
        std::unordered_map<int, Input *> inputMap;
        std::vector<std::shared_ptr<Field>> fields;
        std::unordered_map<int, Field *> fieldMap;
        bool shadow = false;
        int x = 0;
        int y = 0;
        std::string commentId;
        std::shared_ptr<Comment> comment = nullptr;
        IEngine *engine = nullptr;
        Target *target = nullptr;
        BlockPrototype mutationPrototype;
        bool mutationHasNext = true;
        bool isTopLevelReporter = false;
        std::unique_ptr<InputValue> topLevelReporterInfo = nullptr;
};

} // namespace libscratchcpp
