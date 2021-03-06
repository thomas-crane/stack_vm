#ifndef HDR_SVM_INSTRUCTIONS_H
#define HDR_SVM_INSTRUCTIONS_H

#include "svm/value.h"

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  /* Misc. */
  SVM_INST_NOP = 0,
  SVM_INST_HALT,

  /* Stack ops. */
  SVM_INST_PUSH,
  SVM_INST_POP,
  SVM_INST_COPY,
  SVM_INST_SWAP,

  /* Arithmetic. */
  // i64.
  SVM_INST_ADD_I,
  SVM_INST_SUB_I,
  SVM_INST_MULT_I,
  SVM_INST_DIV_I,
  // u64.
  SVM_INST_ADD_U,
  SVM_INST_SUB_U,
  SVM_INST_MULT_U,
  SVM_INST_DIV_U,
  // f64.
  SVM_INST_ADD_F,
  SVM_INST_SUB_F,
  SVM_INST_MULT_F,
  SVM_INST_DIV_F,

  /* Comparison */
  // Don't need an eq/not eq for each type. Just interpret both values as the same type.
  SVM_INST_EQ,
  SVM_INST_NOT_EQ,
  // i64.
  SVM_INST_GT_I,
  SVM_INST_GT_EQ_I,
  SVM_INST_LT_I,
  SVM_INST_LT_EQ_I,
  // u64.
  SVM_INST_GT_U,
  SVM_INST_GT_EQ_U,
  SVM_INST_LT_U,
  SVM_INST_LT_EQ_U,
  // f64.
  SVM_INST_GT_F,
  SVM_INST_GT_EQ_F,
  SVM_INST_LT_F,
  SVM_INST_LT_EQ_F,

  /* Jumps */
  SVM_INST_JMP,
  SVM_INST_JNZ,

  /* Function stuff */
  SVM_INST_CALL,
  SVM_INST_RET,

  /* Heap stuff */
  SVM_INST_ALLOC,
  SVM_INST_FREE,
  SVM_INST_READ,
  SVM_INST_WRITE,
} svm_instruction_type_t;

const char *svm_instruction_type_to_string(svm_instruction_type_t inst_type);

bool svm_instruction_type_needs_operand(svm_instruction_type_t inst_type);
bool svm_instruction_type_needs_label_operand(svm_instruction_type_t inst_type);

bool svm_instruction_type_from_string(const char *str, svm_instruction_type_t *inst_type);

typedef struct {
  svm_instruction_type_t type;
  svm_value_t operand;
} svm_instruction_t;

#endif // HDR_SVM_INSTRUCTIONS_H
