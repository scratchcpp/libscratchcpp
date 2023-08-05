\page extensions Extensions

An extension is a group of block sections (see [Block sections](blockSections.html)).

## Creating an extension
To create a custom extension, subclass \link libscratchcpp::IExtension IExtension \endlink and override
\link libscratchcpp::IExtension::name() name() \endlink and \link libscratchcpp::IExtension::description() description() \endlink
functions to return the name and the description of the extension.

There's also an \link libscratchcpp::IExtension::includeByDefault() includeByDefault() \endlink function which returns `true`
if the extension is intended to be hidden from the block palette and included in a Scratch project by default.

The default implementation returns `false`.

It's recommended to use the libscratchcpp namespace like this in your extension class:
```cpp
using namespace libscratchcpp;
```

### Registering block sections
Block sections can be registered by overriding the \link libscratchcpp::IExtension::registerSections() registerSections() \endlink function:
```cpp
void MyExtension::registerSections(IEngine *engine) {
    engine->registerSection(std::make_shared<MySection>());
}
```
See the [Block sections](blockSections.html) page for instructions on how to create a block section.

### Registering the extension
Register the extension **before** loading a project, using the \link libscratchcpp::ScratchConfiguration ScratchConfiguration \endlink class:
```cpp
libscratchcpp::ScratchConfiguration::registerExtension(std::make_shared<MyExtension>());
```
