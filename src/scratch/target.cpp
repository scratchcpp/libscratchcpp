// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/target.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <scratchcpp/block.h>
#include <scratchcpp/comment.h>
#include <scratchcpp/iengine.h>

#include <unordered_set>

#include "target_p.h"

using namespace libscratchcpp;

static const std::unordered_set<std::string> RESERVED_NAMES = { "_mouse_", "_stage_", "_edge_", "_myself_", "_random_" };

/*! Constructs target. */
Target::Target() :
    impl(spimpl::make_unique_impl<TargetPrivate>())
{
}

/*! Returns the name of the target. */
const std::string &Target::name() const
{
    return impl->name;
}

/*!
 * Sets the name of the target.
 * \note Setting the name to one of the reserved names (_mouse_, _stage_, _edge_, _myself_, _random_) won't do anything.
 */
void Target::setName(const std::string &name)
{
    if (RESERVED_NAMES.find(name) == RESERVED_NAMES.cend())
        impl->name = name;
}

/*! Returns the list of variables. */
const std::vector<std::shared_ptr<Variable>> &Target::variables() const
{
    return impl->variables;
}

/*! Adds a variable and returns its index. */
int Target::addVariable(std::shared_ptr<Variable> variable)
{
    auto it = std::find(impl->variables.begin(), impl->variables.end(), variable);

    if (it != impl->variables.end())
        return it - impl->variables.begin();

    impl->variables.push_back(variable);
    variable->setTarget(this);

    return impl->variables.size() - 1;
}

/*! Returns the variable at index. */
std::shared_ptr<Variable> Target::variableAt(int index) const
{
    if (index < 0 || index >= impl->variables.size())
        return nullptr;

    return impl->variables[index];
}

/*! Returns the index of the variable with the given name. */
int Target::findVariable(const std::string &variableName) const
{
    auto it = std::find_if(impl->variables.begin(), impl->variables.end(), [variableName](std::shared_ptr<Variable> variable) { return variable->name() == variableName; });

    if (it == impl->variables.end())
        return -1;
    else
        return it - impl->variables.begin();
}

/*! Returns the index of the variable with the given ID. */
int Target::findVariableById(const std::string &id) const
{
    auto it = std::find_if(impl->variables.begin(), impl->variables.end(), [id](std::shared_ptr<Variable> variable) { return variable->id() == id; });

    if (it == impl->variables.end())
        return -1;
    else
        return it - impl->variables.begin();
}

/*! Returns the list of Scratch lists. */
const std::vector<std::shared_ptr<List>> &Target::lists() const
{
    return impl->lists;
}

/*! Adds a list and returns its index. */
int Target::addList(std::shared_ptr<List> list)
{
    auto it = std::find(impl->lists.begin(), impl->lists.end(), list);

    if (it != impl->lists.end())
        return it - impl->lists.begin();

    impl->lists.push_back(list);
    list->setTarget(this);

    return impl->lists.size() - 1;
}

/*! Returns the list at index. */
std::shared_ptr<List> Target::listAt(int index) const
{
    if (index < 0 || index >= impl->lists.size())
        return nullptr;

    return impl->lists[index];
}

/*! Returns the index of the list with the given name. */
int Target::findList(const std::string &listName) const
{
    auto it = std::find_if(impl->lists.begin(), impl->lists.end(), [listName](std::shared_ptr<List> list) { return list->name() == listName; });

    if (it == impl->lists.end())
        return -1;
    else
        return it - impl->lists.begin();
}

/*! Returns the index of the list with the given ID. */
int Target::findListById(const std::string &id) const
{
    auto it = std::find_if(impl->lists.begin(), impl->lists.end(), [id](std::shared_ptr<List> list) { return list->id() == id; });

    if (it == impl->lists.end())
        return -1;
    else
        return it - impl->lists.begin();
}

/*! Returns the list of blocks. */
const std::vector<std::shared_ptr<Block>> &Target::blocks() const
{
    if (Target *source = dataSource())
        return source->blocks();

    return impl->blocks;
}

/*! Adds a block and returns its index. */
int Target::addBlock(std::shared_ptr<Block> block)
{
    if (Target *source = dataSource())
        return source->addBlock(block);

    auto it = std::find(impl->blocks.begin(), impl->blocks.end(), block);

    if (it != impl->blocks.end())
        return it - impl->blocks.begin();

    impl->blocks.push_back(block);
    return impl->blocks.size() - 1;
}

/*! Returns the block at index. */
std::shared_ptr<Block> Target::blockAt(int index) const
{
    if (Target *source = dataSource())
        return source->blockAt(index);

    if (index < 0 || index >= impl->blocks.size())
        return nullptr;

    return impl->blocks[index];
}

/*! Returns the index of the block with the given ID. */
int Target::findBlock(const std::string &id) const
{
    if (Target *source = dataSource())
        return source->findBlock(id);

    auto it = std::find_if(impl->blocks.begin(), impl->blocks.end(), [id](std::shared_ptr<Block> block) { return block->id() == id; });

    if (it == impl->blocks.end())
        return -1;
    else
        return it - impl->blocks.begin();
}

/*! Returns list of all "when green flag clicked" blocks. */
std::vector<std::shared_ptr<Block>> Target::greenFlagBlocks() const
{
    std::vector<std::shared_ptr<Block>> ret;
    const auto &blockList = blocks();

    for (auto block : blockList) {
        if (block->opcode() == "event_whenflagclicked")
            ret.push_back(block);
    }

    return ret;
}

/*! Returns the list of comments in the code area. */
const std::vector<std::shared_ptr<Comment>> &Target::comments() const
{
    if (Target *source = dataSource())
        return source->comments();

    return impl->comments;
}

/*! Adds a comment and returns its index. */
int Target::addComment(std::shared_ptr<Comment> comment)
{
    if (Target *source = dataSource())
        return source->addComment(comment);

    auto it = std::find(impl->comments.begin(), impl->comments.end(), comment);

    if (it != impl->comments.end())
        return it - impl->comments.begin();

    impl->comments.push_back(comment);
    return impl->comments.size() - 1;
}

/*! Returns the comment at index. */
std::shared_ptr<Comment> Target::commentAt(int index) const
{
    if (Target *source = dataSource())
        return source->commentAt(index);

    if (index < 0 || index >= impl->comments.size())
        return nullptr;

    return impl->comments[index];
}

/*! Returns the index of the comment with the given ID. */
int Target::findComment(const std::string &id) const
{
    if (Target *source = dataSource())
        return source->findComment(id);

    auto it = std::find_if(impl->comments.begin(), impl->comments.end(), [id](std::shared_ptr<Comment> comment) { return comment->id() == id; });

    if (it == impl->comments.end())
        return -1;
    else
        return it - impl->comments.begin();
}

/*! Returns the index of the current costume. */
int Target::costumeIndex() const
{
    return impl->costumeIndex;
}

/*! Sets the index of the current costume. */
void Target::setCostumeIndex(int newCostumeIndex)
{
    if (newCostumeIndex >= 0 && newCostumeIndex < costumes().size())
        impl->costumeIndex = newCostumeIndex;

    if (isStage()) {
        if (impl->engine)
            impl->engine->requestRedraw();
    }
}

/*! Returns the currently set costume. */
std::shared_ptr<Costume> Target::currentCostume() const
{
    return costumeAt(impl->costumeIndex);
}

/*! Returns the width of the current costume. */
int Target::currentCostumeWidth() const
{
    return 0;
}

/*! Returns the height of the current costume. */
int Target::currentCostumeHeight() const
{
    return 0;
}

/*! Returns the list of costumes. */
const std::vector<std::shared_ptr<Costume>> &Target::costumes() const
{
    if (Target *source = dataSource())
        return source->costumes();

    return impl->costumes;
}

/*! Adds a costume and returns its index. */
int Target::addCostume(std::shared_ptr<Costume> costume)
{
    if (Target *source = dataSource())
        return source->addCostume(costume);

    auto it = std::find(impl->costumes.begin(), impl->costumes.end(), costume);

    if (it != impl->costumes.end())
        return it - impl->costumes.begin();

    impl->costumes.push_back(costume);
    return impl->costumes.size() - 1;
}

/*! Returns the costume at index. */
std::shared_ptr<Costume> Target::costumeAt(int index) const
{
    if (Target *source = dataSource())
        return source->costumeAt(index);

    if (index < 0 || index >= impl->costumes.size())
        return nullptr;

    return impl->costumes[index];
}

/*! Returns the index of the given costume. */
int Target::findCostume(const std::string &costumeName) const
{
    if (Target *source = dataSource())
        return source->findCostume(costumeName);

    auto it = std::find_if(impl->costumes.begin(), impl->costumes.end(), [costumeName](std::shared_ptr<Costume> costume) { return costume->name() == costumeName; });

    if (it == impl->costumes.end())
        return -1;
    else
        return it - impl->costumes.begin();
}

/*! Returns the list of sounds. */
const std::vector<std::shared_ptr<Sound>> &Target::sounds() const
{
    return impl->sounds;
}

/*! Adds a sound and returns its index. */
int Target::addSound(std::shared_ptr<Sound> sound)
{
    auto it = std::find(impl->sounds.begin(), impl->sounds.end(), sound);

    if (it != impl->sounds.end())
        return it - impl->sounds.begin();

    assert(sound);

    if (sound) {
        sound->setTarget(this);
        sound->setVolume(impl->volume);

        for (const auto &[effect, value] : impl->soundEffects)
            sound->setEffect(effect, value);
    }

    impl->sounds.push_back(sound);
    return impl->sounds.size() - 1;
}

/*! Returns the sound at index. */
std::shared_ptr<Sound> Target::soundAt(int index) const
{
    if (index < 0 || index >= impl->sounds.size())
        return nullptr;

    return impl->sounds[index];
}

/*! Returns the index of the sound with the given name. */
int Target::findSound(const std::string &soundName) const
{
    auto it = std::find_if(impl->sounds.begin(), impl->sounds.end(), [soundName](std::shared_ptr<Sound> sound) { return sound->name() == soundName; });

    if (it == impl->sounds.end())
        return -1;
    else
        return it - impl->sounds.begin();
}

/*! Returns the layer number. */
int Target::layerOrder() const
{
    return impl->layerOrder;
}

/*! Sets the layer number. */
void Target::setLayerOrder(int newLayerOrder)
{
    impl->layerOrder = newLayerOrder;
}

/*! Returns the volume. */
double Target::volume() const
{
    return impl->volume;
}

/*! Sets the volume of all sounds of this target. */
void Target::setVolume(double newVolume)
{
    if (newVolume >= 100)
        impl->volume = 100;
    else if (newVolume <= 0)
        impl->volume = 0;
    else
        impl->volume = newVolume;

    for (auto sound : impl->sounds) {
        if (sound)
            sound->setVolume(impl->volume);
    }
}

/*! Returns the value of the given sound effect. */
double Target::soundEffectValue(Sound::Effect effect) const
{
    auto it = impl->soundEffects.find(effect);

    if (it != impl->soundEffects.cend())
        return it->second;

    return 0;
}

/*! Sets the value of the given sound effect. */
void Target::setSoundEffectValue(Sound::Effect effect, double value)
{
    impl->soundEffects[effect] = value;

    for (auto sound : impl->sounds) {
        if (sound)
            sound->setEffect(effect, value);
    }
}

/*! Sets the value of all sound effects to 0 (clears them). */
void Target::clearSoundEffects()
{
    std::unordered_map<Sound::Effect, double> effects = impl->soundEffects; // must copy!

    for (const auto &[effect, value] : effects)
        setSoundEffectValue(effect, 0);

    impl->soundEffects.clear();
}

/*! Returns the bounding rectangle of the sprite. */
Rect Target::boundingRect() const
{
    return Rect();
}

/*!
 * Returns the less accurate bounding rectangle of the sprite
 * which is calculated by transforming the costume rectangle.
 */
Rect Target::fastBoundingRect() const
{
    return Rect();
}

/*! Returns true if the Target is touching the given Sprite (or its clones). */
bool Target::touchingSprite(Sprite *sprite) const
{
    // https://github.com/scratchfoundation/scratch-vm/blob/8dbcc1fc8f8d8c4f1e40629fe8a388149d6dfd1c/src/sprites/rendered-target.js#L792-L805
    if (!sprite)
        return false;

    Sprite *firstClone = sprite->isClone() ? sprite->cloneSprite() : sprite;
    assert(firstClone);
    std::vector<Sprite *> clones;

    if (firstClone != this && firstClone->visible()) // TODO: Filter clones that are being dragged, including firstClone
        clones.push_back(firstClone);

    for (auto clone : firstClone->clones()) {
        if (clone.get() != this && clone->visible()) // TODO: Filter clones that are being dragged
            clones.push_back(clone.get());
    }

    return touchingClones(clones);
}

/*! Returns true if the Target is touching the given point (in Scratch coordinates). */
bool Target::touchingPoint(double x, double y) const
{
    return false;
}

/*! Returns true if the target is touching the edge. */
bool Target::touchingEdge() const
{
    // https://github.com/scratchfoundation/scratch-vm/blob/8dbcc1fc8f8d8c4f1e40629fe8a388149d6dfd1c/src/sprites/rendered-target.js#L772-L785
    if (impl->engine) {
        const double stageWidth = impl->engine->stageWidth();
        const double stageHeight = impl->engine->stageHeight();
        Rect bounds = boundingRect();

        if ((bounds.left() < -stageWidth / 2) || (bounds.right() > stageWidth / 2) || (bounds.top() > stageHeight / 2) || (bounds.bottom() < -stageHeight / 2))
            return true;
    }

    return false;
}

/*! Returns true if the Target is touching the given color (RGB triplet). */
bool Target::touchingColor(const Value &color) const
{
    return false;
}

/*! Returns true if the mask part of the Target is touching the given color (RGB triplet). */
bool Target::touchingColor(const Value &color, const Value &mask) const
{
    return false;
}

/*! Returns the value of the given graphics effect. */
double Target::graphicsEffectValue(IGraphicsEffect *effect) const
{
    auto it = impl->graphicsEffects.find(effect);

    if (it == impl->graphicsEffects.cend())
        return 0;
    else
        return it->second;
}

/*! Sets the value of the given graphics effect. */
void Target::setGraphicsEffectValue(IGraphicsEffect *effect, double value)
{
    impl->graphicsEffects[effect] = value;
}

/*! Sets the value of all graphics effects to 0 (clears them). */
void Target::clearGraphicsEffects()
{
    impl->graphicsEffects.clear();
}

/*! Returns the type of the bubble (say or think). */
Target::BubbleType Target::bubbleType() const
{
    return impl->bubbleType;
}

/*! Sets the type of the bubble (say or think). */
void Target::setBubbleType(BubbleType type)
{
    impl->bubbleType = type;
}

/*!
 * Returns the text of the bubble.
 * \note If the text is an empty string, the bubble is supposed to be hidden.
 */
const std::string &Target::bubbleText() const
{
    return impl->bubbleText;
}

/*!
 * Sets the text of the bubble.
 * \note If the text is an empty string, the bubble is supposed to be hidden.
 */
void Target::setBubbleText(const std::string &text)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/7313ce5199f8a3da7850085d0f7f6a3ca2c89bf6/src/blocks/scratch3_looks.js#L251-L257
    const Value v(text);
    std::string converted = text;

    // Non-integers should be rounded to 2 decimal places (no more, no less), unless they're small enough that rounding would display them as 0.00.
    if (v.isValidNumber()) {
        const double num = v.toDouble();

        if (std::abs(num) >= 0.01 && (v % 1).toDouble() != 0)
            converted = Value(std::round(num * 100) / 100).toString();
    }

    // Limit the length of the string
    size_t limit = 330;
    impl->bubbleText = converted.substr(0, limit);
}

/*! Returns the engine. */
IEngine *Target::engine() const
{
    return impl->engine;
}

/*! Sets the engine. */
void Target::setEngine(IEngine *engine)
{
    impl->engine = engine;
}
