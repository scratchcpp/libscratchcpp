// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/target.h>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <stdexcept>

#include "cppcodesaver.h"
#include "cppcodegenerator.h"
#include "cppcompilercontext.h"

using namespace libscratchcpp;

namespace
{

std::mutex outputMutex;

std::filesystem::path targetOutputDirectory(const std::filesystem::path &root, const CppCompilerContext *ctx)
{
    std::filesystem::path directory = root / "src";
    Target *target = ctx ? ctx->target() : nullptr;

    if (!target || target->isStage())
        return directory / "stage";

    size_t spriteIndex = 0;

    if (IEngine *engine = ctx->engine()) {
        for (const auto &engineTarget : engine->targets()) {
            if (engineTarget->isStage())
                continue;

            if (engineTarget.get() == target)
                return directory / "sprites" / ("sprite" + std::to_string(spriteIndex));

            ++spriteIndex;
        }
    }

    return directory / "sprites" / ("sprite" + std::to_string(spriteIndex));
}

} // namespace

CppCodeSource CppCodeSaver::generate(CppCodeGenerator &codeGenerator)
{
    std::lock_guard<std::mutex> lock(outputMutex);
    CppCodeSource code;
    code.fileName = codeGenerator.generatedFileName();
    code.source = codeGenerator.finalize(code.fileName + ".h");
    code.header = "#pragma once\n\n#include \"cppexecutablecode.h\"\n\n" + codeGenerator.generatedClassDeclaration() + "\n";
    return code;
}

void CppCodeSaver::save(CppCodeGenerator &codeGenerator, CppCompilerContext *ctx, const std::string &destination)
{
    save(generate(codeGenerator), ctx, destination);
}

void CppCodeSaver::save(const CppCodeSource &code, CppCompilerContext *ctx, const std::string &destination)
{
    std::lock_guard<std::mutex> lock(outputMutex);

    if (destination.empty())
        throw std::invalid_argument("C++ output destination cannot be empty");

    const std::filesystem::path outputDirectory = targetOutputDirectory(std::filesystem::path(destination), ctx);
    const std::string &baseName = code.fileName;
    const std::filesystem::path sourcePath = outputDirectory / (baseName + ".cpp");
    const std::filesystem::path headerPath = outputDirectory / (baseName + ".h");
    std::filesystem::create_directories(outputDirectory);

    std::ofstream sourceFile;
    std::ofstream headerFile;
    sourceFile.exceptions(std::ios::failbit | std::ios::badbit);
    headerFile.exceptions(std::ios::failbit | std::ios::badbit);
    sourceFile.open(sourcePath, std::ios::trunc);
    headerFile.open(headerPath, std::ios::trunc);

    sourceFile << code.source;
    headerFile << code.header;
    sourceFile.close();
    headerFile.close();
}
