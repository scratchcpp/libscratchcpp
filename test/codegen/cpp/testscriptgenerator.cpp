// SPDX-License-Identifier: Apache-2.0

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "testcodegenlistener.h"

namespace
{

constexpr size_t SCRIPT_CHUNK_SIZE = 100;
constexpr size_t SCRIPT_CHUNK_COUNT = 64;

bool writeGeneratedFactory(const std::filesystem::path &destination)
{
    std::map<std::string, std::pair<std::string, std::string>> scripts;
    // Analyzer-only selections do not create any executable scripts or manifest.
    std::ifstream manifest(destination / "test_scripts.txt");
    if (!manifest && std::filesystem::exists(destination / "test_scripts.txt"))
        return false;
    std::string id;
    std::string className;
    std::string sourcePath;

    while (manifest >> id >> className >> sourcePath) {
        if (!scripts.emplace(id, std::make_pair(className, sourcePath)).second) {
            std::cerr << "error: duplicate C++ test script ID: " << id << std::endl;
            return false;
        }
    }

    if (scripts.size() > SCRIPT_CHUNK_SIZE * SCRIPT_CHUNK_COUNT) {
        std::cerr << "error: invalid generated C++ test script count: " << scripts.size() << std::endl;
        return false;
    }

    std::ofstream header(destination / "generated_cpp_test_code.h");
    header << "#pragma once\n\n"
              "#include <memory>\n#include <string>\n"
              "#include <scratchcpp/executablecode.h>\n\n"
              "namespace libscratchcpp\n{\n"
              "class CppCompilerContext;\n"
              "std::shared_ptr<ExecutableCode> createGeneratedCppTestCode(const std::string &id, CppCompilerContext *ctx);\n";

    for (size_t chunk = 0; chunk < SCRIPT_CHUNK_COUNT; ++chunk)
        header << "std::shared_ptr<ExecutableCode> createGeneratedCppTestCodeChunk" << chunk << "(const std::string &id, CppCompilerContext *ctx);\n";

    header << "}\n";

    std::ofstream source(destination / "generated_cpp_test_code.cpp");
    source << "#include \"generated_cpp_test_code.h\"\n"
              "\nnamespace libscratchcpp\n{\n\n"
              "std::shared_ptr<ExecutableCode> createGeneratedCppTestCode(const std::string &id, CppCompilerContext *ctx)\n{\n";

    for (size_t chunk = 0; chunk < SCRIPT_CHUNK_COUNT; ++chunk)
        source << "    if (auto code = createGeneratedCppTestCodeChunk" << chunk << "(id, ctx))\n        return code;\n";

    source << "    return nullptr;\n}\n\n} // namespace libscratchcpp\n";

    auto script = scripts.begin();

    for (size_t chunk = 0; chunk < SCRIPT_CHUNK_COUNT; ++chunk) {
        std::ofstream chunkSource(destination / ("generated_cpp_test_code_" + std::to_string(chunk) + ".cpp"));
        chunkSource << "#include \"testfunctions.h\"\n"
                       "#include \"generated_cpp_test_code.h\"\n";

        std::vector<std::pair<std::string, std::string>> chunkScripts;
        for (size_t i = 0; i < SCRIPT_CHUNK_SIZE && script != scripts.end(); ++i, ++script) {
            chunkSource << "#include \"" << script->second.second << "\"\n";
            chunkScripts.emplace_back(script->first, script->second.first);
        }

        chunkSource << "\nnamespace libscratchcpp\n{\n\n"
                       "std::shared_ptr<ExecutableCode> createGeneratedCppTestCodeChunk" << chunk << "(const std::string &id, CppCompilerContext *ctx)\n{\n";

        for (const auto &entry : chunkScripts)
            chunkSource << "    if (id == \"" << entry.first << "\")\n        return std::make_shared<" << entry.second << ">(ctx);\n";

        chunkSource << "    return nullptr;\n}\n\n} // namespace libscratchcpp\n";
        chunkSource.close();
        if (!chunkSource)
            return false;
    }

    header.close();
    source.close();
    return header.good() && source.good();
}

} // namespace

int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;

    const std::filesystem::path destination = std::filesystem::absolute(argv[1]);
    std::error_code error;
    std::filesystem::remove_all(destination, error);

    if (error)
        return 1;

    std::filesystem::create_directories(destination, error);

    if (error)
        return 1;

    libscratchcpp::CppCompilerContext::setTestGenerationDirectory(destination.string());
    std::filesystem::current_path(DATA_DIR);

    int gtestArgc = 1;
    char *gtestArgv[] = { argv[0], nullptr };
    ::testing::InitGoogleTest(&gtestArgc, gtestArgv);
    ::testing::GTEST_FLAG(brief) = true;
    delete ::testing::UnitTest::GetInstance()->listeners().Release(::testing::UnitTest::GetInstance()->listeners().default_result_printer());
    auto *listener = new libscratchcpp::test::CppTestCodegenListener();
    listener->registerProvider();
    ::testing::UnitTest::GetInstance()->listeners().Append(listener);

    std::string testFilter = "CodeBuilderTest.*";

    for (int i = 0; i < ::testing::UnitTest::GetInstance()->total_test_suite_count(); ++i) {
        const std::string suiteName = ::testing::UnitTest::GetInstance()->GetTestSuite(i)->name();

        if (suiteName.size() > 4 && suiteName.substr(suiteName.size() - 4) == "Test")
            testFilter += ":" + suiteName + ".*";
    }

    ::testing::GTEST_FLAG(filter) = testFilter;
    // This pass records builders using placeholder executable code. Execution
    // assertions are checked by codegen_test after the generated C++ is compiled.
    const int testResult = RUN_ALL_TESTS();
    (void)testResult;

    return writeGeneratedFactory(destination) ? 0 : 1;
}
