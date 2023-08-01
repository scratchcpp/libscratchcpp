// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <vector>

#include "global.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_EXPORT IBlockSection;

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

        /*virtual void startScript(std::shared_ptr<Block> topLevelBlock, std::shared_ptr<Target> target) = 0;
        virtual void broadcast(unsigned int index, VirtualMachine *sourceScript) = 0;
        virtual void stopScript(VirtualMachine *vm) = 0;
        virtual void stopTarget(Target *target, VirtualMachine *exceptScript) = 0;*/

        /*!
         * Runs the event loop and calls "when green flag clicked" blocks.
         * \note This function returns when all scripts finish.\n
         * If you need to implement something advanced, such as a GUI with the
         * green flag button, use frame().
         */
        virtual void run() = 0;

        /*! Returns true if there are any running script of the broadcast with the given index. */
        virtual bool broadcastRunning(unsigned int index) = 0;

        /*!
         * Call this from a block implementation to force a "screen refresh".
         * \note This has no effect in "run without screen refresh" custom blocks.
         */
        virtual void breakFrame() = 0;

        /*! Returns true if breakFrame() was called. */
        virtual bool breakingCurrentFrame() = 0;

        /*! Registers the given block section. */
        virtual void registerSection(std::shared_ptr<IBlockSection> section) = 0;

        // virtual unsigned int functionIndex(BlockFunc f);

        /*! Resolves the block and returns the block section in which it has been registered. */
        virtual std::shared_ptr<IBlockSection> blockSection(const std::string &opcode) const = 0;

        /*virtual const std::vector<std::shared_ptr<Broadcast>> &broadcasts() const = 0;
        virtual void setBroadcasts(const std::vector<std::shared_ptr<Broadcast>> &broadcasts) = 0;
        virtual std::shared_ptr<Broadcast> broadcastAt(int index) const = 0;
        virtual int findBroadcast(const std::string &broadcastName) const = 0;
        virtual int findBroadcastById(const std::string &broadcastId) const = 0;*/

        // virtual void addBroadcastScript(std::shared_ptr<Block> whenReceivedBlock, std::shared_ptr<Broadcast> broadcast) = 0;

        /*virtual const std::vector<std::shared_ptr<Target>> &targets() const = 0;
        virtual void setTargets(const std::vector<std::shared_ptr<Target>> &newTargets) = 0;
        virtual Target *targetAt(int index) const = 0;
        virtual int findTarget(const std::string &targetName) const = 0;*/

        /*! Returns the list of extension names. */
        virtual const std::vector<std::string> &extensions() const = 0;

        /*! Sets the list of extension names. */
        virtual void setExtensions(const std::vector<std::string> &newExtensions) = 0;

        // virtual const std::unordered_map<std::shared_ptr<Block>, std::shared_ptr<Script>> &scripts() const = 0;
};

} // namespace libscratchcpp