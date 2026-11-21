#include <scratchcpp/project.h>
#include <scratchcpp/block.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/compilercontext.h>
#include <enginemock.h>
#include <projectdownloaderfactorymock.h>
#include <projectdownloadermock.h>
#include <extensionmock.h>
#include <codebuilderfactorymock.h>
#include <codebuildermock.h>
#include <engine/compiler_p.h>
#include <engine/internal/engine.h>
#include <filesystem>

#include "project_p.h"
#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::ReturnRef;

class ProjectTest : public testing::Test
{
    public:
        void SetUp() override { m_engine = std::make_shared<EngineMock>(); }

        std::shared_ptr<EngineMock> m_engine;
};

TEST_F(ProjectTest, Constructors)
{
    // Constructor 1
    Project p1;
    ASSERT_EQ(p1.fileName(), "");
    ASSERT_TRUE(p1.engine());

    // Constructor 2
    Project p2("nonexistent.sb3");
    ASSERT_EQ(p2.fileName(), "nonexistent.sb3");
    ASSERT_TRUE(p2.engine());

    Project p3("default_project.sb3");
    ASSERT_EQ(p3.fileName(), "default_project.sb3");
    ASSERT_TRUE(p3.engine());
}

TEST_F(ProjectTest, Load)
{
    // Constructor 1
    ProjectPrivate p1;
    p1.engine = m_engine;
    EXPECT_CALL(*m_engine, clear).Times(0);
    EXPECT_CALL(*m_engine, setTargets).Times(0);
    EXPECT_CALL(*m_engine, setBroadcasts).Times(0);
    EXPECT_CALL(*m_engine, setMonitors).Times(0);
    EXPECT_CALL(*m_engine, setExtensions).Times(0);
    EXPECT_CALL(*m_engine, compile).Times(0);
    ASSERT_FALSE(p1.load());

    // Constructor 2
    ProjectPrivate p2("nonexistent.sb3");
    p2.engine = m_engine;
    EXPECT_CALL(*m_engine, clear).Times(0);
    EXPECT_CALL(*m_engine, setTargets).Times(0);
    EXPECT_CALL(*m_engine, setBroadcasts).Times(0);
    EXPECT_CALL(*m_engine, setMonitors).Times(0);
    EXPECT_CALL(*m_engine, setExtensions).Times(0);
    EXPECT_CALL(*m_engine, compile).Times(0);
    ASSERT_FALSE(p2.load());

    ProjectPrivate p3("default_project.sb3");
    p3.engine = m_engine;
    EXPECT_CALL(*m_engine, clear).Times(1);
    EXPECT_CALL(*m_engine, setTargets).Times(1);
    EXPECT_CALL(*m_engine, setBroadcasts).Times(1);
    EXPECT_CALL(*m_engine, setMonitors).Times(1);
    EXPECT_CALL(*m_engine, setExtensions).Times(1);
    EXPECT_CALL(*m_engine, compile).Times(1);
    ASSERT_TRUE(p3.load());

    testing::Mock::AllowLeak(m_engine.get());
}

TEST_F(ProjectTest, SaveDelegatesThroughEngineAndCodeBuilder)
{
    Project project;
    auto engine = std::dynamic_pointer_cast<Engine>(project.engine());
    ASSERT_TRUE(engine);

    auto target = std::make_shared<Sprite>();
    auto stage = std::make_shared<Stage>();
    auto block = std::make_shared<Block>("save-script", "save_test");
    target->addBlock(block);
    engine->setTargets({ stage, target });

    ExtensionMock extension;
    engine->addCompileFunction(&extension, "save_test", [](Compiler *) -> CompilerValue * { return nullptr; });

    CodeBuilderFactoryMock builderFactory;
    auto stageCompilerContext = std::make_shared<CompilerContext>(engine.get(), stage.get());
    auto targetCompilerContext = std::make_shared<CompilerContext>(engine.get(), target.get());
    auto codeBuilder = std::make_shared<CodeBuilderMock>();
    const std::filesystem::path destination = std::filesystem::temp_directory_path() / "libscratchcpp-project-save-test";
    CompilerPrivate::setBuilderFactory(&builderFactory);

    EXPECT_CALL(builderFactory, createCtx(engine.get(), stage.get(), testing::_)).WillOnce(testing::Return(stageCompilerContext));
    EXPECT_CALL(builderFactory, createCtx(engine.get(), target.get(), testing::_)).WillOnce(testing::Return(targetCompilerContext));
    EXPECT_CALL(builderFactory, create(targetCompilerContext.get(), nullptr, Compiler::CodeType::Script)).WillOnce(testing::Return(codeBuilder));
    EXPECT_CALL(*codeBuilder, build()).WillOnce(testing::Return(std::shared_ptr<ExecutableCode>()));
    EXPECT_CALL(*codeBuilder, save(destination.string())).Times(1);

    project.save(destination.string());

    CompilerPrivate::setBuilderFactory(nullptr);
    std::filesystem::remove_all(destination);
}

TEST_F(ProjectTest, Start)
{
    ProjectPrivate p("default_project.sb3");
    p.engine = m_engine;
    EXPECT_CALL(*m_engine, start).Times(1);
    EXPECT_CALL(*m_engine, run).Times(0);
    p.start();
}

TEST_F(ProjectTest, Run)
{
    ProjectPrivate p("default_project.sb3");
    p.engine = m_engine;
    EXPECT_CALL(*m_engine, start).Times(0);
    EXPECT_CALL(*m_engine, run).Times(1);
    p.run();
}

TEST_F(ProjectTest, FileName)
{
    Project p;
    p.setFileName("test.sb3");
    ASSERT_EQ(p.fileName(), "test.sb3");

    p.setFileName("default_project.sb3");
    ASSERT_EQ(p.fileName(), "default_project.sb3");
}

TEST(LoadProjectTest, DownloadProgressChanged)
{
    ProjectDownloaderFactoryMock factory;
    auto downloader = std::make_shared<ProjectDownloaderMock>();
    ProjectPrivate::setDownloaderFactory(&factory);

    EXPECT_CALL(factory, create()).WillOnce(Return(downloader));
    ProjectPrivate p;
    ProjectPrivate::setDownloaderFactory(nullptr);

    sigslot::signal<unsigned int, unsigned int> signal;
    EXPECT_CALL(*downloader, downloadProgressChanged).WillOnce(ReturnRef(signal));
    ASSERT_EQ(&p.downloadProgressChanged(), &signal);
}

TEST(LoadProjectTest, AbortDownload)
{
    ProjectDownloaderFactoryMock factory;
    auto downloader = std::make_shared<ProjectDownloaderMock>();
    ProjectPrivate::setDownloaderFactory(&factory);

    EXPECT_CALL(factory, create()).WillOnce(Return(downloader));
    Project p;
    ProjectPrivate::setDownloaderFactory(nullptr);

    EXPECT_CALL(*downloader, cancel());
    p.stopLoading();
}
