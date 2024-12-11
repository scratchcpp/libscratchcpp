// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/dev/compiler.h>
#include <scratchcpp/dev/compilerconstant.h>
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

            if (impl->customLoopCount > 0) {
                std::cerr << "error: loop created by block '" << impl->block->opcode() << "' not terminated" << std::endl;
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
 * For example: extern "C" bool some_block(double arg1, const char *arg2)
 */
CompilerValue *Compiler::addFunctionCall(const std::string &functionName, StaticType returnType, const ArgTypes &argTypes, const Args &args)
{
    assert(argTypes.size() == args.size());
    return impl->builder->addFunctionCall(functionName, returnType, argTypes, args);
}

/*!
 * Adds a call to the given function with a target parameter.\n
 * For example: extern "C" bool some_block(Target *target, double arg1, const char *arg2)
 */
CompilerValue *Compiler::addTargetFunctionCall(const std::string &functionName, StaticType returnType, const ArgTypes &argTypes, const Args &args)
{
    assert(argTypes.size() == args.size());
    return impl->builder->addTargetFunctionCall(functionName, returnType, argTypes, args);
}

/*!
 * Adds a call to the given function with an execution context parameter.\n
 * For example: extern "C" bool some_block(ExecutionContext *ctx, double arg1, const char *arg2)
 */
CompilerValue *Compiler::addFunctionCallWithCtx(const std::string &functionName, StaticType returnType, const ArgTypes &argTypes, const Args &args)
{
    assert(argTypes.size() == args.size());
    return impl->builder->addFunctionCallWithCtx(functionName, returnType, argTypes, args);
}

/*! Adds the given constant to the compiled code. */
CompilerConstant *Compiler::addConstValue(const Value &value)
{
    return static_cast<CompilerConstant *>(impl->builder->addConstValue(value));
}

/*! Adds the value of the given variable to the code. */
CompilerValue *Compiler::addVariableValue(Variable *variable)
{
    return impl->builder->addVariableValue(variable);
}

/*! Adds the string representation of the given list to the code. */
CompilerValue *Compiler::addListContents(List *list)
{
    return impl->builder->addListContents(list);
}

/*! Adds the item with index of the given list to the code. */
CompilerValue *Compiler::addListItem(List *list, CompilerValue *index)
{
    return impl->builder->addListItem(list, index);
}

/*! Adds the index of item of the given list to the code. */
CompilerValue *Compiler::addListItemIndex(List *list, CompilerValue *item)
{
    return impl->builder->addListItemIndex(list, item);
}

/*! Adds the result of a list contains item check to the code. */
CompilerValue *Compiler::addListContains(List *list, CompilerValue *item)
{
    return impl->builder->addListContains(list, item);
}

/*! Adds the length of the given list to the code. */
CompilerValue *Compiler::addListSize(List *list)
{
    return impl->builder->addListSize(list);
}

/*! Compiles the given input (resolved by name) and adds it to the compiled code. */
CompilerValue *Compiler::addInput(const std::string &name)
{
    return addInput(impl->block->inputAt(impl->block->findInput(name)).get());
}

/*! Creates an add instruction. */
CompilerValue *Compiler::createAdd(CompilerValue *operand1, CompilerValue *operand2)
{
    return impl->builder->createAdd(operand1, operand2);
}

/*! Creates a subtract instruction. */
CompilerValue *Compiler::createSub(CompilerValue *operand1, CompilerValue *operand2)
{
    return impl->builder->createSub(operand1, operand2);
}

/*! Creates a multiply instruction. */
CompilerValue *Compiler::createMul(CompilerValue *operand1, CompilerValue *operand2)
{
    return impl->builder->createMul(operand1, operand2);
}

/*! Creates a divide instruction. */
CompilerValue *Compiler::createDiv(CompilerValue *operand1, CompilerValue *operand2)
{
    return impl->builder->createDiv(operand1, operand2);
}

/*! Creates an equality comparison instruction. */
CompilerValue *Compiler::createCmpEQ(CompilerValue *operand1, CompilerValue *operand2)
{
    return impl->builder->createCmpEQ(operand1, operand2);
}

/*! Creates a greater than comparison instruction. */
CompilerValue *Compiler::createCmpGT(CompilerValue *operand1, CompilerValue *operand2)
{
    return impl->builder->createCmpGT(operand1, operand2);
}

/*! Creates a lower than comparison instruction. */
CompilerValue *Compiler::createCmpLT(CompilerValue *operand1, CompilerValue *operand2)
{
    return impl->builder->createCmpLT(operand1, operand2);
}

/*! Creates an AND operation. */
CompilerValue *Compiler::createAnd(CompilerValue *operand1, CompilerValue *operand2)
{
    return impl->builder->createAnd(operand1, operand2);
}

/*! Creates an OR operation. */
CompilerValue *Compiler::createOr(CompilerValue *operand1, CompilerValue *operand2)
{
    return impl->builder->createOr(operand1, operand2);
}

/*! Creates a NOT operation. */
CompilerValue *Compiler::createNot(CompilerValue *operand)
{
    return impl->builder->createNot(operand);
}

/*! Creates a remainder operation. */
CompilerValue *Compiler::createMod(CompilerValue *num1, CompilerValue *num2)
{
    return impl->builder->createMod(num1, num2);
}

/*! Creates a round operation. */
CompilerValue *Compiler::createRound(CompilerValue *num)
{
    return impl->builder->createRound(num);
}

/*! Creates an abs operation. */
CompilerValue *Compiler::createAbs(CompilerValue *num)
{
    return impl->builder->createAbs(num);
}

/*! Creates a floor operation. */
CompilerValue *Compiler::createFloor(CompilerValue *num)
{
    return impl->builder->createFloor(num);
}

/*! Creates a ceiling operation. */
CompilerValue *Compiler::createCeil(CompilerValue *num)
{
    return impl->builder->createCeil(num);
}

/*! Creates a square root operation. */
CompilerValue *Compiler::createSqrt(CompilerValue *num)
{
    return impl->builder->createSqrt(num);
}

/*! Creates a sin operation. */
CompilerValue *Compiler::createSin(CompilerValue *num)
{
    return impl->builder->createSin(num);
}

/*! Creates a cos operation. */
CompilerValue *Compiler::createCos(CompilerValue *num)
{
    return impl->builder->createCos(num);
}

/*! Creates a tan operation. */
CompilerValue *Compiler::createTan(CompilerValue *num)
{
    return impl->builder->createTan(num);
}

/*! Creates an asin operation. */
CompilerValue *Compiler::createAsin(CompilerValue *num)
{
    return impl->builder->createAsin(num);
}

/*! Creates an acos operation. */
CompilerValue *Compiler::createAcos(CompilerValue *num)
{
    return impl->builder->createAcos(num);
}

/*! Creates an atan operation. */
CompilerValue *Compiler::createAtan(CompilerValue *num)
{
    return impl->builder->createAtan(num);
}

/*! Creates an ln operation. */
CompilerValue *Compiler::createLn(CompilerValue *num)
{
    return impl->builder->createLn(num);
}

/*! Creates a log10 operation. */
CompilerValue *Compiler::createLog10(CompilerValue *num)
{
    return impl->builder->createLog10(num);
}

/*! Creates an e^x operation. */
CompilerValue *Compiler::createExp(CompilerValue *num)
{
    return impl->builder->createExp(num);
}

/*! Creates a 10^x operation. */
CompilerValue *Compiler::createExp10(CompilerValue *num)
{
    return impl->builder->createExp10(num);
}

/*! Creates a select instruction (ternary operator). */
CompilerValue *Compiler::createSelect(CompilerValue *cond, CompilerValue *trueValue, CompilerValue *falseValue, StaticType valueType)
{
    return impl->builder->createSelect(cond, trueValue, falseValue, valueType);
}

/*! Creates a variable write operation. */
void Compiler::createVariableWrite(Variable *variable, CompilerValue *value)
{
    impl->builder->createVariableWrite(variable, value);
}

/*! Creates a clear list operation. */
void Compiler::createListClear(List *list)
{
    impl->builder->createListClear(list);
}

/*!
 * Creates a remove item from list operation.
 * \note The index starts with 0 and is cast to number, special strings like "last" are not handled.
 */
void Compiler::createListRemove(List *list, CompilerValue *index)
{
    impl->builder->createListRemove(list, index);
}

/*! Creates a list append operation. */
void Compiler::createListAppend(List *list, CompilerValue *item)
{
    impl->builder->createListAppend(list, item);
}

/*! Creates a list insert operation. */
void Compiler::createListInsert(List *list, CompilerValue *index, CompilerValue *item)
{
    impl->builder->createListInsert(list, index, item);
}

/*! Creates a list replace operation. */
void Compiler::createListReplace(List *list, CompilerValue *index, CompilerValue *item)
{
    impl->builder->createListReplace(list, index, item);
}

/*!
 * Starts a custom if statement.
 * \note The if statement must be terminated using endIf() after compiling your block.
 */
void Compiler::beginIfStatement(CompilerValue *cond)
{
    impl->builder->beginIfStatement(cond);
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

/*!
 * Begins a custom while loop.
 * \note The loop must be terminated with endLoop() after compiling your block.
 */
void Compiler::beginWhileLoop(CompilerValue *cond)
{
    impl->builder->beginWhileLoop(cond);
    impl->customLoopCount++;
}

/*!
 * Begins a custom repeat until loop.
 * \note The loop must be terminated with endLoop() after compiling your block.
 */
void Compiler::beginRepeatUntilLoop(CompilerValue *cond)
{
    impl->builder->beginRepeatUntilLoop(cond);
    impl->customLoopCount++;
}

/*! Begins a while/until loop condition. */
void Compiler::beginLoopCondition()
{
    impl->builder->beginLoopCondition();
}

/*! Ends custom loop. */
void Compiler::endLoop()
{
    if (impl->customLoopCount == 0) {
        std::cerr << "error: called Compiler::endLoop() without a loop";
        assert(false);
        return;
    }

    impl->builder->endLoop();
    impl->customLoopCount--;
}

/*! Jumps to the given if substack. */
void Compiler::moveToIf(CompilerValue *cond, std::shared_ptr<Block> substack)
{
    if (!substack)
        return; // ignore empty if statements

    impl->substackHit = true;
    impl->substackTree.push_back({ { impl->block, nullptr }, CompilerPrivate::SubstackType::IfStatement });
    impl->block = substack;
    impl->builder->beginIfStatement(cond);
}

/*! Jumps to the given if/else substack. The second substack is used for the else branch. */
void Compiler::moveToIfElse(CompilerValue *cond, std::shared_ptr<Block> substack1, std::shared_ptr<Block> substack2)
{
    if (!substack1 && !substack2)
        return; // ignore empty if statements

    impl->substackHit = true;
    impl->substackTree.push_back({ { impl->block, substack2 }, CompilerPrivate::SubstackType::IfStatement });
    impl->block = substack1;
    impl->builder->beginIfStatement(cond);

    if (!impl->block)
        impl->substackEnd();
}

/*! Jumps to the given repeat loop substack. */
void Compiler::moveToRepeatLoop(CompilerValue *count, std::shared_ptr<Block> substack)
{
    impl->substackHit = true;
    impl->substackTree.push_back({ { impl->block, nullptr }, CompilerPrivate::SubstackType::Loop });
    impl->block = substack;
    impl->builder->beginRepeatLoop(count);

    if (!impl->block)
        impl->substackEnd();
}

/*! Jumps to the given while loop substack. */
void Compiler::moveToWhileLoop(CompilerValue *cond, std::shared_ptr<Block> substack)
{
    impl->substackHit = true;
    impl->substackTree.push_back({ { impl->block, nullptr }, CompilerPrivate::SubstackType::Loop });
    impl->block = substack;
    impl->builder->beginWhileLoop(cond);

    if (!impl->block)
        impl->substackEnd();
}

/*! Jumps to the given until loop substack. */
void Compiler::moveToRepeatUntilLoop(CompilerValue *cond, std::shared_ptr<Block> substack)
{
    impl->substackHit = true;
    impl->substackTree.push_back({ { impl->block, nullptr }, CompilerPrivate::SubstackType::Loop });
    impl->block = substack;
    impl->builder->beginRepeatUntilLoop(cond);

    if (!impl->block)
        impl->substackEnd();
}

/*! Makes current script run without screen refresh. */
void Compiler::warp()
{
    impl->warp = true;
}

/*! Creates a stop script instruction. */
void Compiler::createStop()
{
    impl->builder->createStop();
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

CompilerValue *Compiler::addInput(Input *input)
{
    if (!input)
        return addConstValue(Value());

    switch (input->type()) {
        case Input::Type::Shadow:
        case Input::Type::NoShadow: {
            if (input->pointsToDropdownMenu())
                return addConstValue(input->selectedMenuItem());
            else {
                CompilerValue *ret = nullptr;
                auto previousBlock = impl->block;
                impl->block = input->valueBlock();

                if (impl->block) {
                    if (impl->block->compileFunction())
                        ret = impl->block->compile(this);
                    else {
                        std::cout << "warning: unsupported reporter block: " << impl->block->opcode() << std::endl;
                        impl->unsupportedBlocks.insert(impl->block->opcode());
                        ret = addConstValue(Value());
                    }
                } else
                    ret = addConstValue(input->primaryValue()->value());

                impl->block = previousBlock;
                return ret;
            }
        }

        case Input::Type::ObscuredShadow: {
            CompilerValue *ret = nullptr;
            auto previousBlock = impl->block;
            impl->block = input->valueBlock();

            if (impl->block) {
                if (impl->block->compileFunction())
                    ret = impl->block->compile(this);
                else {
                    std::cout << "warning: unsupported reporter block: " << impl->block->opcode() << std::endl;
                    impl->unsupportedBlocks.insert(impl->block->opcode());
                    ret = addConstValue(Value());
                }
            } else
                ret = input->primaryValue()->compile(this);

            impl->block = previousBlock;
            return ret;
        }
    }

    return nullptr;
}
