// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/scratchconfiguration.h>
#include <string>

#ifdef JPEG_SUPPORT
#include "jpeg/jpegimageformatfactory.h"
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
        }
};

static DefaultImageFormats instance;

} // namespace libscratchcpp
