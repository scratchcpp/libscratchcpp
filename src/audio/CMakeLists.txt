option(LIBSCRATCHCPP_AUDIO_SUPPORT "Audio support" ON)

add_library(scratchcpp-audio SHARED)
set(MINIAUDIO_SRC internal/thirdparty/miniaudio)

if (LIBSCRATCHCPP_AUDIO_SUPPORT)
    add_library(miniaudio SHARED
	    ${MINIAUDIO_SRC}/miniaudio.c
	    ${MINIAUDIO_SRC}/miniaudio.h
    )
    target_include_directories(scratchcpp-audio PUBLIC ${CMAKE_CURRENT_LIST_DIR}/${MINIAUDIO_SRC})
    install(TARGETS miniaudio DESTINATION lib)

    target_link_libraries(scratchcpp-audio PUBLIC miniaudio)
    target_compile_definitions(scratchcpp-audio PUBLIC LIBSCRATCHCPP_AUDIO_SUPPORT)
endif()

target_sources(scratchcpp-audio
  PUBLIC
    iaudioengine.h
    iaudioplayer.h
    iaudiooutput.h
    audiooutput.cpp
    audiooutput.h
    iaudioinput.h
    audioinput.h
    audioinput.cpp
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
        internal/audioenginestub.cpp
        internal/audioenginestub.h
        internal/audioplayerstub.cpp
        internal/audioplayerstub.h
        internal/audioloudnessstub.cpp
        internal/audioloudnessstub.h
    )
endif()

install(TARGETS scratchcpp-audio DESTINATION lib)
