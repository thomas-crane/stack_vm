#ifndef HDR_SVM_INSTRUCTIONS_H
#define HDR_SVM_INSTRUCTIONS_H

#include <stdint.h>

typedef enum {
  /* Misc. */
  SVM_INST_NOP = 0,
  SVM_INST_HALT,

  /* Stack ops. */
  SVM_INST_PUSH,
  SVM_INST_POP,

  /* Arithmetic. */
  SVM_INST_ADD,
  SVM_INST_SUB,
  SVM_INST_MULT,
  SVM_INST_DIV,
} svm_instruction_type_t;

const char *svm_instruction_type_to_string(svm_instruction_type_t inst_type);

typedef struct {
  svm_instruction_type_t type;
  int64_t operand;
} svm_instruction_t;

#endif // HDR_SVM_INSTRUCTIONS_H
