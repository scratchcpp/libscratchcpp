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
        BlockPrivate(const std::string &opcode, bool isMonitorBlock);
        BlockPrivate(const BlockPrivate &) = delete;

        std::string opcode;
        BlockComp compileFunction = nullptr;
        HatPredicateCompileFunc hatPredicateCompileFunction = nullptr;
        Block *next = nullptr;
        std::string nextId;
        Block *parent = nullptr;
        std::string parentId;
        std::vector<std::shared_ptr<Input>> inputs;
        std::vector<std::shared_ptr<Field>> fields;
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
        bool isMonitorBlock = false;
};

} // namespace libscratchcpp
