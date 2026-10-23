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
)

add_subdirectory(llvm)
