set(ZIP_SRC ${PROJECT_SOURCE_DIR}/thirdparty/zip/src)

add_library(zip SHARED
    ${ZIP_SRC}/zip.c
    ${ZIP_SRC}/zip.h
    ${ZIP_SRC}/miniz.h
)

target_include_directories(zip PUBLIC ${ZIP_SRC})
