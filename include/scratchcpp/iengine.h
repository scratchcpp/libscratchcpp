// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "global.h"

namespace libscratchcpp
{

class IBlockSection;
class Broadcast;
class Block;
class Target;
class Sprite;
class Stage;
class Variable;
class List;
class Script;
class ITimer;

/*!
 * \brief The IEngine interface provides an API for running Scratch projects.
 *
 * This interface can be used to manipulate loaded Scratch projects.\n
 * To load a Scratch project, use the Project class.
 */
class LIBSCRATCHCPP_EXPORT IEngine
{
    public:
        virtual ~IEngine() { }

        /*! Clears the engine so that it can be used with another project. */
        virtual void clear() = 0;

        /*!
         * Compiles all scripts to bytecode.
         * \see Compiler
         */
        virtual void compile() = 0;

        /*!
         * Runs a single frame.\n
         * Use this if you want to use a custom event loop
         * in your project player.
         * \note Nothing will happen until start() is called.
         */
        virtual void frame() = 0;

        /*!
         * Calls all "when green flag clicked" blocks.
         * \note Nothing will happen until run() or frame() is called.
         */
        virtual void start() = 0;

        /*! Stops all scripts. */
        virtual void stop() = 0;

        /*! Starts a script with the given top level block as the given Target (a sprite or the stage). */
        virtual void startScript(std::shared_ptr<Block> topLevelBlock, std::shared_ptr<Target> target) = 0;

        /*! Starts the script of the broadcast with the given index. */
        virtual void broadcast(unsigned int index, VirtualMachine *sourceScript, bool wait = false) = 0;

        /*! Starts the script of the given broadcast. */
        virtual void broadcastByPtr(Broadcast *broadcast, VirtualMachine *sourceScript, bool wait = false) = 0;

        /*! Stops the given script. */
        virtual void stopScript(VirtualMachine *vm) = 0;

        /*!
         * Stops all scripts in the given target.
         * \param[in] target The Target to stop.
         * \param[in] exceptScript Sets this parameter to stop all scripts except the given script.
         */
        virtual void stopTarget(Target *target, VirtualMachine *exceptScript) = 0;

        /*! Calls the "when I start as a clone" blocks of the given sprite. */
        virtual void initClone(Sprite *clone) = 0;

        /*! Automatically called from clones that are being deleted. */
        virtual void deinitClone(Sprite *clone) = 0;

        /*!
         * Runs the event loop and calls "when green flag clicked" blocks.
         * \note This function returns when all scripts finish.\n
         * If you need to implement something advanced, such as a GUI with the
         * green flag button, use frame().
         */
        virtual void run() = 0;

        /*! Returns true if the project is currently running. */
        virtual bool isRunning() const = 0;

        /*! Returns the framerate of the project. */
        virtual double fps() const = 0;

        /*! Sets the framerate of the project. */
        virtual void setFps(double fps) = 0;

        /*! Returns true if the given key is pressed. */
        virtual bool keyPressed(const std::string &name) const = 0;

        /*! Sets the state of the given key. */
        virtual void setKeyState(const std::string &name, bool pressed) = 0;

        /*! Sets whether any key is pressed (use this for any key, even for unsupported keys). */
        virtual void setAnyKeyPressed(bool pressed) = 0;

        /*! Returns the X coordinate of the mouse pointer. */
        virtual double mouseX() const = 0;

        /*! Sets the X coordinate of the mouse pointer. */
        virtual void setMouseX(double x) = 0;

        /*! Returns the Y coordinate of the mouse pointer. */
        virtual double mouseY() const = 0;

        /*! Sets the Y coordinate of the mouse pointer. */
        virtual void setMouseY(double y) = 0;

        /*! Returns true if mouse is pressed. */
        virtual bool mousePressed() const = 0;

        /*! Sets the pressed state of mouse. */
        virtual void setMousePressed(bool pressed) = 0;

        /*! Returns the stage width. */
        virtual unsigned int stageWidth() const = 0;

        /*! Sets the stage width. */
        virtual void setStageWidth(unsigned int width) = 0;

        /*! Returns the stage height. */
        virtual unsigned int stageHeight() const = 0;

        /*! Sets the stage height. */
        virtual void setStageHeight(unsigned int height) = 0;

        /*! Returns true if there are any running script of the broadcast with the given index. */
        virtual bool broadcastRunning(unsigned int index, VirtualMachine *sourceScript) = 0;

        /*! Returns true if there are any running script of the given broadcast. */
        virtual bool broadcastByPtrRunning(Broadcast *broadcast, VirtualMachine *sourceScript) = 0;

        /*!
         * Call this from a block implementation to force a "screen refresh".
         * \note This has no effect in "run without screen refresh" custom blocks.
         */
        virtual void breakFrame() = 0;

        /*! Returns true if breakFrame() was called. */
        virtual bool breakingCurrentFrame() = 0;

        /*!
         * Call this from a block implementation to skip a frame and run the next frame immediately.\n
         * The screen will be refreshed according to the frame rate.
         * \note This also works in "run without screen refresh" custom blocks.
         */
        virtual void skipFrame() = 0;

        /*! Call this from a block implementation to ignore calls to skipFrame() until the current frame ends. */
        virtual void lockFrame() = 0;

        /*! Returns the timer of the project. */
        virtual ITimer *timer() const = 0;

        /*!
         * Registers the given block section.
         * \see <a href="blockSections.html">Block sections</a>
         */
        virtual void registerSection(std::shared_ptr<IBlockSection> section) = 0;

        /*! Returns the index of the given block function. */
        virtual unsigned int functionIndex(BlockFunc f) = 0;

        /*!
         * Call this from IBlockSection#registerBlocks() to add a compile function to a block section.
         * \see <a href="blockSections.html">Block sections</a>
         */
        virtual void addCompileFunction(IBlockSection *section, const std::string &opcode, BlockComp f) = 0;

        /*!
         * Call this from IBlockSection#registerBlocks() to add a hat block to a block section.
         * \see <a href="blockSections.html">Block sections</a>
         */
        virtual void addHatBlock(IBlockSection *section, const std::string &opcode) = 0;

        /*!
         * Call this from IBlockSection#registerBlocks() to add an input to a block section.
         * \see <a href="blockSections.html">Block sections</a>
         */
        virtual void addInput(IBlockSection *section, const std::string &name, int id) = 0;

        /*!
         * Call this from IBlockSection#registerBlocks() to add a field to a block section.
         * \see <a href="blockSections.html">Block sections</a>
         */
        virtual void addField(IBlockSection *section, const std::string &name, int id) = 0;

        /*!
         * Call this from IBlockSection#registerBlocks() to add a field value to a block section.
         * \see <a href="blockSections.html">Block sections</a>
         */
        virtual void addFieldValue(IBlockSection *section, const std::string &value, int id) = 0;

        /*! Returns the list of broadcasts. */
        virtual const std::vector<std::shared_ptr<Broadcast>> &broadcasts() const = 0;

        /*! Sets the list of broadcasts. */
        virtual void setBroadcasts(const std::vector<std::shared_ptr<Broadcast>> &broadcasts) = 0;

        /*! Returns the broadcast at index. */
        virtual std::shared_ptr<Broadcast> broadcastAt(int index) const = 0;

        /*! Returns the index of the broadcast with the given name. */
        virtual int findBroadcast(const std::string &broadcastName) const = 0;

        /*! Returns the index of the broadcast with the given ID. */
        virtual int findBroadcastById(const std::string &broadcastId) const = 0;

        /*! Registers the broadcast script. */
        virtual void addBroadcastScript(std::shared_ptr<Block> whenReceivedBlock, Broadcast *broadcast) = 0;

        /* Registers the given "when I start as clone" script. */
        virtual void addCloneInitScript(std::shared_ptr<Block> hatBlock) = 0;

        /* Registers the given "when key pressed" script. */
        virtual void addKeyPressScript(std::shared_ptr<Block> hatBlock, std::string keyName) = 0;

        /*! Returns the list of targets. */
        virtual const std::vector<std::shared_ptr<Target>> &targets() const = 0;

        /*! Sets the list of targets. */
        virtual void setTargets(const std::vector<std::shared_ptr<Target>> &newTargets) = 0;

        /*! Returns the target at index. */
        virtual Target *targetAt(int index) const = 0;

        /*!
         * Returns the index of the target with the given name.
         * \note Using "Stage" will return the index of the sprite with this name, or nullptr if it doesn't exist.
         *       Use the "_stage_" reserved name to get the stage, or use the stage() method.
         */
        virtual int findTarget(const std::string &targetName) const = 0;

        /*! Returns the Stage. */
        virtual Stage *stage() const = 0;

        /*! Returns the list of extension names. */
        virtual const std::vector<std::string> &extensions() const = 0;

        /*! Sets the list of extension names. */
        virtual void setExtensions(const std::vector<std::string> &newExtensions) = 0;

        /*! Returns the map of scripts (each top level block has a Script object). */
        virtual const std::unordered_map<std::shared_ptr<Block>, std::shared_ptr<Script>> &scripts() const = 0;
};

} // namespace libscratchcpp
