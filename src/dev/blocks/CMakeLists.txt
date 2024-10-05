target_sources(scratchcpp
  PRIVATE
    blocks.cpp
    blocks.h
)

# Motion blocks
option(LIBSCRATCHCPP_ENABLE_MOTION_BLOCKS "Motion blocks support" ON)
if (LIBSCRATCHCPP_ENABLE_MOTION_BLOCKS)
    target_compile_definitions(scratchcpp PRIVATE ENABLE_MOTION_BLOCKS)
    target_sources(scratchcpp
      PRIVATE
        motionblocks.cpp
        motionblocks.h
    )
endif()

# Looks blocks
option(LIBSCRATCHCPP_ENABLE_LOOKS_BLOCKS "Looks blocks support" ON)
if (LIBSCRATCHCPP_ENABLE_LOOKS_BLOCKS)
    target_compile_definitions(scratchcpp PRIVATE ENABLE_LOOKS_BLOCKS)
    target_sources(scratchcpp
      PRIVATE
        looksblocks.cpp
        looksblocks.h
    )
endif()

# Sound blocks
option(LIBSCRATCHCPP_ENABLE_SOUND_BLOCKS "Sound blocks support" ON)
if (LIBSCRATCHCPP_ENABLE_SOUND_BLOCKS)
    target_compile_definitions(scratchcpp PRIVATE ENABLE_SOUND_BLOCKS)
    target_sources(scratchcpp
      PRIVATE
        soundblocks.cpp
        soundblocks.h
    )
endif()

# Event blocks
option(LIBSCRATCHCPP_ENABLE_EVENT_BLOCKS "Event blocks support" ON)
if (LIBSCRATCHCPP_ENABLE_EVENT_BLOCKS)
    target_compile_definitions(scratchcpp PRIVATE ENABLE_EVENT_BLOCKS)
    target_sources(scratchcpp
      PRIVATE
        eventblocks.cpp
        eventblocks.h
    )
endif()

# Control blocks
option(LIBSCRATCHCPP_ENABLE_CONTROL_BLOCKS "Control blocks support" ON)
if (LIBSCRATCHCPP_ENABLE_CONTROL_BLOCKS)
    target_compile_definitions(scratchcpp PRIVATE ENABLE_CONTROL_BLOCKS)
    target_sources(scratchcpp
      PRIVATE
        controlblocks.cpp
        controlblocks.h
    )
endif()

# Sensing blocks
option(LIBSCRATCHCPP_ENABLE_SENSING_BLOCKS "Sensing blocks support" ON)
if (LIBSCRATCHCPP_ENABLE_SENSING_BLOCKS)
    target_compile_definitions(scratchcpp PRIVATE ENABLE_SENSING_BLOCKS)
    target_sources(scratchcpp
      PRIVATE
        sensingblocks.cpp
        sensingblocks.h
    )
endif()

# Operator blocks
option(LIBSCRATCHCPP_ENABLE_OPERATOR_BLOCKS "Operator blocks support" ON)
if (LIBSCRATCHCPP_ENABLE_OPERATOR_BLOCKS)
    target_compile_definitions(scratchcpp PRIVATE ENABLE_OPERATOR_BLOCKS)
    target_sources(scratchcpp
      PRIVATE
        operatorblocks.cpp
        operatorblocks.h
    )
endif()

# Variable blocks
option(LIBSCRATCHCPP_ENABLE_VARIABLE_BLOCKS "Variable blocks support" ON)
if (LIBSCRATCHCPP_ENABLE_VARIABLE_BLOCKS)
    target_compile_definitions(scratchcpp PRIVATE ENABLE_VARIABLE_BLOCKS)
    target_sources(scratchcpp
      PRIVATE
        variableblocks.cpp
        variableblocks.h
    )
endif()

# List blocks
option(LIBSCRATCHCPP_ENABLE_LIST_BLOCKS "List blocks support" ON)
if (LIBSCRATCHCPP_ENABLE_LIST_BLOCKS)
    target_compile_definitions(scratchcpp PRIVATE ENABLE_LIST_BLOCKS)
    target_sources(scratchcpp
      PRIVATE
        listblocks.cpp
        listblocks.h
    )
endif()

# Custom blocks
option(LIBSCRATCHCPP_ENABLE_CUSTOM_BLOCKS "Custom blocks support" ON)
if (LIBSCRATCHCPP_ENABLE_CUSTOM_BLOCKS)
    target_compile_definitions(scratchcpp PRIVATE ENABLE_CUSTOM_BLOCKS)
    target_sources(scratchcpp
      PRIVATE
        customblocks.cpp
        customblocks.h
    )
endif()
