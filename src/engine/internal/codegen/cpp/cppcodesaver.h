// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "test_export.h"

namespace libscratchcpp
{

class CppCodeGenerator;
class CppCompilerContext;

struct CppCodeSource
{
    std::string fileName;
    std::string source;
    std::string header;
};

class LIBSCRATCHCPP_TEST_EXPORT CppCodeSaver
{
    public:
        static CppCodeSource generate(CppCodeGenerator &codeGenerator);
        static void save(CppCodeGenerator &codeGenerator, CppCompilerContext *ctx, const std::string &destination);
        static void save(const CppCodeSource &code, CppCompilerContext *ctx, const std::string &destination);
};

} // namespace libscratchcpp
