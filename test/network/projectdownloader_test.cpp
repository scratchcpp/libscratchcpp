#include <internal/projectdownloader.h>
#include <downloaderfactorymock.h>
#include <downloadermock.h>
#include <thread>

#include "../common.h"

using namespace libscratchcpp;

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::Invoke;

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

    // No project metadata
    EXPECT_CALL(*m_tokenDownloader, startDownload(metaUrl));
    EXPECT_CALL(*m_tokenDownloader, wait());
    EXPECT_CALL(*m_tokenDownloader, text()).WillOnce(ReturnRef(empty));
    ASSERT_FALSE(m_downloader->downloadJson(projectId));

    // Cancelled metadata download
    EXPECT_CALL(*m_tokenDownloader, startDownload(metaUrl));
    EXPECT_CALL(*m_tokenDownloader, wait());
    EXPECT_CALL(*m_tokenDownloader, isCancelled()).WillOnce(Return(true));
    EXPECT_CALL(*m_tokenDownloader, text()).WillOnce(ReturnRef(invalidMeta));
    ASSERT_FALSE(m_downloader->downloadJson(projectId));

    // Unparsable project metadata
    EXPECT_CALL(*m_tokenDownloader, startDownload(metaUrl));
    EXPECT_CALL(*m_tokenDownloader, wait());
    EXPECT_CALL(*m_tokenDownloader, isCancelled()).WillOnce(Return(false));
    EXPECT_CALL(*m_tokenDownloader, text()).Times(2).WillRepeatedly(ReturnRef(unparsableMeta));
    ASSERT_FALSE(m_downloader->downloadJson(projectId));

    // Invalid project metadata
    EXPECT_CALL(*m_tokenDownloader, startDownload(metaUrl));
    EXPECT_CALL(*m_tokenDownloader, wait());
    EXPECT_CALL(*m_tokenDownloader, isCancelled()).WillOnce(Return(false));
    EXPECT_CALL(*m_tokenDownloader, text()).Times(2).WillRepeatedly(ReturnRef(invalidMeta));
    ASSERT_FALSE(m_downloader->downloadJson(projectId));

    // No project JSON
    EXPECT_CALL(*m_tokenDownloader, startDownload(metaUrl));
    EXPECT_CALL(*m_tokenDownloader, wait());
    EXPECT_CALL(*m_tokenDownloader, isCancelled()).WillOnce(Return(false));
    EXPECT_CALL(*m_tokenDownloader, text()).Times(2).WillRepeatedly(ReturnRef(meta));

    EXPECT_CALL(*m_jsonDownloader, startDownload(projectJsonUrl));
    EXPECT_CALL(*m_jsonDownloader, wait());
    EXPECT_CALL(*m_jsonDownloader, text()).WillOnce(ReturnRef(empty));
    ASSERT_FALSE(m_downloader->downloadJson(projectId));

    // Cancelled project JSON download
    EXPECT_CALL(*m_tokenDownloader, startDownload(metaUrl));
    EXPECT_CALL(*m_tokenDownloader, wait());
    EXPECT_CALL(*m_tokenDownloader, isCancelled()).WillOnce(Return(false));
    EXPECT_CALL(*m_tokenDownloader, text()).Times(2).WillRepeatedly(ReturnRef(meta));

    EXPECT_CALL(*m_jsonDownloader, startDownload(projectJsonUrl));
    EXPECT_CALL(*m_jsonDownloader, wait());
    EXPECT_CALL(*m_jsonDownloader, isCancelled()).WillOnce(Return(true));
    EXPECT_CALL(*m_jsonDownloader, text()).WillOnce(ReturnRef(projectJson));
    ASSERT_FALSE(m_downloader->downloadJson(projectId));

    // Success
    EXPECT_CALL(*m_tokenDownloader, startDownload(metaUrl));
    EXPECT_CALL(*m_tokenDownloader, wait());
    EXPECT_CALL(*m_tokenDownloader, isCancelled()).WillOnce(Return(false));
    EXPECT_CALL(*m_tokenDownloader, text()).Times(2).WillRepeatedly(ReturnRef(meta));

    EXPECT_CALL(*m_jsonDownloader, startDownload(projectJsonUrl));
    EXPECT_CALL(*m_jsonDownloader, wait());
    EXPECT_CALL(*m_jsonDownloader, isCancelled()).WillOnce(Return(false));
    EXPECT_CALL(*m_jsonDownloader, text()).WillOnce(ReturnRef(projectJson));
    ASSERT_TRUE(m_downloader->downloadJson(projectId));

    EXPECT_CALL(*m_jsonDownloader, text()).WillOnce(ReturnRef(projectJson));
    ASSERT_EQ(m_downloader->json(), projectJson);
}

TEST_F(ProjectDownloaderTest, DownloadAssets)
{
    static const std::string assetPrefix = "https://assets.scratch.mit.edu/internalapi/asset/";
    static const std::string assetSuffix = "/get";
    static const std::vector<std::string> assetIds = { "abc", "def", "ghi", "jkl", "mno", "pqr", "stu", "vwx", "yzA", "BCD", "EFG", "HIJ", "KLM", "NOP", "QRS", "TUV", "WXY" };
    static const std::vector<std::string> assetData = { "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9", "a10", "a11", "a12", "a13", "a14", "a15", "a16", "a17" };
    ASSERT_EQ(assetIds.size(), assetData.size());
    std::vector<std::shared_ptr<DownloaderMock>> downloaders;
    std::vector<unsigned int> downloaderUseCount;
    std::vector<std::vector<const std::string *>> downloaderTexts;
    std::vector<unsigned int> downloaderTextIndexes;
    unsigned int threadCount = std::thread::hardware_concurrency();
    unsigned int times = 0;

    if (threadCount == 0) {
        threadCount = 1;
        times = 1;
    } else
        times = std::ceil(assetIds.size() / static_cast<double>(threadCount));

    downloaders.reserve(threadCount);
    downloaderUseCount.reserve(threadCount);
    downloaderTexts.reserve(threadCount);
    downloaderTextIndexes.reserve(threadCount);

    for (int i = 0; i < threadCount; i++) {
        downloaders.push_back(std::make_shared<DownloaderMock>());
        downloaderUseCount.push_back(0);
        downloaderTexts.push_back({});
        downloaderTextIndexes.push_back(0);
    }

    // Cancelled
    unsigned int count = 0;
    EXPECT_CALL(*m_factory, create()).Times(threadCount);
    ON_CALL(*m_factory, create()).WillByDefault(Invoke([&downloaders, &count]() {
        assert(count < downloaders.size());
        return downloaders[count++];
    }));

    for (int i = 0; i < times; i++) {
        unsigned int count = std::min(threadCount, static_cast<unsigned int>(assetIds.size() - i * threadCount));

        for (int j = 0; j < count; j++) {
            auto downloader = downloaders[j];
            auto index = i * threadCount + j;
            ASSERT_LT(index, assetIds.size());

            if (index < threadCount) {
                EXPECT_CALL(*downloader, startDownload(assetPrefix + assetIds[index] + assetSuffix));
                EXPECT_CALL(*downloader, text()).WillRepeatedly(Invoke([j, &downloaderTexts, &downloaderTextIndexes]() -> const std::string & {
                    unsigned long d = j;
                    return *downloaderTexts[d][downloaderTextIndexes[d]++];
                }));
            }

            downloaderUseCount[j]++;
            downloaderTexts[j].push_back(&assetData[index]);
        }
    }

    for (int i = 0; i < threadCount; i++) {
        if (i == 0) {
            EXPECT_CALL(*downloaders[i], wait());
            EXPECT_CALL(*downloaders[i], isCancelled()).WillOnce(Return(false));
        } else if (i == 1) {
            EXPECT_CALL(*downloaders[i], wait());
            EXPECT_CALL(*downloaders[i], isCancelled()).WillOnce(Return(true));
        } else {
            EXPECT_CALL(*downloaders[i], wait()).Times(0);
            EXPECT_CALL(*downloaders[i], isCancelled()).Times(0);
        }
    }

    ASSERT_FALSE(m_downloader->downloadAssets(assetIds));
    ASSERT_EQ(m_downloader->downloadedAssetCount(), 1);

    // Success
    count = 0;
    EXPECT_CALL(*m_factory, create()).Times(threadCount);

    for (int i = 0; i < threadCount; i++) {
        downloaderUseCount[i] = 0;
        downloaderTexts[i].clear();
        downloaderTextIndexes[i] = 0;
    }

    for (int i = 0; i < times; i++) {
        unsigned int count = std::min(threadCount, static_cast<unsigned int>(assetIds.size() - i * threadCount));

        for (int j = 0; j < count; j++) {
            auto downloader = downloaders[j];
            auto index = i * threadCount + j;
            ASSERT_LT(index, assetIds.size());
            EXPECT_CALL(*downloader, startDownload(assetPrefix + assetIds[index] + assetSuffix));
            EXPECT_CALL(*downloader, text()).WillRepeatedly(Invoke([j, &downloaderTexts, &downloaderTextIndexes]() -> const std::string & {
                unsigned long d = j;
                return *downloaderTexts[d][downloaderTextIndexes[d]++];
            }));
            downloaderUseCount[j]++;
            downloaderTexts[j].push_back(&assetData[index]);
        }
    }

    for (int i = 0; i < threadCount; i++) {
        EXPECT_CALL(*downloaders[i], wait()).Times(downloaderUseCount[i]);
        EXPECT_CALL(*downloaders[i], isCancelled()).Times(downloaderUseCount[i]).WillRepeatedly(Return(false));
    }

    ASSERT_TRUE(m_downloader->downloadAssets(assetIds));
    ASSERT_EQ(m_downloader->assets(), assetData);
    ASSERT_EQ(m_downloader->downloadedAssetCount(), assetIds.size());
}

TEST_F(ProjectDownloaderTest, Cancel)
{
    EXPECT_CALL(*m_tokenDownloader, cancel());
    EXPECT_CALL(*m_jsonDownloader, cancel());
    m_downloader->cancel();
}
