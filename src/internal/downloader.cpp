// SPDX-License-Identifier: Apache-2.0

#include "downloader.h"

using namespace libscratchcpp;

Downloader::Downloader()
{
}

void Downloader::startDownload(const std::string &url)
{
    if (m_asyncResponse)
        cancel();

    m_asyncResponse = std::make_unique<cpr::AsyncResponse>(cpr::GetAsync(cpr::Url(url)));
}

void Downloader::cancel()
{
    if (m_asyncResponse) {
        if (!m_asyncResponse->IsCancelled())
            (void)(m_asyncResponse->Cancel());
        m_asyncResponse.reset();
    }
}

void Downloader::wait()
{
    if (m_asyncResponse) {
        m_asyncResponse->wait();
        m_response = m_asyncResponse->get();
    }
}

const std::string &Downloader::text() const
{
    return m_response.text;
}
