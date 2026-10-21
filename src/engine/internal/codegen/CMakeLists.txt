target_sources(scratchcpp
  PRIVATE
    icodebuilder.h
    icodebuilderfactory.h
    codebuilderfactory.cpp
    codebuilderfactory.h
    instruction.h
    instructionlist.h
)

add_subdirectory(llvm)
