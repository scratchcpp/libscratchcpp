target_sources(scratchcpp
  PRIVATE
    cppprelude.h
    cppruntime.cpp
    cppruntime.h
    cppcodebuilder.cpp
    cppcodebuilder.h
    cppcodebuilderfactory.cpp
    cppcodebuilderfactory.h
    cppregister.h
    cppcompilercontext.cpp
    cppcompilercontext.h
    cppexecutablecode.cpp
    cppexecutablecode.h
    cppexecutioncontext.cpp
    cppexecutioncontext.h
)

# Embed the runtime prelude into the library, it's a part of the generated code
set(CPP_PRELUDE_HEADER ${CMAKE_CURRENT_BINARY_DIR}/cppprelude_source.h)
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/cppprelude.h CPP_PRELUDE_CONTENT)
file(WRITE ${CPP_PRELUDE_HEADER} "// Generated from cppprelude.h\n#pragma once\n\nstatic const char *CPP_PRELUDE_SOURCE = R\"scpp_raw(${CPP_PRELUDE_CONTENT})scpp_raw\";\n")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cppprelude.h)

target_include_directories(scratchcpp PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
