#include <scratchcpp/project.h>
#include <enginemock.h>
#include <projectdownloaderfactorymock.h>
#include <projectdownloadermock.h>

#include "project_p.h"
#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;

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
    ASSERT_EQ(p1.scratchVersion(), ScratchVersion::Invalid);
    ASSERT_TRUE(p1.engine());

    // Constructor 2
    Project p2("nonexistent.sb3");
    ASSERT_EQ(p2.fileName(), "nonexistent.sb3");
    ASSERT_EQ(p2.scratchVersion(), ScratchVersion::Invalid);
    ASSERT_TRUE(p2.engine());

    Project p3("default_project.sb3");
    ASSERT_EQ(p3.fileName(), "default_project.sb3");
    ASSERT_EQ(p3.scratchVersion(), ScratchVersion::Scratch3);
    ASSERT_TRUE(p3.engine());

    // Constructor 3
    Project p4("nonexistent.sb3", ScratchVersion::Scratch3);
    ASSERT_EQ(p4.fileName(), "nonexistent.sb3");
    ASSERT_EQ(p4.scratchVersion(), ScratchVersion::Scratch3);
    ASSERT_TRUE(p4.engine());

    Project p5("default_project.sb3", ScratchVersion::Scratch3);
    ASSERT_EQ(p5.fileName(), "default_project.sb3");
    ASSERT_EQ(p5.scratchVersion(), ScratchVersion::Scratch3);
    ASSERT_TRUE(p5.engine());

    Project p6("default_project.sb3", ScratchVersion::Invalid);
    ASSERT_EQ(p6.fileName(), "default_project.sb3");
    ASSERT_EQ(p6.scratchVersion(), ScratchVersion::Invalid);
    ASSERT_TRUE(p6.engine());
}

TEST_F(ProjectTest, Load)
{
    // Constructor 1
    ProjectPrivate p1;
    p1.engine = m_engine;
    EXPECT_CALL(*m_engine, clear).Times(0);
    EXPECT_CALL(*m_engine, setTargets).Times(0);
    EXPECT_CALL(*m_engine, setBroadcasts).Times(0);
    EXPECT_CALL(*m_engine, setExtensions).Times(0);
    EXPECT_CALL(*m_engine, compile).Times(0);
    ASSERT_FALSE(p1.load());

    // Constructor 2
    ProjectPrivate p2("nonexistent.sb3");
    p2.engine = m_engine;
    EXPECT_CALL(*m_engine, clear).Times(0);
    EXPECT_CALL(*m_engine, setTargets).Times(0);
    EXPECT_CALL(*m_engine, setBroadcasts).Times(0);
    EXPECT_CALL(*m_engine, setExtensions).Times(0);
    EXPECT_CALL(*m_engine, compile).Times(0);
    ASSERT_FALSE(p2.load());

    ProjectPrivate p3("default_project.sb3");
    p3.engine = m_engine;
    EXPECT_CALL(*m_engine, clear).Times(1);
    EXPECT_CALL(*m_engine, setTargets).Times(1);
    EXPECT_CALL(*m_engine, setBroadcasts).Times(1);
    EXPECT_CALL(*m_engine, setExtensions).Times(1);
    EXPECT_CALL(*m_engine, compile).Times(1);
    ASSERT_TRUE(p3.load());

    // Constructor 3
    ProjectPrivate p4("nonexistent.sb3", ScratchVersion::Scratch3);
    p4.engine = m_engine;
    EXPECT_CALL(*m_engine, clear).Times(0);
    EXPECT_CALL(*m_engine, setTargets).Times(0);
    EXPECT_CALL(*m_engine, setBroadcasts).Times(0);
    EXPECT_CALL(*m_engine, setExtensions).Times(0);
    EXPECT_CALL(*m_engine, compile).Times(0);
    ASSERT_FALSE(p4.load());

    ProjectPrivate p5("default_project.sb3", ScratchVersion::Scratch3);
    p5.engine = m_engine;
    EXPECT_CALL(*m_engine, clear).Times(1);
    EXPECT_CALL(*m_engine, setTargets).Times(1);
    EXPECT_CALL(*m_engine, setBroadcasts).Times(1);
    EXPECT_CALL(*m_engine, setExtensions).Times(1);
    EXPECT_CALL(*m_engine, compile).Times(1);
    ASSERT_TRUE(p5.load());

    ProjectPrivate p6("default_project.sb3", ScratchVersion::Invalid);
    p6.engine = m_engine;
    EXPECT_CALL(*m_engine, clear).Times(0);
    EXPECT_CALL(*m_engine, setTargets).Times(0);
    EXPECT_CALL(*m_engine, setBroadcasts).Times(0);
    EXPECT_CALL(*m_engine, setExtensions).Times(0);
    EXPECT_CALL(*m_engine, compile).Times(0);
    ASSERT_FALSE(p6.load());

    testing::Mock::AllowLeak(m_engine.get());
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
    ASSERT_EQ(p.scratchVersion(), ScratchVersion::Invalid);

    p.setFileName("default_project.sb3");
    ASSERT_EQ(p.scratchVersion(), ScratchVersion::Scratch3);
}

TEST_F(ProjectTest, ScratchVersion)
{
    Project p;
    p.setScratchVersion(ScratchVersion::Scratch3);
    ASSERT_EQ(p.scratchVersion(), ScratchVersion::Scratch3);
}

TEST(LoadProjectTest, DownloadProgressCallback)
{
    ProjectDownloaderFactoryMock factory;
    auto downloader = std::make_shared<ProjectDownloaderMock>();
    ProjectPrivate::downloaderFactory = &factory;

    EXPECT_CALL(factory, create()).WillOnce(Return(downloader));
    ProjectPrivate p;
    ProjectPrivate::downloaderFactory = nullptr;

    auto lambda = [](unsigned int, unsigned int) {};
    // TODO: Check the function parameter, if possible (std::function doesn't have operator== for this)
    EXPECT_CALL(*downloader, setDownloadProgressCallback);
    p.setDownloadProgressCallback(lambda);
}
