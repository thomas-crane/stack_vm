#ifndef HDR_SVM_SVM_H
#define HDR_SVM_SVM_H

#include "svm/err.h"
#include "svm/instructions.h"

#include <stdint.h>
#include <stdbool.h>

#define SVM_STACK_SIZE 1024
#define SVM_MAX_PROGRAM_SIZE 1024

typedef struct {
  bool halted;

  int64_t stack[SVM_STACK_SIZE]; 
  uint64_t stack_ptr;

  svm_instruction_t program[SVM_MAX_PROGRAM_SIZE];
  uint64_t program_size;
  uint64_t ip;
} svm_t;

void svm_init(svm_t *svm);
bool svm_load_program_from_array(svm_t *svm, svm_instruction_t *instructions, uint32_t program_size);

svm_err_t svm_exec_instruction(svm_t *svm);
svm_err_t svm_run(svm_t *svm);

void svm_print_stack(svm_t *svm);

#endif // HDR_SVM_SVM_H