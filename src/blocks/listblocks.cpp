// SPDX-License-Identifier: Apache-2.0

#include "listblocks.h"

using namespace libscratchcpp;

ListBlocks::ListBlocks()
{
    // Blocks
    addBlock("data_addtolist", &ListBlocks::addToList);
    addBlock("data_deleteoflist", &ListBlocks::deleteFromList);
    addBlock("data_deletealloflist", &ListBlocks::deleteAllOfList);
    addBlock("data_insertatlist", &ListBlocks::insertToList);
    addBlock("data_replaceitemoflist", &ListBlocks::replaceItemOfList);
    addBlock("data_itemoflist", &ListBlocks::itemOfList);
    addBlock("data_itemnumoflist", &ListBlocks::itemNumberInList);
    addBlock("data_lengthoflist", &ListBlocks::lengthOfList);
    addBlock("data_listcontainsitem", &ListBlocks::listContainsItem);

    // Inputs
    addInput("ITEM", ITEM);
    addInput("INDEX", INDEX);

    // Fields
    addField("LIST", LIST);
}

std::string ListBlocks::name() const
{
    return "Lists";
}

bool ListBlocks::categoryVisible() const
{
    return false;
}

Value ListBlocks::addToList(const BlockArgs &args)
{
    auto list = std::static_pointer_cast<List>(args.field(LIST)->valuePtr());
    list->append(args.input(ITEM)->value());
    return Value();
}

Value ListBlocks::deleteFromList(const BlockArgs &args)
{
    auto list = std::static_pointer_cast<List>(args.field(LIST)->valuePtr());
    Value value = args.input(INDEX)->value();
    int index;
    if (value.isString()) {
        std::string str = value.toString();
        if (str == "last")
            index = list->length();
        else if (str == "all") {
            list->clear();
            return Value();
        } else if (str == "random") {
            index = list->length() == 0 ? 0 : randint(1, list->length());
        } else
            return Value();
    } else
        index = validateIndex(value.toNumber(), list->length());
    if (index == 0)
        return Value();
    list->removeAt(index - 1);
    return Value();
}

Value ListBlocks::deleteAllOfList(const BlockArgs &args)
{
    auto list = std::static_pointer_cast<List>(args.field(LIST)->valuePtr());
    list->clear();
    return Value();
}

Value ListBlocks::insertToList(const BlockArgs &args)
{
    auto list = std::static_pointer_cast<List>(args.field(LIST)->valuePtr());
    Value value = args.input(INDEX)->value();
    int index;
    if (value.isString()) {
        std::string str = value.toString();
        if (str == "last") {
            list->append(args.input(ITEM)->value());
            return Value();
        } else if (str == "random") {
            index = list->length() == 0 ? 1 : randint(1, list->length());
        } else
            return Value();
    } else {
        index = validateIndex(value.toNumber(), list->length());
        if (index == 0)
            return Value();
    }
    list->insert(index - 1, args.input(ITEM)->value());
    return Value();
}

Value ListBlocks::replaceItemOfList(const BlockArgs &args)
{
    auto list = std::static_pointer_cast<List>(args.field(LIST)->valuePtr());
    Value value = args.input(INDEX)->value();
    int index;
    if (value.isString()) {
        std::string str = value.toString();
        if (str == "last")
            index = list->length();
        else if (str == "random") {
            index = list->length() == 0 ? 0 : randint(1, list->length());
        } else
            return Value();
    } else
        index = validateIndex(value.toNumber(), list->length());
    if (index == 0)
        return Value();
    list->replace(index - 1, args.input(ITEM)->value());
    return Value();
}

Value ListBlocks::itemOfList(const BlockArgs &args)
{
    auto list = std::static_pointer_cast<List>(args.field(LIST)->valuePtr());
    Value value = args.input(INDEX)->value();
    int index;
    if (value.isString()) {
        std::string str = value.toString();
        if (str == "last")
            index = list->length();
        else if (str == "random") {
            index = list->length() == 0 ? 0 : randint(1, list->length());
        } else
            return "";
    } else
        index = validateIndex(value.toNumber(), list->length());
    if (index == 0)
        return "";
    return list->at(index - 1);
}

Value ListBlocks::itemNumberInList(const BlockArgs &args)
{
    auto list = std::static_pointer_cast<List>(args.field(LIST)->valuePtr());
    return list->indexOf(args.input(ITEM)->value()) + 1;
}

Value ListBlocks::lengthOfList(const BlockArgs &args)
{
    auto list = std::static_pointer_cast<List>(args.field(LIST)->valuePtr());
    return list->length();
}

Value ListBlocks::listContainsItem(const BlockArgs &args)
{
    auto list = std::static_pointer_cast<List>(args.field(LIST)->valuePtr());
    return list->contains(args.input(ITEM)->value());
}

int ListBlocks::validateIndex(int index, int listLength)
{
    if (listLength == 0) {
        if (index != 1)
            return 0;
    } else if ((index < 1) || (index > listLength))
        return 0;
    return index;
}
