#include <internal/projectdownloader.h>
#include <downloaderfactorymock.h>
#include <downloadermock.h>
#include <thread>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::Invoke;
using ::testing::SaveArg;
using ::testing::_;

class ProjectDownloaderTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_factory = std::make_unique<DownloaderFactoryMock>();
            m_tokenDownloader = std::make_shared<DownloaderMock>();
            m_jsonDownloader = std::make_shared<DownloaderMock>();
            EXPECT_CALL(*m_factory, create()).WillOnce(Return(m_tokenDownloader)).WillOnce(Return(m_jsonDownloader));
            m_downloader = std::make_unique<ProjectDownloader>(m_factory.get());
        }

        std::unique_ptr<DownloaderFactoryMock> m_factory;
        std::unique_ptr<ProjectDownloader> m_downloader;
        std::shared_ptr<DownloaderMock> m_tokenDownloader;
        std::shared_ptr<DownloaderMock> m_jsonDownloader;
};

TEST_F(ProjectDownloaderTest, DownloadJson)
{
    static const std::string projectId = "886166351";
    static const std::string metaUrl = "https://api.scratch.mit.edu/projects/" + projectId;
    static const std::string meta = "{ \"hello\": \"abc\", \"project_token\": \"abc\" }";
    static const std::string unparsableMeta = "test";
    static const std::string invalidMeta = "{ \"hello\": \"abc\", \"world\": \"def\" }";
    static const std::string projectJsonUrl = "https://projects.scratch.mit.edu/" + projectId + "?token=abc";
    static const std::string projectJson = "Hello, world!";
    static const std::string empty = "";

    // Failed metadata download
    EXPECT_CALL(*m_tokenDownloader, download(metaUrl)).WillOnce(Return(false));
    ASSERT_FALSE(m_downloader->downloadJson(projectId));

    // Unparsable project metadata
    EXPECT_CALL(*m_tokenDownloader, download(metaUrl)).WillOnce(Return(true));
    EXPECT_CALL(*m_tokenDownloader, text()).WillOnce(ReturnRef(unparsableMeta));
    ASSERT_FALSE(m_downloader->downloadJson(projectId));

    // Invalid project metadata
    EXPECT_CALL(*m_tokenDownloader, download(metaUrl)).WillOnce(Return(true));
    EXPECT_CALL(*m_tokenDownloader, text()).WillOnce(ReturnRef(invalidMeta));
    ASSERT_FALSE(m_downloader->downloadJson(projectId));

    // Failed project JSON download
    EXPECT_CALL(*m_tokenDownloader, download(metaUrl)).WillOnce(Return(true));
    EXPECT_CALL(*m_tokenDownloader, text()).WillOnce(ReturnRef(meta));

    EXPECT_CALL(*m_jsonDownloader, download(projectJsonUrl)).WillOnce(Return(false));
    ASSERT_FALSE(m_downloader->downloadJson(projectId));

    // Success
    EXPECT_CALL(*m_tokenDownloader, download(metaUrl)).WillOnce(Return(true));
    EXPECT_CALL(*m_tokenDownloader, text()).WillOnce(ReturnRef(meta));

    EXPECT_CALL(*m_jsonDownloader, download(projectJsonUrl)).WillOnce(Return(true));
    ASSERT_TRUE(m_downloader->downloadJson(projectId));

    EXPECT_CALL(*m_jsonDownloader, text()).WillOnce(ReturnRef(projectJson));
    ASSERT_EQ(m_downloader->json(), projectJson);
    ASSERT_EQ(m_downloader->downloadedAssetCount(), 0);
}

// NOTE: Asset downloading should be tested manually
