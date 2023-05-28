#pragma once

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

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
