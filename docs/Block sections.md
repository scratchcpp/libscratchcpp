\page blockSections Block sections

A block section is a group of Scratch blocks, for example motion blocks.

Block sections are created using the \link libscratchcpp::IBlockSection IBlockSection \endlink interface.

## Creating a block section
To create a custom block section, subclass \link libscratchcpp::IBlockSection IBlockSection \endlink and override
\link libscratchcpp::IBlockSection::name() name() \endlink to return the block section name.

It's recommended to use the libscratchcpp namespace like this in your block section class:
```cpp
using namespace libscratchcpp;
```

### Adding blocks
See the [Scratch Wiki](https://en.scratch-wiki.info/wiki/Scratch_File_Format#Blocks) for more information about blocks.

To add a block, create a block implementation function.
For example, a `print` block would look like this:
```cpp
Value MySection::print(const BlockArgs &args) {
    std::cout << args.input("TEXT").value().toStdString();
    return Value();
}
```
\note You shouldn't find inputs by name because it affects performance. See the "Adding inputs" section below for more information.

Register the block using the \link libscratchcpp::IBlockSection::addBlock() addBlock() \endlink method in the constructor:
```cpp
MySection::MySection() {
    addBlock(this, "mysection_print", &MySection::print);
}
```
Where `mysection_print` is the opcode of the `print` block.

### Adding inputs
To add inputs, create an Inputs enumeration in your block section:
```hpp
class MySection : public IBlockSection {
    enum Inputs {
        TEXT
    };
    ...
};
```
Then add inputs in the constructor:
```cpp
MySection::MySection() {
    ...
    addInput("TEXT", TEXT);
}
```
The compiler will assign the input with the `TEXT` ID. In this case, the ID is 0 because it's the first member of the enumeration.

To use the input, get it using the \link libscratchcpp::BlockArgs::input() input() \endlink function in the block implementation:
```cpp
Value MySection::print(const BlockArgs &args) {
    std::cout << args.input(TEXT).value().toStdString();
    return Value();
}
```
This improves performance because the input is no longer being found by its name, but only by an integer.

### Adding fields
Fields can be added using the same way as inputs:
```cpp
class MySection : public IBlockSection {
    ...
    enum Fields {
        SOME_MENU
    };
    ...
};

MySection::MySection() {
    ...
    addField("SOME_MENU", SOME_MENU);
}
```
Get the value of the field using the \link libscratchcpp::BlockArgs::field() field() \endlink function:
```cpp
Value MySection::someBlock(const BlockArgs &args) {
    std::cout << args.field(SOME_MENU).value().toStdString();
    return Value();
}
```

### Registering the block section
Block sections are registered in the \link libscratchcpp::IExtension::registerSections() registerSections() \endlink
function of an extension:

```cpp
void MyExtension::registerSections(Engine *engine) {
    engine->registerSection(std::make_shared<MySection>());
}
```
See the [Extensions](extensions.html) page for instructions on how to create an extension.
