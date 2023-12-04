target_sources(scratchcpp
  PRIVATE
    iprojectreader.h
    scratch3reader.cpp
    scratch3reader.h
    reader_common.h
    zipreader.cpp
    zipreader.h
    projecturl.cpp
    projecturl.h
    idownloader.h
)

if (LIBSCRATCHCPP_NETWORK_SUPPORT)
    target_sources(scratchcpp
      PRIVATE
        downloader.cpp
	downloader.h
    )
endif()
