\page gettingStarted Getting started

libscratchcpp doesn't have any GUI support, but sprites and the stage can be easily implemented using
the \link libscratchcpp::IStageHandler IStageHandler \endlink and
\link libscratchcpp::ISpriteHandler ISpriteHandler \endlink interfaces.

They can be subclassed for listening to events such as when the X and Y coordinates
of a sprite change.

## CLI-only example
```cpp
#include <scratchcpp/project.h>

int main(int argc, char **argv) {
  libscratchcpp::Project p("/path/to/project.sb3");
  bool ret = p.load();
  if (!ret)
    return 1;

  p.run();
  return 0;
}
```
The \link libscratchcpp::Project::run() run() \endlink method runs an event loop which stops after all scripts finish.

For CLI project players, using \link libscratchcpp::Project::run() run() \endlink is enough. If you are developing
a GUI project player and need to receive input events such as key presses, you'll need to use \link libscratchcpp::IEngine::step() step() \endlink
for ticks and \link libscratchcpp::Project::start() start() \endlink and \link libscratchcpp::Project::stop() stop() \endlink
