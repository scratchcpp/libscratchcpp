// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/project.h>

#include "project_p.h"

using namespace libscratchcpp;

/*!
 * Constructs Project without a file name.
 * Use setFileName() to set a file name.
 */
Project::Project() :
    impl(spimpl::make_unique_impl<ProjectPrivate>())
{
}

/*! Constructs ScratchProject with the given file name. */
Project::Project(const std::string &fileName) :
    impl(spimpl::make_unique_impl<ProjectPrivate>(fileName))
{
}

/*!
 * Constructs ScratchProjects with the given file name, for the given Scratch version.
 * \note You shouldn't normally use this constructor because the Scratch version is determined automatically.
 */
Project::Project(const std::string &fileName, ScratchVersion scratchVersion) :
    impl(spimpl::make_unique_impl<ProjectPrivate>(fileName, scratchVersion))
{
}

/*! Loads the project. Returns true if successful. */
bool Project::load()
{
    return impl->load();
}

/*!
 * Calls all "when green flag clicked" blocks.
 * \note Nothing will happen until run() or frame() is called.
 */
void Project::start()
{
    impl->start();
}

/*!
 * Calls and runs "when green flag clicked" blocks.
 * \note This function returns when all scripts finish.\n
 * If you need an event loop that runs even after the project stops,
 * use runEventLoop().
 */
void Project::run()
{
    impl->run();
}

/*!
 * Runs the event loop. Call start() (from another thread) to start the project.
 * \note This should be called from another thread in GUI project players to keep the UI responsive.
 */
void Project::runEventLoop()
{
    impl->runEventLoop();
}

/*! Returns the project file name or URL. */
const std::string &Project::fileName() const
{
    return impl->fileName;
}

/*! Sets the project file name or URL. */
void Project::setFileName(const std::string &newFileName)
{
    impl->fileName = newFileName;
    impl->detectScratchVersion();
}

/*! Returns the version of Scratch used for the project. */
ScratchVersion Project::scratchVersion() const
{
    return impl->scratchVersion;
}

/*!
 * Sets the version of Scratch.
 * \note You shouldn't use this as the Scratch version is automatically determined.
 */
void Project::setScratchVersion(const ScratchVersion &version)
{
    impl->setScratchVersion(version);
}

/*! Returns the engine of the loaded project. \see IEngine */
std::shared_ptr<IEngine> Project::engine() const
{
    return impl->engine;
}

/*!
 * Emits when the asset download progress changes.
 * \note The first parameter is the number of downloaded assets and the latter is the number of all assets to download.
 */
sigslot::signal<unsigned int, unsigned int> &Project::downloadProgressChanged()
{
    return impl->downloadProgressChanged();
}
