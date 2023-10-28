// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/scratchconfiguration.h>
#include <string>

#ifdef JPEG_SUPPORT
#include "jpeg/jpegimageformatfactory.h"
#endif

#ifdef PNG_SUPPORT
#include "png/pngimageformatfactory.h"
#endif

namespace libscratchcpp
{

class DefaultImageFormats
{
    public:
        DefaultImageFormats()
        {
#ifdef JPEG_SUPPORT
            ScratchConfiguration::registerImageFormat("jpg", std::make_shared<JpegImageFormatFactory>());
#endif

#ifdef PNG_SUPPORT
            ScratchConfiguration::registerImageFormat("png", std::make_shared<PngImageFormatFactory>());
#endif
        }
};

static DefaultImageFormats instance;

} // namespace libscratchcpp
