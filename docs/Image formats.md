\page imageFormats Image formats

To work with costumes, libscratchcpp has to read image files from Scratch projects.
Only JPEG is currently supported, but support for PNG is coming soon.

# Implementing a custom image format
To implement a custom image format that libscratchcpp doesn't support,
subclass \link libscratchcpp::IImageFormat IImageFormat \endlink
and override all of the methods.

Then subclass \link libscratchcpp::IImageFormatFactory IImageFormatFactory \endlink
and override \link libscratchcpp::IImageFormatFactory::createInstance() createInstance() \endlink.

It's recommended to use the libscratchcpp namespace like this in your classes:
```cpp
using namespace libscratchcpp;
```

Here's an example of the `createInstance()` method:
```cpp
std::shared_ptr<IImageFormat> TiffImageFormatFactory::createInstance() {
    return std::make_shared<TiffImageFormat>();
}
```

# Registering the image format
To register the image format, you need to register the factory class.
It will be used by classes such as \link libscratchcpp::Costume Costume \endlink
internally to instantiate your image format.

```cpp
libscratchcpp::ScratchConfiguration::registerImageFormat("tiff", std::make_shared<TiffImageFormatFactory>());
```
