// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/project.h>
#include <string>

namespace libscratchcpp
{

class IEngine;

struct ProjectPrivate
{
        ProjectPrivate();
        ProjectPrivate(const std::string &fileName);
        ProjectPrivate(const std::string &fileName, ScratchVersion scratchVersion);
        ProjectPrivate(const ProjectPrivate &) = delete;

        bool load();

        void frame();
        void start();
        void run();

        void setScratchVersion(ScratchVersion version);

        ScratchVersion scratchVersion = ScratchVersion::Invalid;
        std::string fileName;
        std::shared_ptr<IEngine> engine = nullptr;
};

}; // namespace libscratchcpp
