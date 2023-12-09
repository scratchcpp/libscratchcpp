#include <internal/downloaderfactory.h>
#include <internal/downloader.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(DownloaderFactoryTest, Create)
{
    auto factory = DownloaderFactory::instance();
    std::shared_ptr<IDownloader> downloader = factory->create();
    ASSERT_TRUE(downloader);
    ASSERT_TRUE(std::dynamic_pointer_cast<Downloader>(downloader));
}
