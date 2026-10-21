target_sources(scratchcpp
  PRIVATE
    icodebuilder.h
    icodebuilderfactory.h
    codebuilderfactory.cpp
    codebuilderfactory.h
    instruction.h
)

add_subdirectory(llvm)
