# Stack VM

A stack based VM written in C.

## Foreword

This project is just for fun. There are likely to be a few nasty bugs, and there are certainly many missing features that would be needed to actually use this as a backend for a language.

## Building

```shell
# Normal build.
$ make

# Release build.
$ make release

# Clean build artefacts.
$ make clean
```

## Usage

First, you'll need an SVM assembly file to assemble. There are some [examples](examples/) in this repo.

```shell
$ cat example.svma
push 10
push 20
addi
halt
```

Use the `svmasm` binary to assemble the file.

```shell
$ svmasm example.svma
$ ls
example.svma  example.svmo
```

Now you can run your SVM object file using the `svm` binary.

```shell
$ svm example.svmo
Stack:
  i64: 30 | u64: 30 | f64: 0.000000 | ptr: 0x1e
```

## Design

Things that are design goals for Stack VM:

+ Be relatively easy to target for a compiler backend.

Things that are *not* design goals:

+ Be performant
+ Strictly behave like a "real VM" or try to be as close to bare metal instructions as possible.

Stack VM is a fairly standard run-of-the-mill virtual machine that uses a stack as the main working space for your programs.

The "word size" for Stack VM is 64 bits. This means that all values on the stack are 64 bits, each instruction byte in an object file is 64 bits, etc.

Internally, Stack VM has a few stacks that are used for various purposes:

+ The "main stack". This is where your data goes if you use `push` or `copy`, etc.
+ The call stack. This stores return addresses for function calls so that the programmer doesn't have to worry about manually handling return addresses.
+ The heap address stack. This is used to store addresses that have been allocated using `alloc`.
+ The instruction stack. Used to store the actual program.

A more "bare metal" VM may only use a single stack, which is certainly possible, but places a bit more burden on the programmer who is writing the assembly (or the compiler backend).

## Instruction set

### Misc instructions

| Mnemonic | Operands | Description                    |
| -------- | -------- | ------------------------------ |
| `nop`    | None     | Do nothing.                    |
| `halt`   | None     | Stop execution of the program. |

### Stack instructions

| Mnemonic | Operands | Description                                                                     |
| -------- | -------- | ------------------------------------------------------------------------------- |
| `push`   | `value`  | Push `value` onto the top of the stack.                                         |
| `pop`    | None     | Remove the item at the top of the stack.                                        |
| `copy`   | `offset` | Copy the item at `top_of_stack - offset` to the top of the stack.               |
| `swap`   | `offset` | Swap the item at `top_of_stack - offset` with the item at the top of the stack. |

### Arithmetic

Each arithmetic instruction has 3 versions with the suffixes `i`, `u` or `f` (e.g. `addi` or `subf`). The suffix indicates how the operands will be treated.

| Suffix | Operand data type             |
| ------ | ----------------------------- |
| `i`    | 64-bit signed int.            |
| `u`    | 64-bit unsigned int.          |
| `f`    | 64-bit floating point number. |

| Mnemonic                  | Operands | Description                         |
| ------------------------- | -------- | ----------------------------------- |
| `addi`, `addu`, `addf`    | None     | `b = pop(), a = pop(), push(a + b)` |
| `subi`, `subu`, `subf`    | None     | `b = pop(), a = pop(), push(a - b)` |
| `multi`, `multu`, `multf` | None     | `b = pop(), a = pop(), push(a * b)` |
| `divi`, `divu`, `divf`    | None     | `b = pop(), a = pop(), push(a / b)` |

### Comparison

All of the comparison instructions **with the exception of `eq` and `neq`** have 3 versions for each data type, similar to the arithmetic instructions.

| Mnemonic               | Operands | Description                          |
| ---------------------- | -------- | ------------------------------------ |
| `eq`                   | None     | `b = pop(), a = pop(), push(a == b)` |
| `neq`                  | None     | `b = pop(), a = pop(), push(a != b)` |
| `gti`, `gtu`, `gtf`    | None     | `b = pop(), a = pop(), push(a > b)`  |
| `gtei`, `gteu`, `gtef` | None     | `b = pop(), a = pop(), push(a >= b)` |
| `lti`, `ltu`, `ltf`    | None     | `b = pop(), a = pop(), push(a < b)`  |
| `ltei`, `lteu`, `ltef` | None     | `b = pop(), a = pop(), push(a <= b)` |

### Jumps

| Mnemonic | Operands | Description                                                                                                |
| -------- | -------- | ---------------------------------------------------------------------------------------------------------- |
| `jmp`    | `label`  | Continue execution from the given `label`                                                                  |
| `jnz`    | `label`  | `a = pop()`, if `a` is `0`, continue from the next instruction. Otherwise continue from the given `label`. |

### Functions

| Mnemonic | Operands | Description                                                        |
| -------- | -------- | ------------------------------------------------------------------ |
| `call`   | `label`  | Push the return address onto the call stack, then jump to `label`. |
| `ret`    | None     | Pop a return address off the call stack and jump back to it.       |

### Memory

| Mnemonic | Operands    | Description                                                                                      |
| -------- | ----------- | ------------------------------------------------------------------------------------------------ |
| `alloc`  | `num_bytes` | Allocate enough memory to store `num_bytes` bytes and push the allocated address onto the stack. |
| `free`   | None        | `addr = pop()`, free the memory at `addr`.                                                       |
| `read`   | None        | `addr = pop()`, dereference `addr` and put the value onto the stack.                             |
| `write`  | None        | `value = pop(), addr = pop()`, set the value pointed to by `addr` to `value`.                    |


## Acknowledgements

This project is largely inspired by Alexey Kutepov's (better known as tsoding) [bm](https://github.com/tsoding/bm) project.
