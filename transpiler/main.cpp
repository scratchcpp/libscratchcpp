// SPDX-License-Identifier: Apache-2.0

// Converts a Scratch project to a standalone C++ codebase

#include <iostream>
#include <string>

extern "C" bool cpp_generate_codebase(const char *projectFile, const char *outputDir, const char *projectName);

int main(int argc, char **argv)
{
    if (argc < 3 || argc > 4) {
        std::cerr << "usage: " << argv[0] << " <project.sb3> <output directory> [project name]" << std::endl;
        return 1;
    }

    std::string name = argc == 4 ? argv[3] : "project";
    return cpp_generate_codebase(argv[1], argv[2], name.c_str()) ? 0 : 1;
}
