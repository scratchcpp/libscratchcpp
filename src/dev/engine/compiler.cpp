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
    impl->builder = impl->builderFactory->create(impl->target, startBlock->id(), false);
    impl->substackTree.clear();
    impl->substackHit = false;
    impl->warp = false;
    impl->block = startBlock;

    while (impl->block) {
        if (impl->block->compileFunction()) {
            assert(impl->customIfStatementCount == 0);
            impl->block->compile(this);

            if (impl->customIfStatementCount > 0) {
                std::cerr << "error: if statement created by block '" << impl->block->opcode() << "' not terminated" << std::endl;
                assert(false);
            }
        } else {
            std::cout << "warning: unsupported block: " << impl->block->opcode() << std::endl;
            impl->unsupportedBlocks.insert(impl->block->opcode());
        }

        if (impl->substackHit) {
            impl->substackHit = false;
            continue;
        }

        if (impl->block)
            impl->block = impl->block->next();

        if (!impl->block && !impl->substackTree.empty())
            impl->substackEnd();
    }

    return impl->builder->finalize();
}

/*!
 * Adds a call to the given function.\n
 * For example: extern "C" bool some_block(Target *target, double arg1, const char *arg2)
 */
void Compiler::addFunctionCall(const std::string &functionName, StaticType returnType, const std::vector<StaticType> &argTypes)
{
    impl->builder->addFunctionCall(functionName, returnType, argTypes);
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

/*! Creates an add instruction from the last 2 values. */
void Compiler::createAdd()
{
    impl->builder->createAdd();
}

/*! Creates a subtract instruction from the last 2 values. */
void Compiler::createSub()
{
    impl->builder->createSub();
}

/*! Creates a multiply instruction from the last 2 values. */
void Compiler::createMul()
{
    impl->builder->createMul();
}

/*! Creates a divide instruction from the last 2 values. */
void Compiler::createDiv()
{
    impl->builder->createDiv();
}

/*! Creates an equality comparison instruction using the last 2 values. */
void Compiler::createCmpEQ()
{
    impl->builder->createCmpEQ();
}

/*! Creates a greater than comparison instruction using the last 2 values. */
void Compiler::createCmpGT()
{
    impl->builder->createCmpGT();
}

/*! Creates a lower than comparison instruction using the last 2 values. */
void Compiler::createCmpLT()
{
    impl->builder->createCmpLT();
}

/*! Creates an AND operation using the last 2 values. */
void Compiler::createAnd()
{
    impl->builder->createAnd();
}

/*! Creates an OR operation using the last 2 values. */
void Compiler::createOr()
{
    impl->builder->createOr();
}

/*! Creates a NOT operation using the last value. */
void Compiler::createNot()
{
    impl->builder->createNot();
}

/*! Creates a remainder operation using the last 2 values. */
void Compiler::createMod()
{
    impl->builder->createMod();
}

/*! Creates a round operation using the last value. */
void Compiler::createRound()
{
    impl->builder->createRound();
}

/*! Creates an abs operation using the last value. */
void Compiler::createAbs()
{
    impl->builder->createAbs();
}

/*! Creates a floor operation using the last value. */
void Compiler::createFloor()
{
    impl->builder->createFloor();
}

/*! Creates a ceiling operation using the last value. */
void Compiler::createCeil()
{
    impl->builder->createCeil();
}

/*! Creates a square root operation using the last value. */
void Compiler::createSqrt()
{
    impl->builder->createSqrt();
}

/*! Creates a sin operation using the last value. */
void Compiler::createSin()
{
    impl->builder->createSin();
}

/*! Creates a cos operation using the last value. */
void Compiler::createCos()
{
    impl->builder->createCos();
}

/*! Creates a tan operation using the last value. */
void Compiler::createTan()
{
    impl->builder->createTan();
}

/*! Creates an asin operation using the last value. */
void Compiler::createAsin()
{
    impl->builder->createAsin();
}

/*! Creates an acos operation using the last value. */
void Compiler::createAcos()
{
    impl->builder->createAcos();
}

/*! Creates an atan operation using the last value. */
void Compiler::createAtan()
{
    impl->builder->createAtan();
}

/*! Creates an ln operation using the last value. */
void Compiler::createLn()
{
    impl->builder->createLn();
}

/*! Creates a log10 operation using the last value. */
void Compiler::createLog10()
{
    impl->builder->createLog10();
}

/*! Creates an e^x operation using the last value. */
void Compiler::createExp()
{
    impl->builder->createExp();
}

/*! Creates a 10^x operation using the last value. */
void Compiler::createExp10()
{
    impl->builder->createExp10();
}

/*! Creates a variable write operation using the last value. */
void Compiler::createVariableWrite(Variable *variable)
{
    impl->builder->createVariableWrite(variable);
}

/*!
 * Starts a custom if statement.
 * \note The if statement must be terminated using endIf() after compiling your block.
 */
void Compiler::beginIfStatement()
{
    impl->builder->beginIfStatement();
    impl->customIfStatementCount++;
}

/*! Starts the else branch of custom if statement. */
void Compiler::beginElseBranch()
{
    impl->builder->beginElseBranch();
}

/*! Ends custom if statement. */
void Compiler::endIf()
{
    if (impl->customIfStatementCount == 0) {
        std::cerr << "error: called Compiler::endIf() without an if statement";
        assert(false);
        return;
    }

    impl->builder->endIf();
    impl->customIfStatementCount--;
}

/*! Jumps to the given if substack. */
void Compiler::moveToIf(std::shared_ptr<Block> substack)
{
    if (!substack)
        return; // ignore empty if statements

    impl->substackHit = true;
    impl->substackTree.push_back({ { impl->block, nullptr }, CompilerPrivate::SubstackType::IfStatement });
    impl->block = substack;
    impl->builder->beginIfStatement();
}

/*! Jumps to the given if/else substack. The second substack is used for the else branch. */
void Compiler::moveToIfElse(std::shared_ptr<Block> substack1, std::shared_ptr<Block> substack2)
{
    if (!substack1 && !substack2)
        return; // ignore empty if statements

    impl->substackHit = true;
    impl->substackTree.push_back({ { impl->block, substack2 }, CompilerPrivate::SubstackType::IfStatement });
    impl->block = substack1;
    impl->builder->beginIfStatement();

    if (!impl->block)
        impl->substackEnd();
}

/*! Jumps to the given repeat loop substack. */
void Compiler::moveToRepeatLoop(std::shared_ptr<Block> substack)
{
    impl->substackHit = true;
    impl->substackTree.push_back({ { impl->block, nullptr }, CompilerPrivate::SubstackType::Loop });
    impl->block = substack;
    impl->builder->beginRepeatLoop();

    if (!impl->block)
        impl->substackEnd();
}

/*! Jumps to the given while loop substack. */
void Compiler::moveToWhileLoop(std::shared_ptr<Block> substack)
{
    impl->substackHit = true;
    impl->substackTree.push_back({ { impl->block, nullptr }, CompilerPrivate::SubstackType::Loop });
    impl->block = substack;
    impl->builder->beginWhileLoop();

    if (!impl->block)
        impl->substackEnd();
}

/*! Jumps to the given until loop substack. */
void Compiler::moveToRepeatUntilLoop(std::shared_ptr<Block> substack)
{
    impl->substackHit = true;
    impl->substackTree.push_back({ { impl->block, nullptr }, CompilerPrivate::SubstackType::Loop });
    impl->block = substack;
    impl->builder->beginRepeatUntilLoop();

    if (!impl->block)
        impl->substackEnd();
}

/*! Begins a while/until loop condition. */
void Compiler::beginLoopCondition()
{
    impl->builder->beginLoopCondition();
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
