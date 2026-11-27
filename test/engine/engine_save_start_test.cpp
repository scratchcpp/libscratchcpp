// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <engine/internal/engine.h>
#include <scratchcpp/project.h>
#include <scratchcpp/script.h>
#include <zip.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>

using namespace libscratchcpp;

class EngineExportTest : public testing::Test
{
    protected:
        void SetUp() override
        {
            const auto name = "libscratchcpp-export-start-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
            const auto directory = std::filesystem::temp_directory_path() / name;
            ASSERT_TRUE(std::filesystem::create_directory(directory));
            m_directory = directory;
        }

        void TearDown() override
        {
            if (!m_directory.empty())
                std::filesystem::remove_all(m_directory);
        }

        std::filesystem::path m_directory;
};

TEST_F(EngineExportTest, LoadSaveStartPreservesGreenFlagScript)
{
#ifndef LIBSCRATCHCPP_TEST_EVENT_BLOCKS
    GTEST_SKIP() << "Event blocks are disabled";
#else
    const auto projectFile = m_directory / "green-flag.sb3";
    const std::string json = R"({"targets":[{"isStage":true,"name":"Stage","variables":{},"lists":{},"broadcasts":{},
            "blocks":{"hat":{"opcode":"event_whenflagclicked","next":null,"parent":null,"inputs":{},"fields":{},"shadow":false,"topLevel":true,"x":0,"y":0}},
            "comments":{},"currentCostume":0,"costumes":[],"sounds":[],"volume":100,"layerOrder":0,
            "tempo":60,"videoTransparency":50,"videoState":"on","textToSpeechLanguage":null}],
            "monitors":[],"extensions":[],"meta":{"semver":"3.0.0","vm":"test","agent":"test"}})";
    std::unique_ptr<zip_t, decltype(&zip_close)> archive(zip_open(projectFile.string().c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'w'), zip_close);
    ASSERT_NE(archive, nullptr);
    ASSERT_EQ(zip_entry_open(archive.get(), "project.json"), 0);
    ASSERT_EQ(zip_entry_write(archive.get(), json.data(), json.size()), 0);
    ASSERT_EQ(zip_entry_close(archive.get()), 0);
    archive.reset();

    Project project(projectFile.string());
    ASSERT_TRUE(project.load());
    auto *engine = dynamic_cast<Engine *>(project.engine().get());
    ASSERT_NE(engine, nullptr);
    ASSERT_EQ(engine->scripts().size(), 1);
    ASSERT_FALSE(engine->isRunning());
    std::weak_ptr<Script> original = engine->scripts().begin()->second;
    auto *code = engine->scripts().begin()->second->code();

    const auto destination = m_directory / "export";
#ifdef LIBSCRATCHCPP_TEST_CPP_EXPORT
    ASSERT_NO_THROW(project.save(destination.string()));
    EXPECT_TRUE(std::filesystem::is_directory(destination / "src/stage"));
#else
    EXPECT_THROW(project.save(destination.string()), std::runtime_error);
    EXPECT_FALSE(std::filesystem::exists(destination));
#endif
    ASSERT_FALSE(original.expired());
    EXPECT_EQ(engine->scripts().begin()->second, original.lock());
    EXPECT_EQ(engine->scripts().begin()->second->code(), code);
    EXPECT_FALSE(engine->isRunning());

    int stoppedThreads = 0;
    engine->threadAboutToStop().connect([&stoppedThreads](Thread *) { ++stoppedThreads; });
    for (int i = 0; i < 2; ++i) {
        project.start();
        EXPECT_TRUE(engine->isRunning());
        engine->stop();
        EXPECT_FALSE(engine->isRunning());
        EXPECT_EQ(stoppedThreads, i + 1);
    }
#endif
}

#ifndef LIBSCRATCHCPP_TEST_CPP_EXPORT
TEST_F(EngineExportTest, UnsupportedBackendDoesNotCompile)
{
    class CompileSpyEngine : public Engine
    {
        public:
            void compile() override { ++compileCalls; }
            int compileCalls = 0;
    } engine;

    const auto destination = m_directory / "export";
    EXPECT_THROW(engine.save(destination.string()), std::runtime_error);
    EXPECT_EQ(engine.compileCalls, 0);
    EXPECT_FALSE(std::filesystem::exists(destination));
}
#endif
