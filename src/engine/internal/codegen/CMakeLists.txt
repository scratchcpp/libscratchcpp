target_sources(scratchcpp
  PRIVATE
    icodebuilder.h
    icodebuilderfactory.h
    codebuilderfactory.cpp
    codebuilderfactory.h
    instructionbase.h
    instruction.h
    instructionlistbase.cpp
    instructionlistbase.h
    instructionlist.h
    codeanalyzer.cpp
    codeanalyzer.h
    optimizationutils.cpp
    optimizationutils.h
)

if (LIBSCRATCHCPP_CODEGEN_BACKEND STREQUAL "llvm")
  add_subdirectory(llvm)
elseif (LIBSCRATCHCPP_CODEGEN_BACKEND STREQUAL "cpp")
  add_subdirectory(cpp)
endif()
