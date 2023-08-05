// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <scratchcpp/costume.h>
#include <scratchcpp/sound.h>

namespace libscratchcpp
{

class Variable;
class List;
class Block;

struct TargetPrivate
{
        TargetPrivate();
        TargetPrivate(const TargetPrivate &) = delete;

        std::string name;
        std::vector<std::shared_ptr<Variable>> variables;
        std::vector<std::shared_ptr<List>> lists;
        std::vector<std::shared_ptr<Block>> blocks;
        int currentCostume = 1;
        std::vector<Costume> costumes;
        std::vector<Sound> sounds;
        int layerOrder = 0;
        int volume = 100;
};

} // namespace libscratchcpp
