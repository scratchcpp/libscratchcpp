// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <engine/internal/engine.h>
#include <scratchcpp/block.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/script.h>
#include <scratchcpp/monitor.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/compilerconstant.h>
#include <extensionmock.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
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
            m_engine.compile();
        }

        void TearDown() override
        {
            if (!m_directory.empty())
                std::filesystem::remove_all(m_directory);
        }

        size_t sourceCount(const std::filesystem::path &directory)
        {
            size_t count = 0;
            for (const auto &entry : std::filesystem::recursive_directory_iterator(directory)) {
                if (entry.path().extension() == ".cpp")
                    ++count;
            }
            return count;
        }

        void checkRemovedSpriteExport(bool destroyRemoved)
        {
            auto stage = m_engine.targets().front();
            auto removed = std::make_shared<Sprite>();
            auto remaining = std::make_shared<Sprite>();
            removed->addBlock(std::make_shared<Block>("removed-script", "save_green_flag"));
            remaining->addBlock(std::make_shared<Block>("remaining-script", "save_green_flag"));
            m_engine.addCompileFunction(&m_extension, "save_green_flag", [](Compiler *compiler) -> CompilerValue * {
                compiler->engine()->addGreenFlagScript(compiler->block());
                return nullptr;
            });
            m_engine.setTargets({ stage, removed, remaining });
            m_engine.compile();

            std::weak_ptr<Sprite> removedWeak = removed;
            m_engine.setTargets({ stage, remaining });
            if (destroyRemoved) {
                removed.reset();
                ASSERT_TRUE(removedWeak.expired());
            }
            m_engine.compile();

            const auto destination = m_directory / "export";
            ASSERT_NO_THROW(m_engine.save(destination.string()));
            EXPECT_EQ(sourceCount(destination), 2);
            ASSERT_TRUE(std::filesystem::is_directory(destination / "src/stage"));
            EXPECT_EQ(sourceCount(destination / "src/stage"), 1);
            ASSERT_TRUE(std::filesystem::is_directory(destination / "src/sprites/sprite0"));
            EXPECT_EQ(sourceCount(destination / "src/sprites/sprite0"), 1);
            EXPECT_FALSE(std::filesystem::exists(destination / "src/sprites/sprite1"));
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

TEST_F(EngineSaveTest, RepeatedExportsUseRetainedArtifacts)
{
    auto *script = m_engine.scripts().begin()->second.get();
    m_engine.addCompileFunction(&m_extension, "save_test", [](Compiler *) -> CompilerValue * {
        throw std::runtime_error("export must not compile blocks");
    });

    const auto first = m_directory / "first";
    const auto second = m_directory / "second";
    ASSERT_NO_THROW(m_engine.save(first.string()));
    ASSERT_NO_THROW(m_engine.save(second.string()));
    EXPECT_EQ(m_engine.scripts().begin()->second.get(), script);

    const auto readFiles = [](const std::filesystem::path &directory) {
        std::map<std::string, std::string> files;
        for (const auto &entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (!entry.is_regular_file())
                continue;
            std::ifstream file(entry.path());
            files[std::filesystem::relative(entry.path(), directory).generic_string()] = std::string(std::istreambuf_iterator<char>(file), {});
        }
        return files;
    };
    const auto files = readFiles(first);
    EXPECT_EQ(files.size(), 2);
    EXPECT_EQ(readFiles(second), files);
}

TEST_F(EngineSaveTest, RetainsPredicateAndMonitorArtifacts)
{
    m_engine.addHatPredicateCompileFunction(&m_extension, "save_test", [](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(true); });
    m_engine.addCompileFunction(&m_extension, "save_monitor", [](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(42); });
    auto monitor = std::make_shared<Monitor>("monitor", "save_monitor");
    m_engine.setMonitors({ monitor });
    m_engine.compile();
    auto *script = m_engine.scripts().begin()->second.get();
    auto *predicate = script->hatPredicateCode();
    auto *reporter = monitor->script()->code();
    ASSERT_NE(predicate, nullptr);
    ASSERT_NE(reporter, nullptr);

    const auto destination = m_directory / "export";
    ASSERT_NO_THROW(m_engine.save(destination.string()));
    EXPECT_EQ(m_engine.scripts().begin()->second.get(), script);
    EXPECT_EQ(script->hatPredicateCode(), predicate);
    EXPECT_EQ(monitor->script()->code(), reporter);
    size_t sourceCount = 0;
    for (const auto &file : std::filesystem::recursive_directory_iterator(destination)) {
        if (file.path().extension() == ".cpp")
            ++sourceCount;
    }
    EXPECT_EQ(sourceCount, 3);
}

TEST_F(EngineSaveTest, ExportExcludesRemovedSprite)
{
    checkRemovedSpriteExport(false);
}

TEST_F(EngineSaveTest, ExportExcludesDestroyedSprite)
{
    checkRemovedSpriteExport(true);
}

TEST_F(EngineSaveTest, ExportExcludesBlocksThatAreNoLongerEligible)
{
    m_engine.targets().front()->blocks().front()->setShadow(true);
    m_engine.compile();

    const auto destination = m_directory / "export";
    ASSERT_NO_THROW(m_engine.save(destination.string()));
    EXPECT_EQ(sourceCount(destination), 0);
}

TEST_F(EngineSaveTest, ExportExcludesMonitorsForRemovedSprites)
{
    auto stage = m_engine.targets().front();
    auto removed = std::make_shared<Sprite>();
    m_engine.setTargets({ stage, removed });
    m_engine.addCompileFunction(&m_extension, "save_monitor", [](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(42); });
    auto monitor = std::make_shared<Monitor>("removed-monitor", "save_monitor");
    monitor->setSprite(removed.get());
    m_engine.setMonitors({ monitor });
    m_engine.compile();

    m_engine.setTargets({ stage });
    m_engine.compile();
    std::weak_ptr<Sprite> removedWeak = removed;
    removed.reset();
    ASSERT_TRUE(removedWeak.expired());

    const auto destination = m_directory / "export";
    ASSERT_NO_THROW(m_engine.save(destination.string()));
    EXPECT_EQ(sourceCount(destination), 1);
    EXPECT_FALSE(std::filesystem::exists(destination / "src/sprites"));
}

TEST_F(EngineSaveTest, ExportExcludesCachedMonitorsWithoutRecompiling)
{
    auto stage = m_engine.targets().front();
    auto removed = std::make_shared<Sprite>();
    m_engine.setTargets({ stage, removed });
    m_engine.addCompileFunction(&m_extension, "save_monitor", [](Compiler *compiler) -> CompilerValue * { return compiler->addConstValue(42); });
    auto monitor = std::make_shared<Monitor>("cached-monitor", "save_monitor");
    monitor->setSprite(removed.get());
    m_engine.setMonitors({ monitor });
    m_engine.compile();

    auto *cachedScript = monitor->script().get();
    ASSERT_NE(cachedScript, nullptr);
    auto *cachedCode = cachedScript->code();
    ASSERT_NE(cachedCode, nullptr);
    ASSERT_EQ(cachedScript->target(), removed.get());

    m_engine.setTargets({ stage });
    std::weak_ptr<Sprite> removedWeak = removed;
    removed.reset();
    ASSERT_TRUE(removedWeak.expired());

    // Export with the original monitor script still cached for the destroyed sprite.
    const auto destination = m_directory / "export";
    ASSERT_NO_THROW(m_engine.save(destination.string()));
    EXPECT_EQ(sourceCount(destination), 1);
    EXPECT_FALSE(std::filesystem::exists(destination / "src/sprites"));
    ASSERT_EQ(m_engine.monitors().size(), 1);
    EXPECT_EQ(m_engine.monitors().front(), monitor);
    ASSERT_EQ(monitor->script().get(), cachedScript);
    EXPECT_EQ(monitor->script()->code(), cachedCode);
}
