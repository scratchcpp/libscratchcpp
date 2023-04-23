// SPDX-License-Identifier: Apache-2.0

#include "scratchproject.h"
#include "internal/scratch3reader.h"
#include <iostream>

using namespace libscratchcpp;

/*!
 * Constructs ScratchProject without a file name.
 * Use setFileName() to set a file name.
 */
ScratchProject::ScratchProject()
{
}

/*! Constructs ScratchProject with the given file name. */
ScratchProject::ScratchProject(std::string fileName) :
    m_fileName(fileName)
{
    // Auto detect Scratch version
    m_scratchVersion = Version::Invalid;
    Scratch3Reader scratch3;
    scratch3.setFileName(fileName);
    if (scratch3.isValid())
        m_scratchVersion = Version::Scratch3;

    if (m_scratchVersion == Version::Invalid)
        std::cerr << "Unable to determine Scratch version." << std::endl;
}

/*!
 * Constructs ScratchProjects with the given file name, for the given Scratch version.
 * \note You shouldn't normally use this constructor because the Scratch version is determined automatically.
 */
ScratchProject::ScratchProject(std::string fileName, Version scratchVersion) :
    m_fileName(fileName)
{
    setScratchVersion(scratchVersion);
}

/*! Loads the project. Returns true if successful. */
bool ScratchProject::load()
{
    std::shared_ptr<IProjectReader> reader;
    switch (m_scratchVersion) {
        case Version::Invalid:
            std::cerr << "Could not read the project because version is set to invalid." << std::endl;
            return false;
        case Version::Scratch3:
            reader = std::make_shared<Scratch3Reader>();
            break;
    }

    reader->setFileName(m_fileName);
    if (!reader->isValid()) {
        m_scratchVersion = Version::Invalid;
        std::cerr << "Could not read the project." << std::endl;
        return false;
    }

    bool ret = reader->load();
    if (!ret)
        return false;

    m_engine.clear();
    m_engine.setTargets(reader->targets());
    m_engine.setBroadcasts(reader->broadcasts());
    m_engine.setExtensions(reader->extensions());
    m_engine.compile();
    return true;
}

/*! Calls Engine#frame(). */
void ScratchProject::frame()
{
    m_engine.frame();
}

/*! Calls Engine#start(). */
void ScratchProject::start()
{
    m_engine.start();
}

/*! Calls Engine#run(). */
void ScratchProject::run()
{
    m_engine.run();
}

/*! Returns the project file name. */
std::string ScratchProject::fileName() const
{
    return m_fileName;
}

/*! Sets the project file name. */
void ScratchProject::setFileName(const std::string &newFileName)
{
    m_fileName = newFileName;
}

/*! Returns the version of Scratch used for the project. */
ScratchProject::Version ScratchProject::scratchVersion() const
{
    return m_scratchVersion;
}

/*!
 * Sets the version of Scratch.
 * \note You shouldn't use this as the Scratch version is automatically determined.
 */
void ScratchProject::setScratchVersion(const Version &version)
{
    // TODO: Use this when more versions become supported
    // if((version >= Version::Scratch3) && (version <= Version::Scratch3))
    if (version == Version::Scratch3)
        m_scratchVersion = version;
    else
        std::cerr << "Unsupported Scratch version: " << static_cast<int>(version) << std::endl;
}
