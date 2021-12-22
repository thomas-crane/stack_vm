#include "svm/svm.h"
#include "svm/err.h"
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
  if (svm->ip >= SVM_MAX_PROGRAM_SIZE) {
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
    case  SVM_INST_ADD:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] += svm->stack[svm->stack_ptr - 1];
      svm->stack_ptr--;
      break;
    case  SVM_INST_SUB:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] -= svm->stack[svm->stack_ptr - 1];
      svm->stack_ptr--;
      break;
    case  SVM_INST_MULT:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] *= svm->stack[svm->stack_ptr - 1];
      svm->stack_ptr--;
      break;
    case  SVM_INST_DIV:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] /= svm->stack[svm->stack_ptr - 1];
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
    for (uint64_t i = svm->stack_ptr - 1; i > 0; i--) {
      printf("  %ld\n", svm->stack[i]);
    }
    printf("  %ld\n", svm->stack[0]);
  }
}

int main(void)
{
  svm_t svm;
  svm_init(&svm);

  svm_instruction_t program[] = {
    {.type = SVM_INST_PUSH, .operand = 20},
    {.type = SVM_INST_PUSH, .operand = 30},
    {.type = SVM_INST_MULT, },
    {.type = SVM_INST_HALT, },
  };
  svm_load_program_from_array(&svm, program, sizeof(program) / sizeof(*program));

  svm_err_t result = svm_run(&svm);
  printf("VM finished with: %s\n", svm_err_to_string(result));
  svm_print_stack(&svm);

  return 0;
}
