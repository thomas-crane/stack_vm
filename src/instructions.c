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
    default:
      return "Unknown instruction type.";
  }
}
