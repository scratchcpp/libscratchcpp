// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>

#include "../inputvalue.h"

namespace libscratchcpp
{

class IExtension;
class IEngine;
class Target;
class List;

} // namespace libscratchcpp

namespace libscratchcpp::test
{

class ScriptBuilderPrivate;

/*! \brief The ScriptBuilder class is used to build Scratch scripts in unit tests. */
class LIBSCRATCHCPP_EXPORT ScriptBuilder
{
    public:
        ScriptBuilder(IExtension *extension, IEngine *engine, std::shared_ptr<Target> target, bool createHatBlock = true);
        ScriptBuilder(const ScriptBuilder &) = delete;

        ~ScriptBuilder();

        void addBlock(std::shared_ptr<Block> block);
        void addBlock(const std::string &opcode);
        void captureBlockReturnValue();

        void addValueInput(const std::string &name, const Value &value);
        void addNullInput(const std::string &name);

        void addObscuredInput(const std::string &name, std::shared_ptr<Block> valueBlock);
        void addNullObscuredInput(const std::string &name);

        void addDropdownInput(const std::string &name, const std::string &selectedValue);
        void addDropdownField(const std::string &name, const std::string &selectedValue);

        void addEntityInput(const std::string &name, const std::string &entityName, InputValue::Type entityType, std::shared_ptr<Entity> entity);
        void addEntityField(const std::string &name, std::shared_ptr<Entity> entity);

        std::shared_ptr<Block> currentBlock();
        std::shared_ptr<Block> takeBlock();

        void build();
        void run();

        List *capturedValues() const;

        static void buildMultiple(const std::vector<ScriptBuilder *> &builders);

    private:
        void addBlockToList(std::shared_ptr<Block> block);
        void build(std::shared_ptr<Target> target);
        std::string nextId();

        static void addBlocksToTarget(Target *target, const std::vector<std::shared_ptr<Block>> &blocks);
        static void addTargetToEngine(IEngine *engine, std::shared_ptr<Target> target);

        spimpl::unique_impl_ptr<ScriptBuilderPrivate> impl;
};

} // namespace libscratchcpp::test
