#include "svm/svm.h"
#include "svm/err.h"
#include "svm/value.h"
#include "svm/instructions.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void svm_init(svm_t *svm)
{
  svm->halted = false;

  memset(svm->stack, 0, sizeof(svm->stack));
  svm->stack_ptr = 0;
  
  memset(svm->program, 0, sizeof(svm->program));
  svm->ip = 0;
}

bool svm_load_program_from_array(svm_t *svm, svm_instruction_t *instructions, uint32_t program_size)
{
  if (program_size > SVM_MAX_PROGRAM_SIZE) {
    return false;
  }

  svm->program_size = program_size;
  memcpy(svm->program, instructions, program_size * sizeof(*instructions));
  return true;
}

svm_err_t svm_exec_instruction(svm_t *svm)
{
  if (svm->ip >= svm->program_size) {
    return SVM_ERR_IP_OVERFLOW;
  }
  svm_instruction_t instruction = svm->program[svm->ip];
  svm->ip++;

  switch (instruction.type) {
    case  SVM_INST_NOP:
      break;
    case  SVM_INST_HALT:
      svm->halted = true;
      break;
    case  SVM_INST_PUSH:
      if (svm->stack_ptr >= SVM_STACK_SIZE) {
        return SVM_ERR_STACK_OVERFLOW;
      }
      svm->stack[svm->stack_ptr++] = instruction.operand;
      break;
    case  SVM_INST_POP:
      if (svm->stack_ptr == 0) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack_ptr--;
      break;
    case SVM_INST_DUP:
      if (svm->stack_ptr < 1) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr] = svm->stack[svm->stack_ptr - 1];
      svm->stack_ptr++;
      break;
    case  SVM_INST_ADD:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_i64 += svm->stack[svm->stack_ptr - 1].as_i64;
      svm->stack_ptr--;
      break;
    case  SVM_INST_SUB:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_i64 -= svm->stack[svm->stack_ptr - 1].as_i64;
      svm->stack_ptr--;
      break;
    case  SVM_INST_MULT:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_i64 *= svm->stack[svm->stack_ptr - 1].as_i64;
      svm->stack_ptr--;
      break;
    case  SVM_INST_DIV:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_i64 /= svm->stack[svm->stack_ptr - 1].as_i64;
      svm->stack_ptr--;
      break;
    case SVM_INST_EQ:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_ptr == svm->stack[svm->stack_ptr - 1].as_ptr);
      svm->stack_ptr--;
      break;
    case SVM_INST_NOT_EQ:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_ptr != svm->stack[svm->stack_ptr - 1].as_ptr);
      svm->stack_ptr--;
      break;
    case SVM_INST_GT:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_i64 > svm->stack[svm->stack_ptr - 1].as_i64);
      svm->stack_ptr--;
      break;
    case SVM_INST_GT_EQ:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_i64 >= svm->stack[svm->stack_ptr - 1].as_i64);
      svm->stack_ptr--;
      break;
    case SVM_INST_LT:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_i64 < svm->stack[svm->stack_ptr - 1].as_i64);
      svm->stack_ptr--;
      break;
    case SVM_INST_LT_EQ:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_i64 <= svm->stack[svm->stack_ptr - 1].as_i64);
      svm->stack_ptr--;
      break;
    case SVM_INST_JMP:
      // Don't worry about checking bounds here because if the ip goes beyond the program size it will be caught in the
      // next call to svm_exec_instruction.
      svm->ip = instruction.operand.as_u64;
      break;
    case SVM_INST_JNZ:
      if (svm->stack_ptr < 1) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      if (svm->stack[svm->stack_ptr - 1].as_i64 != 0) {
        // See above note.
        svm->ip = instruction.operand.as_u64;
      }
      svm->stack_ptr--;
      break;
    default:
      return SVM_ERR_ILLEGAL_INSTRUCTION;
      break;
  }

  return SVM_ERR_OK;
}

svm_err_t svm_run(svm_t *svm) {
  while (!svm->halted) {
    svm_err_t err = svm_exec_instruction(svm);
    if (err != SVM_ERR_OK) {
      return err;
    }
  }
  return SVM_ERR_OK;
}

void svm_print_stack(svm_t *svm)
{
  printf("Stack: \n");
  if (svm->stack_ptr == 0) {
    printf("  [empty]\n");
  } else {
    uint64_t cnt = svm->stack_ptr;
    do {
      cnt--;
      svm_value_t value = svm->stack[cnt];
      printf("  i64: %ld | u64: %lu | f64: %f | ptr: %p\n", value.as_i64, value.as_u64, value.as_f64, value.as_ptr);
    } while (cnt != 0);
  }
}

int main(void)
{
  svm_t svm;
  svm_init(&svm);

  svm_instruction_t program[] = {
    // Counter.
    {.type = SVM_INST_PUSH, .operand = SVM_VALUE_I64(0)},

    // Add one.
    {.type = SVM_INST_PUSH, .operand = SVM_VALUE_I64(1)},
    {.type = SVM_INST_ADD, },

    // Check if less than 10.
    {.type = SVM_INST_DUP, },
    {.type = SVM_INST_PUSH, .operand = SVM_VALUE_I64(10)},
    {.type = SVM_INST_LT, },

    // Go back to the start if it is.
    {.type = SVM_INST_JNZ, .operand = SVM_VALUE_U64(1)},

    {.type = SVM_INST_HALT, },
  };
  svm_load_program_from_array(&svm, program, sizeof(program) / sizeof(*program));

  svm_err_t result = svm_run(&svm);
  printf("VM finished with: %s\n", svm_err_to_string(result));
  svm_print_stack(&svm);

  return 0;
}
