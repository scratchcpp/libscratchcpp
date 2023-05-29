#pragma once

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

#define ASSERT_VAR(target, varName)                                                                                                                                                                    \
    ASSERT_NE(target->findVariable(varName), -1);                                                                                                                                                      \
    ASSERT_TRUE(target->variableAt(target->findVariable(varName)))
#define GET_VAR(target, varName) target->variableAt(target->findVariable(varName))

#define ASSERT_LIST(target, listName)                                                                                                                                                                  \
    ASSERT_NE(target->findList(listName), -1);                                                                                                                                                         \
    ASSERT_TRUE(target->listAt(target->findList(listName)))
#define GET_LIST(target, listName) target->listAt(target->findList(listName))

#define ASSERT_INPUT(block, inputName)                                                                                                                                                                 \
    ASSERT_NE(block->findInput(inputName), -1);                                                                                                                                                        \
    ASSERT_TRUE(block->inputAt(block->findInput(inputName)))
#define GET_INPUT(block, inputName) block->inputAt(block->findInput(inputName))

#define ASSERT_FIELD(block, fieldName)                                                                                                                                                                 \
    ASSERT_NE(block->findField(fieldName), -1);                                                                                                                                                        \
    ASSERT_TRUE(block->fieldAt(block->findField(fieldName)))
#define GET_FIELD(block, fieldName) block->fieldAt(block->findField(fieldName))

std::string readFileStr(const std::string &fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cout << "Failed to open " + fileName << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char **argv)
{
    std::filesystem::current_path(DATA_DIR);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
