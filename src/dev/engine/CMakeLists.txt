target_sources(scratchcpp
  PRIVATE
    compiler.cpp
    compiler_p.cpp
    compiler_p.h
    compilervalue.cpp
    compilervalue_p.cpp
    compilervalue_p.h
    compilerconstant.cpp
    compilerconstant_p.cpp
    compilerconstant_p.h
    executioncontext.cpp
    executioncontext_p.cpp
    executioncontext_p.h
    promise.cpp
    promise_p.cpp
    promise_p.h
    internal/icodebuilder.h
    internal/icodebuilderfactory.h
    internal/codebuilderfactory.cpp
    internal/codebuilderfactory.h
)

add_subdirectory(internal/llvm)
