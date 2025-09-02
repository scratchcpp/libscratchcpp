// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cpr/api.h>
#include <cpr/response.h>

#include "idownloader.h"
#include "test_export.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_TEST_EXPORT Downloader : public IDownloader
{
    public:
        Downloader();

        bool download(const std::string &url) override;
        const std::string &text() const override;

    private:
        cpr::Session m_session;
        cpr::Response m_response;
};

} // namespace libscratchcpp
