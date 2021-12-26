#include "svm/svm.h"
#include "svm/err.h"
#include "svm/value.h"
#include "svm/instructions.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

static void usage()
{
  fprintf(stderr, "Usage: svm [FILE]\n");
  fprintf(stderr, "Run the given binary file on the SVM.\n");
}

static bool find_addr(svm_t *svm, void *addr, uint64_t *idx)
{
  for (uint64_t i = 0; i < SVM_HEAP_ADDRS_SIZE; i++) {
    // If we get to a NULL addr we can stop.
    if (svm->heap_addrs[i] == NULL) {
      return false;
    }
    if (svm->heap_addrs[i] == addr) {
      *idx = i;
      return true;
    }
  }
  return false;
}

void svm_init(svm_t *svm)
{
  svm->halted = false;

  memset(svm->stack, 0, sizeof(svm->stack));
  svm->stack_ptr = 0;

  memset(svm->program, 0, sizeof(svm->program));
  svm->program_size = 0;
  svm->ip = 0;

  memset(svm->call_stack, 0, sizeof(svm->call_stack));
  svm->call_stack_ptr = 0;

  memset(svm->heap_addrs, 0, sizeof(svm->heap_addrs));
  svm->heap_addrs_ptr = 0;
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

bool svm_load_program_from_file(svm_t *svm, const char *file_name)
{
  FILE *fd;

  fd = fopen(file_name, "r");
  if (fd == NULL) {
    fprintf(stderr, "Error: Cannot open '%s'\n", file_name);
    return false;
  }

  uint64_t cnt = 0;
  while (true) {
    uint64_t type_value;
    size_t num_read = fread(&type_value, 1, sizeof(type_value), fd);
    if (num_read < 1) {
      break;
    }

    svm_instruction_type_t type = (svm_instruction_type_t)type_value;
    svm_value_t operand;
    if (svm_instruction_type_needs_operand(type)) {
      num_read = fread(&operand, 1, sizeof(operand), fd);
      if (num_read < 1) {
        break;
      }
    }

    svm->program[cnt] = (svm_instruction_t){.type = type, .operand = operand};
    cnt++;
  }
  svm->program_size = cnt;

  fclose(fd);
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
    case SVM_INST_COPY:
      if (svm->stack_ptr >= SVM_STACK_SIZE) {
        return SVM_ERR_STACK_OVERFLOW;
      }
      if (svm->stack_ptr < instruction.operand.as_u64) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      if (instruction.operand.as_u64 == 0) {
        // stack_ptr points above the top of the stack so an offset of 0 is an overflow.
        return SVM_ERR_STACK_OVERFLOW;
      }
      svm->stack[svm->stack_ptr] = svm->stack[svm->stack_ptr - instruction.operand.as_u64];
      svm->stack_ptr++;
      break;
    case SVM_INST_SWAP:
      if (svm->stack_ptr < instruction.operand.as_u64) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      if (instruction.operand.as_u64 == 0) {
        // stack_ptr points above the top of the stack so an offset of 0 is an overflow.
        return SVM_ERR_STACK_OVERFLOW;
      }
      svm_value_t tmp = svm->stack[svm->stack_ptr - 1];
      svm->stack[svm->stack_ptr - 1] = svm->stack[svm->stack_ptr - instruction.operand.as_u64 - 1];
      svm->stack[svm->stack_ptr - instruction.operand.as_u64 - 1] = tmp;
      break;
    case  SVM_INST_ADD_I:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_i64 += svm->stack[svm->stack_ptr - 1].as_i64;
      svm->stack_ptr--;
      break;
    case  SVM_INST_SUB_I:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_i64 -= svm->stack[svm->stack_ptr - 1].as_i64;
      svm->stack_ptr--;
      break;
    case  SVM_INST_MULT_I:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_i64 *= svm->stack[svm->stack_ptr - 1].as_i64;
      svm->stack_ptr--;
      break;
    case  SVM_INST_DIV_I:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_i64 /= svm->stack[svm->stack_ptr - 1].as_i64;
      svm->stack_ptr--;
      break;
    case  SVM_INST_ADD_U:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_u64 += svm->stack[svm->stack_ptr - 1].as_u64;
      svm->stack_ptr--;
      break;
    case  SVM_INST_SUB_U:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_u64 -= svm->stack[svm->stack_ptr - 1].as_u64;
      svm->stack_ptr--;
      break;
    case  SVM_INST_MULT_U:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_u64 *= svm->stack[svm->stack_ptr - 1].as_u64;
      svm->stack_ptr--;
      break;
    case  SVM_INST_DIV_U:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_u64 /= svm->stack[svm->stack_ptr - 1].as_u64;
      svm->stack_ptr--;
      break;
    case  SVM_INST_ADD_F:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_f64 += svm->stack[svm->stack_ptr - 1].as_f64;
      svm->stack_ptr--;
      break;
    case  SVM_INST_SUB_F:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_f64 -= svm->stack[svm->stack_ptr - 1].as_f64;
      svm->stack_ptr--;
      break;
    case  SVM_INST_MULT_F:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_f64 *= svm->stack[svm->stack_ptr - 1].as_f64;
      svm->stack_ptr--;
      break;
    case  SVM_INST_DIV_F:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2].as_f64 /= svm->stack[svm->stack_ptr - 1].as_f64;
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
    case SVM_INST_GT_I:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_i64 > svm->stack[svm->stack_ptr - 1].as_i64);
      svm->stack_ptr--;
      break;
    case SVM_INST_GT_EQ_I:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_i64 >= svm->stack[svm->stack_ptr - 1].as_i64);
      svm->stack_ptr--;
      break;
    case SVM_INST_LT_I:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_i64 < svm->stack[svm->stack_ptr - 1].as_i64);
      svm->stack_ptr--;
      break;
    case SVM_INST_LT_EQ_I:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_i64 <= svm->stack[svm->stack_ptr - 1].as_i64);
      svm->stack_ptr--;
      break;
    case SVM_INST_GT_U:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_u64 > svm->stack[svm->stack_ptr - 1].as_u64);
      svm->stack_ptr--;
      break;
    case SVM_INST_GT_EQ_U:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_u64 >= svm->stack[svm->stack_ptr - 1].as_u64);
      svm->stack_ptr--;
      break;
    case SVM_INST_LT_U:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_u64 < svm->stack[svm->stack_ptr - 1].as_u64);
      svm->stack_ptr--;
      break;
    case SVM_INST_LT_EQ_U:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_u64 <= svm->stack[svm->stack_ptr - 1].as_u64);
      svm->stack_ptr--;
      break;
    case SVM_INST_GT_F:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_f64 > svm->stack[svm->stack_ptr - 1].as_f64);
      svm->stack_ptr--;
      break;
    case SVM_INST_GT_EQ_F:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_f64 >= svm->stack[svm->stack_ptr - 1].as_f64);
      svm->stack_ptr--;
      break;
    case SVM_INST_LT_F:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_f64 < svm->stack[svm->stack_ptr - 1].as_f64);
      svm->stack_ptr--;
      break;
    case SVM_INST_LT_EQ_F:
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      svm->stack[svm->stack_ptr - 2] = SVM_VALUE_I64(svm->stack[svm->stack_ptr - 2].as_f64 <= svm->stack[svm->stack_ptr - 1].as_f64);
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
    case SVM_INST_CALL:
      if (svm->call_stack_ptr >= SVM_CALL_STACK_SIZE) {
        return SVM_ERR_CALL_STACK_OVERFLOW;
      }
      svm->call_stack[svm->call_stack_ptr++] = svm->ip;
      svm->ip = instruction.operand.as_u64;
      break;
    case SVM_INST_RET:
      if (svm->call_stack_ptr < 1) {
        return SVM_ERR_CALL_STACK_UNDERFLOW;
      }
      svm->ip = svm->call_stack[svm->call_stack_ptr - 1];
      svm->call_stack_ptr--;
      break;
    case SVM_INST_ALLOC: {
      if (svm->stack_ptr >= SVM_STACK_SIZE) {
        return SVM_ERR_STACK_OVERFLOW;
      }
      if (svm->heap_addrs_ptr >= SVM_HEAP_ADDRS_SIZE) {
        return SVM_ERR_ADDR_LIST_FULL;
      }

      // Allocate the address.
      void* addr = malloc(instruction.operand.as_u64);
      memset(addr, 0, instruction.operand.as_u64);
      svm->heap_addrs[svm->heap_addrs_ptr++] = addr;

      // Put the address on the stack.
      svm->stack[svm->stack_ptr] = SVM_VALUE_PTR(addr);
      svm->stack_ptr++;
      break;
    }
    case SVM_INST_FREE: {
      if (svm->stack_ptr < 1) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      // Find the address.
      void* addr = svm->stack[svm->stack_ptr - 1].as_ptr;
      if (addr == NULL) {
        return SVM_ERR_ILLEGAL_ADDR;
      }
      uint64_t addr_idx;
      bool found = find_addr(svm, addr, &addr_idx);
      if (!found) {
        return SVM_ERR_ILLEGAL_ADDR;
      }

      // Pop the addr from the stack and free it.
      svm->stack_ptr--;
      free(addr);

      // Best case scenario is that the free'd addr was at the end.
      if (addr_idx == svm->heap_addrs_ptr - 1) {
        svm->heap_addrs_ptr--;
        break;
      }

      // If it's not, we need to shift all the addrs that are past it down by one.
      uint64_t remaining_addrs = svm->heap_addrs_ptr - 1 - addr_idx;
      memmove(&svm->heap_addrs[addr_idx], &svm->heap_addrs[addr_idx + 1], remaining_addrs * sizeof(addr));
      svm->heap_addrs_ptr--;
      break;
    }
    case SVM_INST_READ: {
      if (svm->stack_ptr < 1) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      void* addr = svm->stack[svm->stack_ptr - 1].as_ptr;
      uint64_t addr_idx;
      bool found = find_addr(svm, addr, &addr_idx);
      if (!found) {
        return SVM_ERR_ILLEGAL_ADDR;
      }
      memcpy(&svm->stack[svm->stack_ptr - 1], addr, sizeof(svm_value_t));
      break;
    }
    case SVM_INST_WRITE: {
      if (svm->stack_ptr < 2) {
        return SVM_ERR_STACK_UNDERFLOW;
      }
      void* addr = svm->stack[svm->stack_ptr - 2].as_ptr;
      memcpy(addr, &svm->stack[svm->stack_ptr - 1], sizeof(svm_value_t));
      svm->stack_ptr -= 2;
      break;
    }
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
  if (svm->heap_addrs_ptr != 0) {
    char* plural_char = svm->heap_addrs_ptr == 1 ? "" : "es";
    fprintf(stderr, "WARNING: %li address%s leaked.\n", svm->heap_addrs_ptr, plural_char);
    svm_print_addr_list(svm);
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

void svm_print_addr_list(svm_t *svm)
{
  printf("Addrs: \n");
  if (svm->heap_addrs_ptr == 0) {
    printf("  [empty]\n");
  } else {
    for (uint64_t i = 0; i < svm->heap_addrs_ptr; i++) {
      printf("  %p\n", svm->heap_addrs[i]);
    }
  }
}

int main (int argc, char *argv[])
{
  for (int i = 0; i < argc; i++) {
    if (strncmp(argv[i], "--help", 6) == 0) {
      usage();
      return 0;
    }
  }

  if (argc < 2) {
    fprintf(stderr, "Error: No input file.\n");
    usage();
    return 1;
  }
  if (argc > 2) {
    fprintf(stderr, "Error: Too many arguments.\n");
    usage();
    return 1;
  }
  svm_t svm;
  svm_init(&svm);

  if (!svm_load_program_from_file(&svm, argv[1])) {
    fprintf(stderr, "Error loading input file '%s'\n", argv[1]);
  }

  svm_err_t result = svm_run(&svm);
  if (result != SVM_ERR_OK) {
    fprintf(stderr, "Error: %s\n", svm_err_to_string(result));
  }
  svm_print_stack(&svm);

  return result;
}
