// SPDX-License-Identifier: Apache-2.0

#include <vector>
#include <sstream>
#include <algorithm>

#include "projecturl.h"

using namespace libscratchcpp;

static const std::string PROJECT_SITE = "scratch.mit.edu";
static const std::string PROJECT_SUBSITE = "projects";

ProjectUrl::ProjectUrl(const std::string &url)
{
    std::vector<std::string> parts;
    std::istringstream stream(url);
    std::string str;

    while (std::getline(stream, str, '/'))
        parts.push_back(str);

    if (parts.empty())
        return;

    // Remove empty parts
    parts.erase(std::remove(parts.begin(), parts.end(), ""), parts.end());

    // Remove "http:" or "https:"
    while (!parts.empty() && ((parts.front() == "http:") || (parts.front() == "https:")))
        parts.erase(parts.begin());

    // scratch.mit.edu/projects/(id)
    if (parts.size() != 3)
        return;

    m_isProjectUrl = ((parts[0] == PROJECT_SITE) && (parts[1] == PROJECT_SUBSITE));
    m_projectId = parts[2];
}

bool ProjectUrl::isProjectUrl() const
{
    return m_isProjectUrl;
}

const std::string &ProjectUrl::projectId() const
{
    return m_projectId;
}
