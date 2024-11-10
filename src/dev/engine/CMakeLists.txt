target_sources(scratchcpp
  PRIVATE
    compiler.cpp
    compiler_p.cpp
    compiler_p.h
    executioncontext.cpp
    executioncontext_p.cpp
    executioncontext_p.h
    internal/icodebuilder.h
    internal/icodebuilderfactory.h
    internal/codebuilderfactory.cpp
    internal/codebuilderfactory.h
)

add_subdirectory(internal/llvm)
