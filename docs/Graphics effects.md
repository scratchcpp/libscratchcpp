\page graphicsEffects Graphics effects

libscratchcpp provides API for implementing custom graphics effects.
No effects are currently included with libscratchcpp, but that is planned to change.

In case you've implemented some of the effects yourself, feel free to make pull requests!
These are the issues (tasks) for the default graphics effects:

- Color: https://github.com/scratchcpp/libscratchcpp/issues/283
- Fisheye: https://github.com/scratchcpp/libscratchcpp/issues/284
- Whirl: https://github.com/scratchcpp/libscratchcpp/issues/285
- Pixelate: https://github.com/scratchcpp/libscratchcpp/issues/286
- Mosaic: https://github.com/scratchcpp/libscratchcpp/issues/287
- Brightness: https://github.com/scratchcpp/libscratchcpp/issues/288
- Ghost: https://github.com/scratchcpp/libscratchcpp/issues/289

# Implementing a graphics effect
To implement a graphics effect that libscratchcpp doesn't support,
subclass \link libscratchcpp::IGraphicsEffect IGraphicsEffect \endlink
and override all of the methods.

It's recommended to use the libscratchcpp namespace like this in your class:
```cpp
using namespace libscratchcpp;
```

The `name()` method should return the name of the effect which is
used by the set and change effect blocks, for example `ghost` or `fisheye`.

The `apply()` method is used to apply the effect on a bitmap. The bitmap
can be accessed using the `bitmap` parameter and is writable.

# Registering the graphics effect
To register the graphics effect, use \link libscratchcpp::ScratchConfiguration::registerGraphicsEffect() ScratchConfiguration::registerGraphicsEffect() \endlink.

```cpp
libscratchcpp::ScratchConfiguration::registerGraphicsEffect(std::make_shared<MyGraphicsEffect>());
```
