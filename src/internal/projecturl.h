// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace libscratchcpp
{

class ProjectUrl
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
