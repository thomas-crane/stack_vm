# Stack VM

A stack based VM written in C.

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
