option(LIBSCRATCHCPP_AUDIO_SUPPORT "Audio support" ON)

add_library(scratchcpp-audio STATIC)
set(MINIAUDIO_SRC internal/thirdparty/miniaudio)

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
    iaudiooutput.h
    audiooutput.cpp
    audiooutput.h
    iaudioloudness.h
)

if (LIBSCRATCHCPP_AUDIO_SUPPORT)
    target_sources(scratchcpp-audio
      PUBLIC
        internal/audioengine.cpp
        internal/audioengine.h
        internal/audioplayer.cpp
        internal/audioplayer.h
        internal/audioloudness.cpp
        internal/audioloudness.h
    )
else()
    target_sources(scratchcpp-audio
      PUBLIC
        internal/audioplayerstub.cpp
        internal/audioplayerstub.h
        internal/audioloudnessstub.cpp
        internal/audioloudnessstub.h
    )
endif()
