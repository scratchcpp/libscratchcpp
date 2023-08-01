\page gettingStarted Getting started

libscratchcpp doesn't have any GUI support, but sprites and the stage can be easily implemented using
the \link libscratchcpp::IScratchStage IScratchStage \endlink and
\link libscratchcpp::IScratchTarget IScratchTarget \endlink interfaces.

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
a GUI project player and need to receive input events such as key presses, you'll need to implement your own event loop
or use the one provided by the GUI framework.

## GUI example (Qt)
Qt provides an event loop which can be easily used to run frames with a specific frame rate.

For example, a `Player` class which **inherits from QObject** can use a timer:
```cpp
#include <QObject>
#include <scratchcpp/project.h>

class Player : public QObject {
    Q_OBJECT
    public:
        Player(QObject *parent = nullptr);
        Player(const Player &) = delete; // Project is not copyable

    protected:
        void timerEvent(QTimerEvent *event) override;

    private:
        libscratchcpp::Project m_proj;
}

Player::Player(QObject *parent) {
    m_proj.setFileName("/path/to/project.sb3");
    m_proj.start();
    int fps = 30;
    startTimer(1000.0 / fps);
}

void Player::timerEvent(QTimerEvent *event) {
    m_proj.frame();
    event->accept();
}
```
