// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/iengine.h>
#include <scratchcpp/target.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>

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

void CppCodeSaver::save(CppCodeGenerator &codeGenerator, CppCompilerContext *ctx, const std::string &destination)
{
    std::lock_guard<std::mutex> lock(outputMutex);

    if (destination.empty()) {
        std::cerr << "error: C++ output destination cannot be empty" << std::endl;
        return;
    }

    const std::filesystem::path outputDirectory = targetOutputDirectory(std::filesystem::path(destination), ctx);
    const std::string baseName = codeGenerator.generatedFileName();
    const std::filesystem::path sourcePath = outputDirectory / (baseName + ".cpp");
    const std::filesystem::path headerPath = outputDirectory / (baseName + ".h");
    std::error_code error;
    std::filesystem::create_directories(outputDirectory, error);

    if (error) {
        std::cerr << "error: could not create C++ output directory '" << outputDirectory.string() << "': " << error.message() << std::endl;
        return;
    }

    std::ofstream sourceFile(sourcePath, std::ios::trunc);
    std::ofstream headerFile(headerPath, std::ios::trunc);

    if (!sourceFile || !headerFile) {
        std::cerr << "error: could not write generated C++ files for '" << baseName << "'" << std::endl;
        return;
    }

    headerFile << "#pragma once\n\n#include \"cppexecutablecode.h\"\n\n";

    sourceFile << codeGenerator.finalize(headerPath.filename().string());
    headerFile << codeGenerator.generatedClassDeclaration() << "\n";
}
