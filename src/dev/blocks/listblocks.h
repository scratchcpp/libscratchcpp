// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iextension.h>

namespace libscratchcpp
{

class List;

class ListBlocks : public IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;

        void registerBlocks(IEngine *engine) override;

    private:
        static CompilerValue *compileAddToList(Compiler *compiler);
        static CompilerValue *getListIndex(Compiler *compiler, CompilerValue *input, List *list, CompilerValue *listSize);
        static CompilerValue *compileDeleteOfList(Compiler *compiler);
        static CompilerValue *compileDeleteAllOfList(Compiler *compiler);
        static CompilerValue *compileInsertAtList(Compiler *compiler);
        static CompilerValue *compileReplaceItemOfList(Compiler *compiler);
        static CompilerValue *compileItemOfList(Compiler *compiler);
        static CompilerValue *compileItemNumOfList(Compiler *compiler);
        static CompilerValue *compileLengthOfList(Compiler *compiler);
        static CompilerValue *compileListContainsItem(Compiler *compiler);
};

} // namespace libscratchcpp
