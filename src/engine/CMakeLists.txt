target_sources(scratchcpp
  PRIVATE
    virtualmachine.cpp
    virtualmachine_p.cpp
    virtualmachine_p.h
    script.cpp
    script_p.cpp
    script_p.h
    thread.cpp
    thread_p.cpp
    thread_p.h
    internal/engine.cpp
    internal/engine.h
    internal/clock.cpp
    internal/clock.h
    internal/iclock.h
    internal/timer.cpp
    internal/timer.h
    internal/randomgenerator.h
    internal/randomgenerator.cpp
    internal/irandomgenerator.h
)

if(NOT LIBSCRATCHCPP_USE_LLVM)
    target_sources(scratchcpp
      PRIVATE
        compiler.cpp
        compiler_p.cpp
        compiler_p.h
    )
endif()
