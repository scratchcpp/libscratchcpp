// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cpr/api.h>
#include <cpr/response.h>

#include "idownloader.h"

namespace libscratchcpp
{

class Downloader : public IDownloader
{
    public:
        Downloader();

        void startDownload(const std::string &url) override;
        void cancel() override;
        void wait() override;

        const std::string &text() const override;

    private:
        std::unique_ptr<cpr::AsyncResponse> m_asyncResponse;
        cpr::Response m_response;
};

} // namespace libscratchcpp
