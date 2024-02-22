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
    
    // trim leading whitespaces
    str = parts.front();
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    parts[0] = str;

    // trim trailing whitespaces
    str = parts.back();
    str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch){
        return !std::isspace(ch);
        }).base(), str.end());
    parts[parts.size() - 1] = str;

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

