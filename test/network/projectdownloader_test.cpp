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

TEST_F(ProjectDownloaderTest, DownloadAssets)
{
    static const std::string assetPrefix = "https://assets.scratch.mit.edu/internalapi/asset/";
    static const std::string assetSuffix = "/get";
    static const std::vector<std::string> assetIds = { "abc", "def", "ghi", "jkl", "mno", "pqr", "stu", "vwx", "yzA", "BCD", "EFG", "HIJ", "KLM", "NOP", "QRS", "TUV", "WXY" };
    static const std::vector<std::string> assetData = { "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9", "a10", "a11", "a12", "a13", "a14", "a15", "a16", "a17" };
    ASSERT_EQ(assetIds.size(), assetData.size());
    std::vector<std::shared_ptr<DownloaderMock>> downloaders;
    std::vector<unsigned int> downloaderTextIndexes;
    unsigned int threadCount = std::thread::hardware_concurrency();
    threadCount = std::max(1u, std::min(threadCount, static_cast<unsigned int>(std::ceil(assetIds.size() / 5.0))));

    downloaders.reserve(threadCount);
    downloaderTextIndexes.reserve(threadCount);

    for (unsigned int i = 0; i < threadCount; i++) {
        downloaders.push_back(std::make_shared<DownloaderMock>());
        downloaderTextIndexes.push_back(0);
    }

    // Success
    unsigned int count = 0;
    EXPECT_CALL(*m_factory, create()).Times(threadCount);
    ON_CALL(*m_factory, create()).WillByDefault(Invoke([&downloaders, &count]() {
        assert(count < downloaders.size());
        return downloaders[count++];
    }));

    for (unsigned int i = 0; i < threadCount; i++)
        downloaderTextIndexes[i] = 0;

    for (unsigned int i = 0; i < threadCount; i++) {
        auto downloader = downloaders[i];
        unsigned int n = std::ceil(assetIds.size() / static_cast<double>(threadCount));
        unsigned int repeatCount = std::min(n, static_cast<unsigned int>(assetIds.size()) - i * n);

        for (unsigned int j = 0; j < repeatCount; j++) {
            unsigned int index = i * n + j;
            EXPECT_CALL(*downloader, download(assetPrefix + assetIds[index] + assetSuffix)).WillOnce(Return(true));
        }

        EXPECT_CALL(*downloader, text()).Times(repeatCount).WillRepeatedly(Invoke([i, n, &downloaderTextIndexes]() -> const std::string & { return assetData[i * n + downloaderTextIndexes[i]++]; }));
    }

    ASSERT_TRUE(m_downloader->downloadAssets(assetIds));
    ASSERT_EQ(m_downloader->assets(), assetData);
    ASSERT_EQ(m_downloader->downloadedAssetCount(), assetIds.size());
}
