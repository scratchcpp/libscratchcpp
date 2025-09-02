// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "test_export.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_TEST_EXPORT ProjectUrl
{
    public:
        ProjectUrl(const std::string &url);

        bool isProjectUrl() const;
        const std::string &projectId() const;

    private:
        bool m_isProjectUrl = false;
        std::string m_projectId;
};

} // namespace libscratchcpp
