// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>

#include "global.h"
#include "spimpl.h"

namespace libscratchcpp
{

class Variable;
class List;
class Block;
class Costume;
class Sound;
class TargetPrivate;

/*! \brief The Target class is the Stage or a Sprite. */
class LIBSCRATCHCPP_EXPORT Target
{
    public:
        Target();
        Target(const Target &) = delete;

        /*! Returns true if this Target is the stage. */
        virtual bool isStage() const { return false; }

        const std::string &name() const;
        void setName(const std::string &name);

        const std::vector<std::shared_ptr<Variable>> &variables() const;
        int addVariable(std::shared_ptr<Variable> variable);
        std::shared_ptr<Variable> variableAt(int index) const;
        int findVariable(const std::string &variableName) const;
        int findVariableById(const std::string &id) const;

        const std::vector<std::shared_ptr<List>> &lists() const;
        int addList(std::shared_ptr<List> list);
        std::shared_ptr<List> listAt(int index) const;
        int findList(const std::string &listName) const;
        int findListById(const std::string &id) const;

        const std::vector<std::shared_ptr<Block>> &blocks() const;
        int addBlock(std::shared_ptr<Block> block);
        std::shared_ptr<Block> blockAt(int index) const;
        int findBlock(const std::string &id) const;
        std::vector<std::shared_ptr<Block>> greenFlagBlocks() const;

        int currentCostume() const;
        void setCurrentCostume(int newCostume);

        const std::vector<Costume> &costumes() const;
        int addCostume(const Costume &costume);
        const Costume &costumeAt(int index) const;
        int findCostume(const std::string &costumeName) const;

        const std::vector<Sound> &sounds() const;
        int addSound(const Sound &sound);
        const Sound &soundAt(int index) const;
        int findSound(const std::string &soundName) const;

        int layerOrder() const;
        void setLayerOrder(int newLayerOrder);

        int volume() const;
        void setVolume(int newVolume);

    private:
        spimpl::unique_impl_ptr<TargetPrivate> impl;
};

} // namespace libscratchcpp
