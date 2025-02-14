add_library(cpp-unicodelib INTERFACE
    unicodelib_encodings.h
    unicodelib_names.h
    unicodelib.h
)

target_include_directories(cpp-unicodelib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
install(TARGETS cpp-unicodelib DESTINATION include)
