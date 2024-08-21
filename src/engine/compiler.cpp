// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/compiler.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/input.h>
#include <scratchcpp/block.h>
#include <scratchcpp/variable.h>
#include <scratchcpp/list.h>
#include <iostream>

#include "compiler_p.h"

using namespace libscratchcpp;
using namespace vm;

/*! Constructs Compiler. */
Compiler::Compiler(IEngine *engine, Target *target) :
    impl(spimpl::make_unique_impl<CompilerPrivate>(engine, target))
{
}

/*! Initializes the Compiler. Useful if you don't use compile(). */
void Compiler::init()
{
    if (impl->initialized)
        return;

    impl->bytecode.clear();
    impl->procedurePrototype = nullptr;
    impl->warp = false;

    // Add start instruction
    addInstruction(OP_START);

    impl->initialized = true;
}

/*! Compiles the script. Use bytecode() to read the generated bytecode. */
void Compiler::compile(std::shared_ptr<Block> topLevelBlock)
{
    init();

    impl->block = topLevelBlock;

    // If this is a top-level block, it might be an edge-activated hat block with a predicate compile function
    if (impl->block->topLevel()) {
        HatPredicateCompileFunc f = impl->block->hatPredicateCompileFunction();

        if (f) {
            f(this);

            // Workaround for register leak warning spam: pause the script after getting the reported value
            addFunctionCall([](VirtualMachine *vm) -> unsigned int {
                vm->stop(false, false, false);
                return 0;
            });

            end(); // finished with the predicate
            impl->hatPredicateBytecode = impl->bytecode;
            init(); // now start the real compilation
        }
    }

    while (impl->block) {
        size_t substacks = impl->substackTree.size();

        if (impl->block->compileFunction())
            impl->block->compile(this);
        else
            std::cout << "warning: unsupported block: " << impl->block->opcode() << std::endl;

        if (substacks != impl->substackTree.size())
            continue;

        if (impl->block)
            impl->block = impl->block->next();

        if (!impl->block && !impl->substackTree.empty())
            impl->substackEnd();
    }

    end();
}

/*! Finalizes the bytecode. Useful if you don't use compile(). */
void Compiler::end()
{
    if (!impl->initialized)
        return;

    // Add end instruction (halt)
    addInstruction(OP_HALT);

    impl->initialized = false;
}

/*! Returns the generated bytecode. */
const std::vector<unsigned int> &Compiler::bytecode() const
{
    return impl->bytecode;
}

/*! Returns the generated hat predicate bytecode (if this is an edge-activated hat). */
const std::vector<unsigned int> &Compiler::hatPredicateBytecode() const
{
    return impl->hatPredicateBytecode;
}

/*! Returns the Engine. */
IEngine *Compiler::engine() const
{
    return impl->engine;
}

Target *Compiler::target() const
{
    return impl->target;
}

/*! Returns the list of constant input values. */
const std::vector<InputValue *> &Compiler::constInputValues() const
{
    return impl->constValues;
}

/*! Returns the list of constant values. */
std::vector<Value> Compiler::constValues() const
{
    std::vector<Value> ret;
    for (auto value : impl->constValues) {
        const auto &menuInfo = impl->constValueMenuInfo.at(value);

        if (menuInfo.first)
            ret.push_back(menuInfo.second);
        else
            ret.push_back(value->value());
    }
    return ret;
}

/*! Returns the list of variables. */
const std::vector<Variable *> &Compiler::variables() const
{
    return impl->variables;
}

/*! Returns the list of pointers to variable values. */
std::vector<Value *> Compiler::variablePtrs() const
{
    std::vector<Value *> ret;
    for (auto var : impl->variables)
        ret.push_back(var->valuePtr());
    return ret;
}

/*! Returns the list of lists. */
const std::vector<List *> &Compiler::lists() const
{
    return impl->lists;
}

/*! Adds an instruction to the bytecode. */
void Compiler::addInstruction(Opcode opcode, const std::initializer_list<unsigned int> &args)
{
    impl->addInstruction(opcode, args);
}

/*! Compiles the given input and adds it to the bytecode. */
void Compiler::addInput(Input *input)
{
    if (!input) {
        addInstruction(OP_NULL);
        return;
    }
    switch (input->type()) {
        case Input::Type::Shadow:
        case Input::Type::NoShadow: {
            if (input->pointsToDropdownMenu())
                addInstruction(OP_CONST, { impl->constIndex(input->primaryValue(), true, input->selectedMenuItem()) });
            else {
                auto previousBlock = impl->block;
                impl->block = input->valueBlock();

                if (impl->block) {
                    if (impl->block->compileFunction())
                        impl->block->compile(this);
                    else {
                        std::cout << "warning: unsupported reporter block: " << impl->block->opcode() << std::endl;
                        addInstruction(OP_NULL);
                    }
                } else
                    addInstruction(OP_CONST, { impl->constIndex(input->primaryValue()) });

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
                    addInstruction(OP_NULL);
                }
            } else
                input->primaryValue()->compile(this);
            impl->block = previousBlock;
            break;
        }
    }
}

/*! Compiles the given input (resolved by ID) and adds it to the bytecode. */
void Compiler::addInput(int id)
{
    addInput(input(id));
}

/*! Adds a constant value and an instruction to load it. Useful if you don't have an input for the addInput() method. */
void libscratchcpp::Compiler::addConstValue(const Value &value)
{
    impl->customConstValues.push_back(std::make_unique<InputValue>());
    impl->customConstValues.back()->setValue(value);
    addInstruction(OP_CONST, { impl->constIndex(impl->customConstValues.back().get()) });
}

/*! Adds a function call to the bytecode (the OP_EXEC instruction). */
void Compiler::addFunctionCall(BlockFunc f)
{
    addInstruction(OP_EXEC, { impl->engine->functionIndex(f) });
}

/*! Adds an argument to a procedure (custom block). */
void Compiler::addProcedureArg(const std::string &procCode, const std::string &argName)
{
    if (impl->procedureArgs.find(procCode) != impl->procedureArgs.cend()) {
        const auto &procedure = impl->procedureArgs[procCode];

        if (std::find(procedure.begin(), procedure.end(), argName) != procedure.end())
            return;
    }

    impl->procedureArgs[procCode].push_back(argName);
}

/*! Jumps to the given substack. The second substack is used for example for the if/else block. */
void Compiler::moveToSubstack(std::shared_ptr<Block> substack1, std::shared_ptr<Block> substack2, SubstackType type)
{
    impl->substackTree.push_back({ { impl->block, substack2 }, type });
    impl->block = substack1;
    if (!impl->block)
        impl->substackEnd();
}

/*! Jumps to the given substack. */
void Compiler::moveToSubstack(std::shared_ptr<Block> substack, SubstackType type)
{
    moveToSubstack(substack, nullptr, type);
}

/*! Makes current script run without screen refresh. */
void Compiler::warp()
{
    impl->warp = true;
    addInstruction(vm::OP_WARP);
}

/*! Returns the input with the given ID. */
Input *Compiler::input(int id) const
{
    return impl->block->findInputById(id);
}

/*! Returns the field with the given ID. */
Field *Compiler::field(int id) const
{
    return impl->block->findFieldById(id);
}

/*! Returns the block in the given input. Same as input(id)->valueBlock(), but with a null pointer check. */
std::shared_ptr<Block> Compiler::inputBlock(int id) const
{
    auto in = input(id);
    return in ? in->valueBlock() : nullptr;
}

/*! Returns the index of the given variable. */
unsigned int Compiler::variableIndex(std::shared_ptr<Entity> varEntity)
{
    assert(varEntity);
    auto var = dynamic_cast<Variable *>(varEntity.get());
    assert(var);
    auto it = std::find(impl->variables.begin(), impl->variables.end(), var);
    if (it != impl->variables.end())
        return it - impl->variables.begin();
    impl->variables.push_back(var);
    return impl->variables.size() - 1;
}

/*! Returns the index of the given list. */
unsigned int Compiler::listIndex(std::shared_ptr<Entity> listEntity)
{
    auto list = dynamic_cast<List *>(listEntity.get());
    auto it = std::find(impl->lists.begin(), impl->lists.end(), list);
    if (it != impl->lists.end())
        return it - impl->lists.begin();
    impl->lists.push_back(list);
    return impl->lists.size() - 1;
}

/*! Returns the index of the given constant input value. */
unsigned int Compiler::constIndex(InputValue *value)
{
    return impl->constIndex(value);
}

/*! Returns the index of the procedure code of the given block. */
unsigned int Compiler::procedureIndex(const std::string &proc)
{
    auto it = std::find(impl->procedures.begin(), impl->procedures.end(), proc);
    if (it != impl->procedures.end())
        return it - impl->procedures.begin();
    impl->procedures.push_back(proc);
    return impl->procedures.size() - 1;
}

/*! Returns the index of the argument of the given procedure (custom block). */
long Compiler::procedureArgIndex(const std::string &procCode, const std::string &argName)
{
    if (impl->procedureArgs.count(procCode) == 0) {
        std::cout << "warning: could not find custom block '" << procCode << "'" << std::endl;
        return -1;
    }
    const std::vector<std::string> args = impl->procedureArgs[procCode];
    auto it = std::find(args.begin(), args.end(), argName);
    if (it != args.end())
        return it - args.begin();
    std::cout << "warning: could not find argument '" << argName << "' in custom block '" << procCode << "'" << std::endl;
    return -1;
}

/*! Returns the prototype of the current custom block. */
BlockPrototype *Compiler::procedurePrototype() const
{
    return impl->procedurePrototype;
}

/*! Sets the prototype of the current custom block. */
void Compiler::setProcedurePrototype(BlockPrototype *prototype)
{
    impl->procedurePrototype = prototype;
}

/*! Returns the list of custom block prototypes. */
const std::vector<std::string> &Compiler::procedures() const
{
    return impl->procedures;
}

/*! Returns the current block. */
const std::shared_ptr<Block> &Compiler::block() const
{
    return impl->block;
}

/*! Sets the current block. Useful if you don't use compile(). */
void Compiler::setBlock(std::shared_ptr<Block> block)
{
    impl->block = block;
}
