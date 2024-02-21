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

    // Whitespaces to be removed
    const char* whitespace = " \t\v\r\n";

    // Parse first part of URL and remove leading whitespaces
    std::getline(stream, str, '/');
    if (str.empty())
        return;
    std::size_t start = str.find_first_not_of(whitespace);
    if (start != 0) 
        str = str.substr(start, str.length() - start + 1);
    parts.push_back(str);
    
    // Parse the rest of the URL
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
    
    // Remove trailing whitespaces
    std::size_t end = str.find_last_not_of(whitespace);
    if (end != m_projectId.length())
        m_projectId = m_projectId.substr(0, end + 1);
}

bool ProjectUrl::isProjectUrl() const
{
    return m_isProjectUrl;
}

const std::string &ProjectUrl::projectId() const
{
    return m_projectId;
}

