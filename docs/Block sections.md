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

Scratch projects are compiled by the \link libscratchcpp::Compiler Compiler \endlink class.
To add a block, you have to let the compiler know how to compile it.
Start by defining a **compile function**.
For example, the compile function for a `hello world` block would look like this:
```cpp
class MySection : public IBlockSection {
    public:
        ...
        static void compileHelloWorld(Compiler *compiler);
        static unsigned int helloWorld(VirtualMachine *vm);
};

void MySection::compileHelloWorld(Compiler *compiler) {
    compiler->addFunctionCall(&helloWorld);
}

unsigned int MySection::helloWorld(VirtualMachine *vm) {
    std::cout << "Hello, world!" << std::endl;
    return 0;
}
```
\note Make sure the functions are **static**.

Register the compile function using the \link libscratchcpp::IBlockSection::addCompileFunction() addCompileFunction() \endlink method in the constructor:
```cpp
MySection::MySection() {
    addCompileFunction("mysection_helloworld", &MySection::compileHelloWorld);
}
```
Where `mysection_helloworld` is the opcode of the `hello world` block.

### Adding inputs
To add inputs, create an `Inputs` enumeration in your block section:
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
The compiler will assign the input name with the `TEXT` ID. In this case, the ID is 0 because it's the first member of the enumeration.

To add the input to the compiled code, call the \link libscratchcpp::Compiler::addInput() addInput() \endlink function:
```cpp
void MySection::compileHelloWorld(Compiler *compiler) {
    compiler->addInput(TEXT);
    compiler->addFunctionCall(&helloWorld);
}
```

The value of the input can be read during runtime using the \link libscratchcpp::VirtualMachine::getArg() getArg() \endlink function:
```cpp
unsigned int MySection::helloWorld(VirtualMachine *vm) {
    std::cout << "Hello, " << vm->getArg(0, 1)->toString() << "!" << std::endl;
    return 1;
}
```
\note The order of the inputs is the same as in the compile function. Do not use the `Inputs` enumeration in runtime functions.

```cpp
vm->getArg(0, 1)
```
The first argument is the index of the input and the second argument is the amount of inputs.
\note Make sure to return the amount of inputs in the `helloWorld` function.

### Adding fields
**Fields** are drop-down menus into which one cannot drop a reporter.
Fields have a predefined set of values.
```cpp
class MySection : public IBlockSection {
    ...
    enum Fields {
        ANIMAL
    };

    enum FieldValues {
        Cat,
        Dog
    };
    ...
};

MySection::MySection() {
    ...
    addField("ANIMAL", ANIMAL);
}
```

Because fields are handled at compile time, you can read them from the compile function:
```cpp
void MySection::compileHelloWorld(Compiler *compiler) {
    int id = compiler->field(ANIMAL)->specialValueId();

    switch(id) {
        case Cat:
            compiler->addFunctionCall(&helloCat);
            break;

        case Dog:
            compiler->addFunctionCall(&helloDog);
            break;

        default:
            break;
    }
}

unsigned int MySection::helloCat(VirtualMachine *vm) {
    std::cout << "Hello, cat!" << std::endl;
    return 0;
}

unsigned int MySection::helloDog(VirtualMachine *vm) {
    std::cout << "Hello, dog!" << std::endl;
    return 0;
}
```
\note Don't confuse \link libscratchcpp::Field::specialValueId() specialValueId() \endlink with \link libscratchcpp::Field::valueId() valueId() \endlink
because \link libscratchcpp::Field::valueId() valueId() \endlink stores the ID of the block, variable, list or broadcast selected in the dropdown list.

To get a pointer to the block, variable, list or broadcast selected in the dropdown list, use \link libscratchcpp::Field::valuePtr() valuePtr() \endlink.

### Registering the block section
Block sections are registered in the \link libscratchcpp::IExtension::registerSections() registerSections() \endlink
function of an extension:

```cpp
void MyExtension::registerSections(Engine *engine) {
    engine->registerSection(std::make_shared<MySection>());
}
```
See the [Extensions](extensions.html) page for instructions on how to create an extension.
