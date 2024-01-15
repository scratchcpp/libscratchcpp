// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/iblocksection.h>

namespace libscratchcpp
{

class Compiler;

/*! \brief The ListBlocks class contains the implementation of list blocks. */
class ListBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            ITEM,
            INDEX
        };

        enum Fields
        {
            LIST
        };

        std::string name() const override;
        bool categoryVisible() const override;

        void registerBlocks(IEngine *engine) override;

        static void compileListContents(Compiler *compiler);
        static void compileAddToList(Compiler *compiler);
        static void compileDeleteFromList(Compiler *compiler);
        static void compileDeleteAllOfList(Compiler *compiler);
        static void compileInsertToList(Compiler *compiler);
        static void compileReplaceItemOfList(Compiler *compiler);
        static void compileItemOfList(Compiler *compiler);
        static void compileItemNumberInList(Compiler *compiler);
        static void compileLengthOfList(Compiler *compiler);
        static void compileListContainsItem(Compiler *compiler);

        static const std::string &listContentsMonitorName(Block *block);

    private:
        static int validateIndex(size_t index, size_t listLength);
};

} // namespace libscratchcpp
