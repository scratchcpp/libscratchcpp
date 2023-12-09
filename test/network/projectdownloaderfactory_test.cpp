#include <internal/projectdownloaderfactory.h>
#include <internal/projectdownloader.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(ProjectDownloaderFactoryTest, Create)
{
    auto factory = ProjectDownloaderFactory::instance();
    std::shared_ptr<IProjectDownloader> downloader = factory->create();
    ASSERT_TRUE(downloader);
    ASSERT_TRUE(std::dynamic_pointer_cast<ProjectDownloader>(downloader));
}
