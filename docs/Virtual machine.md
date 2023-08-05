\page virtualMachine Virtual machine

Scratch projects are compiled to bytecode which is interpreted by the
\link libscratchcpp::VirtualMachine VirtualMachine \endlink class.

The opcodes are defined in the \link libscratchcpp::vm::Opcode Opcode \endlink enumeration.

## Start instruction

The first instruction of the bytecode is \link libscratchcpp::vm::OP_START OP_START \endlink.

## End instruction
The bytecode ends with the \link libscratchcpp::vm::OP_HALT OP_HALT \endlink opcode
which can be also used to stop the script from anywhere in the code.

## Constant values
To load a constant value, the \link libscratchcpp::vm::OP_CONST OP_CONST \endlink opcode can
be used.
```
OP_CONST
0
```
`0` is the index of the constant value.
The array of constant values can be set using \link libscratchcpp::VirtualMachine::setConstValues setConstValues() \endlink

To load a null value (zero), use \link libscratchcpp::vm::OP_NULL OP_NULL \endlink without any arguments.

## If statements
If statements can be implemented using \link libscratchcpp::vm::OP_IF OP_IF \endlink,
\link libscratchcpp::vm::OP_ELSE OP_ELSE \endlink and \link libscratchcpp::vm::OP_ENDIF OP_ENDIF \endlink.
```
(evaluate the condition here)
OP_IF
...
OP_ELSE
...
OP_ENDIF
```
\note \link libscratchcpp::vm::OP_ELSE OP_ELSE \endlink is optional.

## Loops
All loops end with \link libscratchcpp::vm::OP_LOOP_END OP_LOOP_END \endlink.

### Repeat n times
The repeat loop can be implemented using \link libscratchcpp::vm::OP_REPEAT_LOOP OP_REPEAT_LOOP \endlink.
```
(load the amount of periods here)
OP_REPEAT_LOOP
...
OP_LOOP_END
```

### Repeat until
Scratch has a while loop, but until is used more frequently, so the VM doesn't support the while loop.

The repeat until loop can be implemented using \link libscratchcpp::vm::OP_UNTIL_LOOP OP_UNTIL_LOOP \endlink
and \link libscratchcpp::vm::OP_BEGIN_UNTIL_LOOP OP_BEGIN_UNTIL_LOOP \endlink.
```
OP_UNTIL_LOOP
(evaluate the condition here)
OP_BEGIN_UNTIL_LOOP
...
OP_LOOP_END
```
\note The condition is evaluated after \link libscratchcpp::vm::OP_UNTIL_LOOP OP_UNTIL_LOOP \endlink and the loop
starts with \link libscratchcpp::vm::OP_BEGIN_UNTIL_LOOP OP_BEGIN_UNTIL_LOOP \endlink.

### Forever
The forever loop can be implemented using \link libscratchcpp::vm::OP_FOREVER_LOOP OP_FOREVER_LOOP \endlink.
```
OP_FOREVER_LOOP
...
OP_LOOP_END
```

This is equivalent to:
```
OP_UNTIL_LOOP
OP_NULL
OP_BEGIN_UNTIL_LOOP
...
OP_LOOP_END
```
\link libscratchcpp::vm::OP_NULL OP_NULL \endlink will be evaluated as `false`.

## Operators
### Arithmetic operators
- \link libscratchcpp::vm::OP_ADD OP_ADD \endlink
- \link libscratchcpp::vm::OP_SUBTRACT OP_SUBTRACT \endlink
- \link libscratchcpp::vm::OP_MULTIPLY OP_MULTIPLY \endlink
- \link libscratchcpp::vm::OP_DIVIDE OP_DIVIDE \endlink
- \link libscratchcpp::vm::OP_MOD OP_MOD \endlink

### Relational operators
- \link libscratchcpp::vm::OP_GREATER_THAN OP_GREATER_THAN \endlink
- \link libscratchcpp::vm::OP_LESS_THAN OP_LESS_THAN \endlink
- \link libscratchcpp::vm::OP_EQUALS OP_EQUALS \endlink

### Logical operators
- \link libscratchcpp::vm::OP_AND OP_ADD \endlink
- \link libscratchcpp::vm::OP_OR OP_OR \endlink
- \link libscratchcpp::vm::OP_NOT OP_NOT \endlink

## Example
Add 5 and 3:
```
OP_CONST
0
OP_CONST
1
OP_ADD
(use the result in another instruction)
```
List of constant values: `5`, `3`

## Variables
Variables are stored in an array similar to constant values.

It can be set using \link libscratchcpp::VirtualMachine::setVariables setVariables() \endlink.

Set variable at index 0 to loaded value:
```
(load the value here)
OP_SET_VAR
0
```

Change variable at index 0 by loaded value:
```
(load the value here)
OP_CHANGE_VAR
0
```

Read the variable at index 0:
```
OP_READ_VAR
0
(use the value in another instruction)
```

## Lists
Lists are stored in an array just like variables. The array can be set using \link libscratchcpp::VirtualMachine::setLists setLists() \endlink.

Opcodes for lists can be found in the \link libscratchcpp::vm::Opcode Opcode \endlink enumeration.

## Custom functions
Custom functions can be called using \link libscratchcpp::vm::OP_EXEC OP_EXEC \endlink.

Function pointers are stored in an array which can be set using \link libscratchcpp::VirtualMachine::setFunctions setFunctions() \endlink.

Example: print function
```
OP_CONST
0
OP_EXEC
0
```

```cpp
unsigned int print(VirtualMachine *vm) {
    std::cout << vm->getInput(0, 1)->toString() << std::endl;
    return 1;
}
```

`getInput(0, 1)` returns the value at index 0 and `1` is the number of arguments

The function returns the number of arguments.

\note This is just an example. The \link libscratchcpp::vm::OP_PRINT OP_PRINT \endlink opcode can be used to print a string.
