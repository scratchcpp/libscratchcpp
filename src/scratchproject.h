// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "engine/engine.h"
#include "libscratchcpp_global.h"
#include "scratch/variable.h"
#include <string>

namespace libscratchcpp
{

/*! \brief The ScratchProject class provides an API for reading and running Scratch projects. */
class LIBSCRATCHCPP_EXPORT ScratchProject
{
    public:
        enum class Version
        {
            Invalid = 0, /*!< An unsupported version. */
            Scratch3 = 3 /*!< Scratch 3.0 */
        };

        ScratchProject();
        ScratchProject(std::string fileName);
        ScratchProject(std::string fileName, Version scratchVersion);

        bool load();

        void frame();
        void start();
        void run();

        std::string fileName() const;
        void setFileName(const std::string &newFileName);

        Version scratchVersion() const;
        void setScratchVersion(const Version &version);

        const Engine *engine() const;

    private:
        std::string m_fileName;
        Version m_scratchVersion = Version::Invalid;
        Engine m_engine;
};

} // namespace libscratchcpp
