// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/project.h>
#include <string>

#include "engine/engine.h"

namespace libscratchcpp
{

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

        ScratchVersion scratchVersion;
        std::string fileName;
        Engine engine;
};

}; // namespace libscratchcpp
