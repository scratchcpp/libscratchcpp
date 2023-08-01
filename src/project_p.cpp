// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "project_p.h"
#include "internal/scratch3reader.h"
#include "engine/engine.h"

using namespace libscratchcpp;

ProjectPrivate::ProjectPrivate() :
    engine(std::make_shared<Engine>())
{
}

ProjectPrivate::ProjectPrivate(const std::string &fileName) :
    fileName(fileName),
    engine(std::make_shared<Engine>())
{
    // Auto detect Scratch version
    scratchVersion = ScratchVersion::Invalid;
    Scratch3Reader scratch3;
    scratch3.setFileName(fileName);
    if (scratch3.isValid())
        scratchVersion = ScratchVersion::Scratch3;

    if (scratchVersion == ScratchVersion::Invalid)
        std::cerr << "Unable to determine Scratch version." << std::endl;
}

ProjectPrivate::ProjectPrivate(const std::string &fileName, ScratchVersion scratchVersion) :
    fileName(fileName),
    engine(std::make_shared<Engine>())
{
    setScratchVersion(scratchVersion);
}

bool ProjectPrivate::load()
{
    std::shared_ptr<IProjectReader> reader;
    switch (scratchVersion) {
        case ScratchVersion::Invalid:
            std::cerr << "Could not read the project because version is set to invalid." << std::endl;
            return false;
        case ScratchVersion::Scratch3:
            reader = std::make_shared<Scratch3Reader>();
            break;
    }

    reader->setFileName(fileName);
    if (!reader->isValid()) {
        scratchVersion = ScratchVersion::Invalid;
        std::cerr << "Could not read the project." << std::endl;
        return false;
    }

    bool ret = reader->load();
    if (!ret)
        return false;

    engine->clear();
    // TODO: Remove the casts
    std::reinterpret_pointer_cast<Engine>(engine)->setTargets(reader->targets());
    std::reinterpret_pointer_cast<Engine>(engine)->setBroadcasts(reader->broadcasts());
    engine->setExtensions(reader->extensions());
    engine->compile();
    return true;
}

void ProjectPrivate::frame()
{
    engine->frame();
}

void ProjectPrivate::start()
{
    engine->start();
}

void ProjectPrivate::run()
{
    engine->run();
}

void ProjectPrivate::setScratchVersion(ScratchVersion version)
{
    // TODO: Use this when more versions become supported
    // if((version >= Version::Scratch3) && (version <= Version::Scratch3))
    if (version == ScratchVersion::Scratch3)
        scratchVersion = version;
    else
        std::cerr << "Unsupported Scratch version: " << static_cast<int>(version) << std::endl;
}
