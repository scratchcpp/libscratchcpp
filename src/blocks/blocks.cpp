// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/scratchconfiguration.h>

#include "blocks.h"

#ifdef ENABLE_MOTION_BLOCKS
#include "motionblocks.h"
#endif
#ifdef ENABLE_LOOKS_BLOCKS
#include "looksblocks.h"
#endif
#ifdef ENABLE_SOUND_BLOCKS
#include "soundblocks.h"
#endif
#ifdef ENABLE_EVENT_BLOCKS
#include "eventblocks.h"
#endif
#ifdef ENABLE_CONTROL_BLOCKS
#include "controlblocks.h"
#endif
#ifdef ENABLE_SENSING_BLOCKS
#include "sensingblocks.h"
#endif
#ifdef ENABLE_OPERATOR_BLOCKS
#include "operatorblocks.h"
#endif
#ifdef ENABLE_VARIABLE_BLOCKS
#include "variableblocks.h"
#endif
#ifdef ENABLE_LIST_BLOCKS
#include "listblocks.h"
#endif
#ifdef ENABLE_CUSTOM_BLOCKS
#include "customblocks.h"
#endif

using namespace libscratchcpp;

Blocks Blocks::m_instance;

const std::vector<std::shared_ptr<IExtension>> &Blocks::extensions()
{
    return m_instance.m_extensions;
}

Blocks::Blocks()
{
#ifdef ENABLE_MOTION_BLOCKS
    m_extensions.push_back(std::make_shared<MotionBlocks>());
#endif
#ifdef ENABLE_LOOKS_BLOCKS
    m_extensions.push_back(std::make_shared<LooksBlocks>());
#endif
#ifdef ENABLE_SOUND_BLOCKS
    m_extensions.push_back(std::make_shared<SoundBlocks>());
#endif
#ifdef ENABLE_EVENT_BLOCKS
    m_extensions.push_back(std::make_shared<EventBlocks>());
#endif
#ifdef ENABLE_CONTROL_BLOCKS
    m_extensions.push_back(std::make_shared<ControlBlocks>());
#endif
#ifdef ENABLE_SENSING_BLOCKS
    m_extensions.push_back(std::make_shared<SensingBlocks>());
#endif
#ifdef ENABLE_OPERATOR_BLOCKS
    m_extensions.push_back(std::make_shared<OperatorBlocks>());
#endif
#ifdef ENABLE_VARIABLE_BLOCKS
    m_extensions.push_back(std::make_shared<VariableBlocks>());
#endif
#ifdef ENABLE_LIST_BLOCKS
    m_extensions.push_back(std::make_shared<ListBlocks>());
#endif
#ifdef ENABLE_CUSTOM_BLOCKS
    m_extensions.push_back(std::make_shared<CustomBlocks>());
#endif
}

void Blocks::registerExtensions()
{
    for (auto ext : m_extensions)
        ScratchConfiguration::registerExtension(ext);
}
