// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <scratchcpp/costume.h>
#include <scratchcpp/sound.h>
#include <scratchcpp/target.h>
#include <scratchcpp/textbubble.h>

namespace libscratchcpp
{

class IEngine;
class Variable;
class List;
class Block;
class Comment;
class IGraphicsEffect;

struct TargetPrivate
{
        TargetPrivate();
        TargetPrivate(const TargetPrivate &) = delete;

        std::string name;
        std::vector<std::shared_ptr<Variable>> variables;
        std::vector<std::shared_ptr<List>> lists;
        std::vector<std::shared_ptr<Block>> blocks;
        std::vector<std::shared_ptr<Comment>> comments;
        int costumeIndex = -1;
        std::vector<std::shared_ptr<Costume>> costumes;
        std::vector<std::shared_ptr<Sound>> sounds;
        double volume = 100;
        std::unordered_map<Sound::Effect, double> soundEffects;
        std::unordered_map<IGraphicsEffect *, double> graphicsEffects;
        TextBubble bubble;
};

} // namespace libscratchcpp
