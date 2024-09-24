// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>

#include "drawable.h"
#include "rect.h"
#include "sound.h"

namespace libscratchcpp
{

class Variable;
class List;
class Block;
class Comment;
class Costume;
class Sound;
class Sprite;
class TextBubble;
class IGraphicsEffect;
class TargetPrivate;

/*! \brief The Target class is the Stage or a Sprite. */
class LIBSCRATCHCPP_EXPORT Target : public Drawable
{
    public:
        Target();
        Target(const Target &) = delete;
        virtual ~Target() { }

        bool isTarget() const override final;

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

        const std::vector<std::shared_ptr<Comment>> &comments() const;
        int addComment(std::shared_ptr<Comment> comment);
        std::shared_ptr<Comment> commentAt(int index) const;
        int findComment(const std::string &id) const;

        int costumeIndex() const;
        virtual void setCostumeIndex(int newCostumeIndex);

        std::shared_ptr<Costume> currentCostume() const;
        virtual int currentCostumeWidth() const;
        virtual int currentCostumeHeight() const;

        const std::vector<std::shared_ptr<Costume>> &costumes() const;
        int addCostume(std::shared_ptr<Costume> costume);
        std::shared_ptr<Costume> costumeAt(int index) const;
        int findCostume(const std::string &costumeName) const;

        const std::vector<std::shared_ptr<Sound>> &sounds() const;
        int addSound(std::shared_ptr<Sound> sound);
        std::shared_ptr<Sound> soundAt(int index) const;
        int findSound(const std::string &soundName) const;

        double volume() const;
        void setVolume(double newVolume);

        virtual double soundEffectValue(Sound::Effect effect) const;
        virtual void setSoundEffectValue(Sound::Effect effect, double value);

        virtual void clearSoundEffects();

        virtual Rect boundingRect() const;
        virtual Rect fastBoundingRect() const;

        bool touchingSprite(Sprite *sprite) const;
        virtual bool touchingPoint(double x, double y) const;
        bool touchingEdge() const;
        virtual bool touchingColor(const Value &color) const;
        virtual bool touchingColor(const Value &color, const Value &mask) const;

        double graphicsEffectValue(IGraphicsEffect *effect) const;
        virtual void setGraphicsEffectValue(IGraphicsEffect *effect, double value);

        virtual void clearGraphicsEffects();

        TextBubble *bubble();
        const TextBubble *bubble() const;

        void setEngine(IEngine *engine) override final;

    protected:
        /*! Override this method to set a custom data source for blocks, assets, comments, etc. */
        virtual Target *dataSource() const { return nullptr; }

        /*! Override this method to check whether the target touches the given sprite clones. */
        virtual bool touchingClones(const std::vector<Sprite *> &clones) const { return false; }

    private:
        spimpl::unique_impl_ptr<TargetPrivate> impl;
};

} // namespace libscratchcpp
