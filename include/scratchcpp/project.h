// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include "spimpl.h"

#include "global.h"

namespace libscratchcpp
{

class ProjectPrivate;

class IEngine;

class LIBSCRATCHCPP_EXPORT Project
{
    public:
        Project();
        Project(const std::string &fileName);
        Project(const std::string &fileName, ScratchVersion scratchVersion);
        Project(const Project &) = delete;

        bool load();

        void frame();
        void start();
        void run();

        const std::string &fileName() const;
        void setFileName(const std::string &newFileName);

        ScratchVersion scratchVersion() const;
        void setScratchVersion(const ScratchVersion &version);

        std::shared_ptr<IEngine> engine() const;

    private:
        spimpl::unique_impl_ptr<ProjectPrivate> impl;
};

} // namespace libscratchcpp
