// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <scratchcpp/costume.h>
#include <scratchcpp/sound.h>

namespace libscratchcpp
{

class IEngine;
class Variable;
class List;
class Block;

struct TargetPrivate
{
        TargetPrivate();
        TargetPrivate(const TargetPrivate &) = delete;

        IEngine *engine = nullptr;
        std::string name;
        std::vector<std::shared_ptr<Variable>> variables;
        std::vector<std::shared_ptr<List>> lists;
        std::vector<std::shared_ptr<Block>> blocks;
        int costumeIndex = -1;
        std::vector<std::shared_ptr<Costume>> costumes;
        std::vector<std::shared_ptr<Sound>> sounds;
        int layerOrder = 0;
        double volume = 100;
};

} // namespace libscratchcpp
