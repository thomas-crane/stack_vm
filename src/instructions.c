#include "svm/instructions.h"

const char *svm_instruction_type_to_string(svm_instruction_type_t inst_type)
{
  switch (inst_type) {
    case SVM_INST_NOP: return "SVM_INST_NOP";
    case SVM_INST_HALT: return "SVM_INST_HALT";
    case SVM_INST_PUSH: return "SVM_INST_PUSH";
    case SVM_INST_POP: return "SVM_INST_POP";
    case SVM_INST_ADD: return "SVM_INST_ADD";
    case SVM_INST_SUB: return "SVM_INST_SUB";
    case SVM_INST_MULT: return "SVM_INST_MULT";
    case SVM_INST_DIV: return "SVM_INST_DIV";
    case SVM_INST_EQ: return "SVM_INST_EQ";
    case SVM_INST_NOT_EQ: return "SVM_INST_NOT_EQ";
    case SVM_INST_GT: return "SVM_INST_GT";
    case SVM_INST_GT_EQ: return "SVM_INST_GT_EQ";
    case SVM_INST_LT: return "SVM_INST_LT";
    case SVM_INST_LT_EQ: return "SVM_INST_LT_EQ";
    default:
      return "Unknown instruction type.";
  }
}