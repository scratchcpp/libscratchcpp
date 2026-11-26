// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <engine/internal/engine.h>
#include <scratchcpp/block.h>
#include <scratchcpp/stage.h>
#include <extensionmock.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>

using namespace libscratchcpp;

class EngineSaveTest : public testing::Test
{
    protected:
        void SetUp() override
        {
            const auto name = "libscratchcpp-engine-save-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
            const auto directory = std::filesystem::temp_directory_path() / name;
            ASSERT_TRUE(std::filesystem::create_directory(directory));
            m_directory = directory;

            auto stage = std::make_shared<Stage>();
            stage->addBlock(std::make_shared<Block>("save-block", "save_test"));
            m_engine.setTargets({ stage });
            m_engine.addCompileFunction(&m_extension, "save_test", [](Compiler *) -> CompilerValue * { return nullptr; });
        }

        void TearDown() override
        {
            if (!m_directory.empty())
                std::filesystem::remove_all(m_directory);
        }

        std::filesystem::path m_directory;
        Engine m_engine;
        testing::NiceMock<ExtensionMock> m_extension;
};

TEST_F(EngineSaveTest, ExistingSourceFileSurvivesRejectedExport)
{
    const auto source = m_directory / "src" / "unrelated.cpp";
    std::filesystem::create_directory(source.parent_path());
    const std::string contents = "// User-owned source file\n";
    {
        std::ofstream file(source);
        file << contents;
        ASSERT_TRUE(file.good());
    }

    EXPECT_THROW(m_engine.save(m_directory.string()), std::runtime_error);

    std::ifstream file(source);
    ASSERT_TRUE(file.is_open());
    EXPECT_EQ(std::string(std::istreambuf_iterator<char>(file), {}), contents);
}

TEST_F(EngineSaveTest, RejectsExistingEmptyDirectory)
{
    EXPECT_THROW(m_engine.save(m_directory.string()), std::runtime_error);
    EXPECT_TRUE(std::filesystem::is_empty(m_directory));
}

TEST_F(EngineSaveTest, CreatesFreshDestinationAndRejectsReuse)
{
    const auto destination = m_directory / "export";
    EXPECT_NO_THROW(m_engine.save(destination.string()));
    ASSERT_TRUE(std::filesystem::is_directory(destination / "src/stage"));
    size_t sourceCount = 0;
    for (const auto &file : std::filesystem::directory_iterator(destination / "src/stage")) {
        if (file.path().extension() == ".cpp")
            ++sourceCount;
    }
    EXPECT_EQ(sourceCount, 1);
    EXPECT_THROW(m_engine.save(destination.string()), std::runtime_error);
}

TEST_F(EngineSaveTest, RejectsEmptyDestination)
{
    EXPECT_THROW(m_engine.save(""), std::invalid_argument);
}

TEST_F(EngineSaveTest, ThrowsWhenParentIsAFile)
{
    const auto parent = m_directory / "file";
    std::ofstream(parent) << "preserve me";
    EXPECT_THROW(m_engine.save((parent / "export").string()), std::filesystem::filesystem_error);
    EXPECT_TRUE(std::filesystem::is_regular_file(parent));
}

TEST_F(EngineSaveTest, ExportFailurePropagatesAndClearsExportState)
{
    class BlockingEngine : public Engine
    {
        public:
            void compile() override
            {
                if (blockOutput)
                    std::ofstream(destination / "src") << "blocked";
                Engine::compile();
            }

            std::filesystem::path destination;
            bool blockOutput = true;
    } engine;

    engine.destination = m_directory / "export";
    engine.setTargets(m_engine.targets());
    engine.addCompileFunction(&m_extension, "save_test", [](Compiler *) -> CompilerValue * { return nullptr; });

    EXPECT_THROW(engine.save(engine.destination.string()), std::filesystem::filesystem_error);
    engine.blockOutput = false;
    EXPECT_NO_THROW(engine.compile());
    EXPECT_NO_THROW(engine.save((m_directory / "retry").string()));
}
