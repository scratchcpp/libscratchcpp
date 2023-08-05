// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/block.h>

#include "target.h"

using namespace libscratchcpp;

/*! Constructs target. */
Target::Target()
{
}

/*! Returns the name of the target. */
const std::string &Target::name() const
{
    return m_name;
}

/*! Sets the name of the target. */
void Target::setName(const std::string &name)
{
    m_name = name;
}

/*! Returns the list of variables. */
const std::vector<std::shared_ptr<Variable>> &Target::variables() const
{
    return m_variables;
}

/*! Adds a variable and returns its index. */
int Target::addVariable(std::shared_ptr<Variable> variable)
{
    m_variables.push_back(variable);
    return m_variables.size() - 1;
}

/*! Returns the variable at index. */
std::shared_ptr<Variable> Target::variableAt(int index) const
{
    return m_variables[index];
}

/*! Returns the index of the variable with the given name. */
int Target::findVariable(const std::string &variableName) const
{
    int i = 0;
    for (auto var : m_variables) {
        if (var->name() == variableName)
            return i;
        i++;
    }
    return -1;
}

/*! Returns the index of the variable with the given ID. */
int Target::findVariableById(const std::string &id) const
{
    int i = 0;
    for (auto var : m_variables) {
        if (var->id() == id)
            return i;
        i++;
    }
    return -1;
}

/*! Returns the list of Scratch lists. */
const std::vector<std::shared_ptr<List>> &Target::lists() const
{
    return m_lists;
}

/*! Adds a list and returns its index. */
int Target::addList(std::shared_ptr<List> list)
{
    m_lists.push_back(list);
    return m_lists.size() - 1;
}

/*! Returns the list at index. */
std::shared_ptr<List> Target::listAt(int index) const
{
    return m_lists[index];
}

/*! Returns the index of the list with the given name. */
int Target::findList(const std::string &listName) const
{
    int i = 0;
    for (auto list : m_lists) {
        if (list->name() == listName)
            return i;
        i++;
    }
    return -1;
}

/*! Returns the index of the list with the given ID. */
int Target::findListById(const std::string &id) const
{
    int i = 0;
    for (auto list : m_lists) {
        if (list->id() == id)
            return i;
        i++;
    }
    return -1;
}

/*! Returns the list of blocks. */
const std::vector<std::shared_ptr<Block>> &Target::blocks() const
{
    return m_blocks;
}

/*! Adds a block and returns its index. */
int Target::addBlock(std::shared_ptr<Block> block)
{
    m_blocks.push_back(block);
    return m_blocks.size() - 1;
}

/*! Returns the block at index. */
std::shared_ptr<Block> Target::blockAt(int index) const
{
    return m_blocks[index];
}

/*! Returns the index of the block with the given ID. */
int Target::findBlock(const std::string &id) const
{
    int i = 0;
    for (auto block : m_blocks) {
        if (block->id() == id)
            return i;
        i++;
    }
    return -1;
}

/*! Returns list of all "when green flag clicked" blocks. */
std::vector<std::shared_ptr<Block>> Target::greenFlagBlocks() const
{
    std::vector<std::shared_ptr<Block>> ret;
    for (auto block : m_blocks) {
        if (block->opcode() == "event_whenflagclicked")
            ret.push_back(block);
    }

    return ret;
}

/*! Returns the ID of the current costume. */
int Target::currentCostume() const
{
    return m_currentCostume;
}

/*! Sets the ID of the current costume. */
void Target::setCurrentCostume(int newCostume)
{
    m_currentCostume = newCostume;
}

/*! Returns the list of costumes. */
const std::vector<Costume> &Target::costumes() const
{
    return m_costumes;
}

/*! Adds a costume and returns its index. */
int Target::addCostume(const Costume &costume)
{
    m_costumes.push_back(costume);
    return m_costumes.size() - 1;
}

/*! Returns the costume at index. */
Costume Target::costumeAt(int index) const
{
    return m_costumes[index];
}

/*! Returns the index of the given costume. */
int Target::findCostume(const std::string &costumeName) const
{
    int i = 0;
    for (auto costume : m_costumes) {
        if (costume.name() == costumeName)
            return i;
        i++;
    }
    return -1;
}

/*! Returns the list of sounds. */
const std::vector<Sound> &Target::sounds() const
{
    return m_sounds;
}

/*! Adds a sound and returns its index. */
int Target::addSound(const Sound &sound)
{
    m_sounds.push_back(sound);
    return m_sounds.size() - 1;
}

/*! Returns the sound at index. */
Sound Target::soundAt(int index) const
{
    return m_sounds[index];
}

/*! Returns the index of the sound with the given name. */
int Target::findSound(const std::string &soundName) const
{
    int i = 0;
    for (auto sound : m_sounds) {
        if (sound.name() == soundName)
            return i;
        i++;
    }
    return -1;
}

/*! Returns the layer number. */
int Target::layerOrder() const
{
    return m_layerOrder;
}

/*! Sets the layer number. */
void Target::setLayerOrder(int newLayerOrder)
{
    m_layerOrder = newLayerOrder;
}

/*! Returns the volume. */
int Target::volume() const
{
    return m_volume;
}

/*! Sets the volume. */
void Target::setVolume(int newVolume)
{
    m_volume = newVolume;
}
