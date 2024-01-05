option(LIBSCRATCHCPP_AUDIO_SUPPORT "Audio support" ON)

add_library(scratchcpp-audio STATIC)
set(MINIAUDIO_SRC thirdparty/miniaudio)

if (LIBSCRATCHCPP_AUDIO_SUPPORT)
    add_library(miniaudio STATIC
	    ${MINIAUDIO_SRC}/miniaudio.c
	    ${MINIAUDIO_SRC}/miniaudio.h
    )
    target_include_directories(scratchcpp-audio PUBLIC ${CMAKE_CURRENT_LIST_DIR}/${MINIAUDIO_SRC})
    target_link_libraries(scratchcpp-audio PRIVATE miniaudio)
    target_compile_definitions(scratchcpp-audio PUBLIC LIBSCRATCHCPP_AUDIO_SUPPORT)
endif()

target_sources(scratchcpp-audio
  PUBLIC
    iaudioplayer.h
    iaudioplayerfactory.h
    audioplayerfactory.cpp
    audioplayerfactory.h
)

if (LIBSCRATCHCPP_AUDIO_SUPPORT)
    target_sources(scratchcpp-audio
      PUBLIC
        audioengine.cpp
	audioengine.h
	audioplayer.cpp
	audioplayer.h
    )
else()
    target_sources(scratchcpp-audio
      PUBLIC
        audioplayerstub.cpp
	audioplayerstub.h
    )
endif()
