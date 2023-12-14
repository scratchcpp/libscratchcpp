\page graphicsEffects Graphics effects

libscratchcpp provides API for adding custom graphics effects.
No effects are included with libscratchcpp and must be implemented by the project player.

# Adding a custom graphics effect
To add a graphics effect that libscratchcpp doesn't support,
subclass \link libscratchcpp::IGraphicsEffect IGraphicsEffect \endlink
and override all of the methods.

It's recommended to use the libscratchcpp namespace like this in your class:
```cpp
using namespace libscratchcpp;
```

The `name()` method should return the name of the effect which is
used by the set and change effect blocks, for example `ghost` or `fisheye`.

# Registering the graphics effect
To register the graphics effect, use \link libscratchcpp::ScratchConfiguration::registerGraphicsEffect() ScratchConfiguration::registerGraphicsEffect() \endlink.

```cpp
libscratchcpp::ScratchConfiguration::registerGraphicsEffect(std::make_shared<MyGraphicsEffect>());
```

# Why should effects be registered?
Effects in Scratch are usually identified by their name (`ghost`, `brightness`, etc.), but this isn't effective
when it comes to running projects. Those "names" are therefore replaced by \link libscratchcpp::IGraphicsEffect IGraphicsEffect \endlink
pointers which makes working with effects faster.
