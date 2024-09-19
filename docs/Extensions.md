\page extensions Extensions

An extension is a group of Scratch blocks, for example motion blocks.

## Creating an extension
To create a custom extension, subclass \link libscratchcpp::IExtension IExtension \endlink and override
\link libscratchcpp::IExtension::name() name() \endlink and \link libscratchcpp::IExtension::description() description() \endlink
functions to return the name and the description of the extension. Then override \link libscratchcpp::IExtension::registerBlocks() registerBlocks() \endlink
where you'll register the compile functions for blocks.

It's recommended to use the libscratchcpp namespace like this in your extension class:
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
class MyExtension : public IExtension {
    public:
        ...
        static void compileHelloWorld(Compiler *compiler);
        static unsigned int helloWorld(VirtualMachine *vm);
};

void MyExtension::compileHelloWorld(Compiler *compiler) {
    compiler->addFunctionCall(&helloWorld);
}

unsigned int MyExtension::helloWorld(VirtualMachine *vm) {
    std::cout << "Hello, world!" << std::endl;
    return 0;
}
```
\note Make sure the functions are **static**.

Register the compile function using the \link libscratchcpp::IEngine::addCompileFunction() addCompileFunction() \endlink method in link libscratchcpp::IExtension::registerBlocks() registerBlocks() \endlink:
```cpp
MyExtension::registerBlocks(IEngine *engine) {
    engine->addCompileFunction(this, "myextension_helloworld", &MyExtension::compileHelloWorld);
}
```
Where `myextension_helloworld` is the opcode of the `hello world` block.

### Adding inputs
To add inputs, create an `Inputs` enumeration in your extension:
```hpp
class MyExtension : public IExtension {
    enum Inputs {
        TEXT
    };
    ...
};
```
Then add inputs in link libscratchcpp::IExtension::registerBlocks() registerBlocks() \endlink:
```cpp
MyExtension::registerBlocks(IEngine *engine) {
    ...
    engine->addInput(this, "TEXT", TEXT);
}
```
The compiler will assign the input name with the `TEXT` ID. In this case, the ID is 0 because it's the first member of the enumeration.

To add the input to the compiled code, call the \link libscratchcpp::Compiler::addInput() addInput() \endlink function:
```cpp
void MyExtension::compileHelloWorld(Compiler *compiler) {
    compiler->addInput(TEXT);
    compiler->addFunctionCall(&helloWorld);
}
```

The value of the input can be read during runtime using the \link libscratchcpp::VirtualMachine::getInput() getInput() \endlink function:
```cpp
unsigned int MyExtension::helloWorld(VirtualMachine *vm) {
    std::cout << "Hello, " << vm->getInput(0, 1)->toString() << "!" << std::endl;
    return 1;
}
```
\note The order of the inputs is the same as in the compile function. Do not use the `Inputs` enumeration in runtime functions.

```cpp
vm->getInput(0, 1)
```
The first argument is the index of the input and the second argument is the amount of inputs.
\note Make sure to return the amount of inputs in the `helloWorld` function.

### Adding fields
**Fields** are drop-down menus into which one cannot drop a reporter.
Fields have a predefined set of values.
```cpp
class MyExtension : public IExtension {
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

MyExtension::registerBlocks(IEngine *engine) {
    ...
    engine->addField(this, "ANIMAL", ANIMAL);
    engine->addFieldValue(this, "Cat", Cat);
    engine->addFieldValue(this, "Dog", Dog);
}
```

Because fields are handled at compile time, you can read them from the compile function:
```cpp
void MyExtension::compileHelloWorld(Compiler *compiler) {
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

unsigned int MyExtension::helloCat(VirtualMachine *vm) {
    std::cout << "Hello, cat!" << std::endl;
    return 0;
}

unsigned int MyExtension::helloDog(VirtualMachine *vm) {
    std::cout << "Hello, dog!" << std::endl;
    return 0;
}
```
\note Don't confuse \link libscratchcpp::Field::specialValueId() specialValueId() \endlink with \link libscratchcpp::Field::valueId() valueId() \endlink
because \link libscratchcpp::Field::valueId() valueId() \endlink stores the ID of the block, variable, list or broadcast selected in the dropdown list.

To get a pointer to the block, variable, list or broadcast selected in the dropdown list, use \link libscratchcpp::Field::valuePtr() valuePtr() \endlink.

### Registering the extension
Register the extension **before** loading a project, using the \link libscratchcpp::ScratchConfiguration ScratchConfiguration \endlink class:
```cpp
libscratchcpp::ScratchConfiguration::registerExtension(std::make_shared<MyExtension>());
```
