// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "test_export.h"

namespace libscratchcpp
{

class CppCodeGenerator;
class CppCompilerContext;

class LIBSCRATCHCPP_TEST_EXPORT CppCodeSaver
{
    public:
        static void save(CppCodeGenerator &codeGenerator, CppCompilerContext *ctx, const std::string &destination);
};

} // namespace libscratchcpp
