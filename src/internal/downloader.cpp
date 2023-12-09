// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "downloader.h"

using namespace libscratchcpp;

Downloader::Downloader()
{
    m_session.SetTimeout(cpr::Timeout(5000));
}

bool Downloader::download(const std::string &url)
{
    m_session.SetUrl(cpr::Url(url));
    m_response = m_session.Get();

    if (m_response.status_code != 200) {
        std::cerr << "download error: " << m_response.error.message << std::endl;

        if (m_response.status_code != 0)
            std::cerr << "code: " << m_response.status_code << std::endl;

        std::cerr << "URL: " << url << std::endl;
        return false;
    }

    return true;
}

const std::string &Downloader::text() const
{
    return m_response.text;
}
