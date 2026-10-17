// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "../../../test_export.h"

namespace libscratchcpp
{

/*! Generates a standalone C++ codebase (CMake project) from a Scratch project. */
class LIBSCRATCHCPP_TEST_EXPORT CppCodebaseGenerator
{
    public:
        static bool generate(const std::string &projectFile, const std::string &outputDir, const std::string &projectName = "project");

        static std::string mainSource();
        static std::string dataSource(const std::string &data);
        static std::string cmakeSource(const std::string &projectName);
};

} // namespace libscratchcpp
