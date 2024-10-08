// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/inputvalue.h>

#include "compiler_p.h"
#include "internal/icodebuilderfactory.h"
#include "internal/icodebuilder.h"

using namespace libscratchcpp;

/*! Constructs Compiler. */
Compiler::Compiler(IEngine *engine, Target *target) :
    impl(spimpl::make_unique_impl<CompilerPrivate>(engine, target))
{
}

/*! Returns the Engine of the project. */
IEngine *Compiler::engine() const
{
    return impl->engine;
}

/*! Returns the Target of this compiler. */
Target *Compiler::target() const
{
    return impl->target;
}

/*! Returns currently compiled block. */
std::shared_ptr<libscratchcpp::Block> Compiler::block() const
{
    return impl->block;
}

/*! Compiles the script starting with the given block. */
std::shared_ptr<ExecutableCode> Compiler::compile(std::shared_ptr<Block> startBlock)
{
    impl->builder = impl->builderFactory->create(startBlock->id());
    impl->warp = false;
    impl->block = startBlock;

    while (impl->block) {
        size_t substacks = impl->substackTree.size();

        if (impl->block->compileFunction())
            impl->block->compile(this);
        else {
            std::cout << "warning: unsupported block: " << impl->block->opcode() << std::endl;
            impl->unsupportedBlocks.insert(impl->block->opcode());
        }

        if (substacks != impl->substackTree.size())
            continue;

        impl->block = impl->block->next();

        if (!impl->block && !impl->substackTree.empty())
            impl->substackEnd();
    }

    return impl->builder->finalize();
}

/*!
 * Adds a call to the given function.\n
 * For example: extern "C" some_block(ValueData *ret, ValueData *arg1, ValueData *arg2) has 2 arguments
 */
void Compiler::addFunctionCall(const std::string &functionName, int argCount, bool returns)
{
    impl->builder->addFunctionCall(functionName, argCount, returns);
}

/*! Adds a constant value to the compiled code. */
void Compiler::addConstValue(const Value &value)
{
    impl->builder->addConstValue(value);
}

/*! Adds the given variable to the code (to read it). */
void Compiler::addVariableValue(Variable *variable)
{
    impl->builder->addVariableValue(variable);
}

/*! Adds the given list to the code (to read its string representation). */
void Compiler::addListContents(List *list)
{
    impl->builder->addListContents(list);
}

/*! Compiles the given input (resolved by name) and adds it to the compiled code. */
void Compiler::addInput(const std::string &name)
{
    addInput(impl->block->inputAt(impl->block->findInput(name)).get());
}

/*! Jumps to the given if substack. */
void Compiler::moveToIf(std::shared_ptr<Block> substack)
{
    impl->substackTree.push_back({ { impl->block, nullptr }, CompilerPrivate::SubstackType::IfStatement });
    impl->block = substack;

    if (!impl->block)
        impl->substackEnd();
}

/*! Jumps to the given if/else substack. The second substack is used for the else branch. */
void Compiler::moveToIfElse(std::shared_ptr<Block> substack1, std::shared_ptr<Block> substack2)
{
    impl->substackTree.push_back({ { impl->block, substack2 }, CompilerPrivate::SubstackType::IfStatement });
    impl->block = substack1;

    if (!impl->block)
        impl->substackEnd();
}

/*! Jumps to the given loop substack. */
void Compiler::moveToLoop(std::shared_ptr<Block> substack)
{
    impl->substackTree.push_back({ { impl->block, nullptr }, CompilerPrivate::SubstackType::Loop });
    impl->block = substack;

    if (!impl->block)
        impl->substackEnd();
}

/*! Makes current script run without screen refresh. */
void Compiler::warp()
{
    impl->warp = true;
}

/*! Convenience method which returns the field with the given name. */
Input *Compiler::input(const std::string &name) const
{
    return impl->block->inputAt(impl->block->findInput(name)).get();
}

/*! Convenience method which returns the field with the given name. */
Field *Compiler::field(const std::string &name) const
{
    return impl->block->fieldAt(impl->block->findField(name)).get();
}

/*! Returns unsupported block opcodes which were found when compiling. */
const std::unordered_set<std::string> &Compiler::unsupportedBlocks() const
{
    return impl->unsupportedBlocks;
}

void Compiler::addInput(Input *input)
{
    if (!input) {
        addConstValue(Value());
        return;
    }

    switch (input->type()) {
        case Input::Type::Shadow:
        case Input::Type::NoShadow: {
            if (input->pointsToDropdownMenu())
                addConstValue(input->selectedMenuItem());
            else {
                auto previousBlock = impl->block;
                impl->block = input->valueBlock();

                if (impl->block) {
                    if (impl->block->compileFunction())
                        impl->block->compile(this);
                    else {
                        std::cout << "warning: unsupported reporter block: " << impl->block->opcode() << std::endl;
                        impl->unsupportedBlocks.insert(impl->block->opcode());
                        addConstValue(Value());
                    }
                } else
                    addConstValue(input->primaryValue()->value());

                impl->block = previousBlock;
            }

            break;
        }

        case Input::Type::ObscuredShadow: {
            auto previousBlock = impl->block;
            impl->block = input->valueBlock();
            if (impl->block) {
                if (impl->block->compileFunction())
                    impl->block->compile(this);
                else {
                    std::cout << "warning: unsupported reporter block: " << impl->block->opcode() << std::endl;
                    impl->unsupportedBlocks.insert(impl->block->opcode());
                    addConstValue(Value());
                }
            } else
                input->primaryValue()->compile(this);

            impl->block = previousBlock;
            break;
        }
    }
}
