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
 * Runs a single frame.\n
 * Use this if you want to use a custom event loop
 * in your project player.
 * \note Nothing will happen until start() is called.
 */
void Project::frame()
{
    impl->frame();
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
 * Runs the event loop and calls "when green flag clicked" blocks.
 * \note This function returns when all scripts finish.\n
 * If you need to implement something advanced, such as a GUI with the
 * green flag button, use frame().
 */
void Project::run()
{
    impl->run();
}

/*! Returns the project file name. */
const std::string &Project::fileName() const
{
    return impl->fileName;
}

/*! Sets the project file name. */
void Project::setFileName(const std::string &newFileName)
{
    impl->fileName = newFileName;
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
