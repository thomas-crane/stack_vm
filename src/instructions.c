#include "svm/instructions.h"

const char *svm_instruction_type_to_string(svm_instruction_type_t inst_type)
{
  switch (inst_type) {
    case SVM_INST_NOP: return "SVM_INST_NOP";
    case SVM_INST_HALT: return "SVM_INST_HALT";

    case SVM_INST_PUSH: return "SVM_INST_PUSH";
    case SVM_INST_POP: return "SVM_INST_POP";
    case SVM_INST_COPY: return "SVM_INST_COPY";
    case SVM_INST_SWAP: return "SVM_INST_SWAP";

    case SVM_INST_ADD_I: return "SVM_INST_ADD_I";
    case SVM_INST_SUB_I: return "SVM_INST_SUB_I";
    case SVM_INST_MULT_I: return "SVM_INST_MULT_I";
    case SVM_INST_DIV_I: return "SVM_INST_DIV_I";

    case SVM_INST_ADD_U: return "SVM_INST_ADD_U";
    case SVM_INST_SUB_U: return "SVM_INST_SUB_U";
    case SVM_INST_MULT_U: return "SVM_INST_MULT_U";
    case SVM_INST_DIV_U: return "SVM_INST_DIV_U";

    case SVM_INST_ADD_F: return "SVM_INST_ADD_F";
    case SVM_INST_SUB_F: return "SVM_INST_SUB_F";
    case SVM_INST_MULT_F: return "SVM_INST_MULT_F";
    case SVM_INST_DIV_F: return "SVM_INST_DIV_F";

    case SVM_INST_EQ: return "SVM_INST_EQ";
    case SVM_INST_NOT_EQ: return "SVM_INST_NOT_EQ";

    case SVM_INST_GT_I: return "SVM_INST_GT_I";
    case SVM_INST_GT_EQ_I: return "SVM_INST_GT_EQ_I";
    case SVM_INST_LT_I: return "SVM_INST_LT_I";
    case SVM_INST_LT_EQ_I: return "SVM_INST_LT_EQ_I";

    case SVM_INST_GT_U: return "SVM_INST_GT_U";
    case SVM_INST_GT_EQ_U: return "SVM_INST_GT_EQ_U";
    case SVM_INST_LT_U: return "SVM_INST_LT_U";
    case SVM_INST_LT_EQ_U: return "SVM_INST_LT_EQ_U";

    case SVM_INST_GT_F: return "SVM_INST_GT_F";
    case SVM_INST_GT_EQ_F: return "SVM_INST_GT_EQ_F";
    case SVM_INST_LT_F: return "SVM_INST_LT_F";
    case SVM_INST_LT_EQ_F: return "SVM_INST_LT_EQ_F";

    case SVM_INST_JMP: return "SVM_INST_JMP";
    case SVM_INST_JNZ: return "SVM_INST_JNZ";

    case SVM_INST_CALL: return "SVM_INST_CALL";
    case SVM_INST_RET: return "SVM_INST_RET";

    case SVM_INST_ALLOC: return "SVM_INST_ALLOC";
    case SVM_INST_FREE: return "SVM_INST_FREE";
    case SVM_INST_READ: return "SVM_INST_READ";
    case SVM_INST_WRITE: return "SVM_INST_WRITE";
    default:
      return "Unknown instruction type.";
  }
}

bool svm_instruction_type_needs_operand(svm_instruction_type_t inst_type)
{
  // This could be much shorter, but we have -Wswitch-enum on and we will get a compiler error if we add a new
  // instruction and forget to add it here.
  switch (inst_type) {
    case SVM_INST_NOP: return false;
    case SVM_INST_HALT: return false;

    case SVM_INST_PUSH: return true;
    case SVM_INST_POP: return false;
    case SVM_INST_COPY: return true;
    case SVM_INST_SWAP: return true;

    case SVM_INST_ADD_I: return false;
    case SVM_INST_SUB_I: return false;
    case SVM_INST_MULT_I: return false;
    case SVM_INST_DIV_I: return false;

    case SVM_INST_ADD_U: return false;
    case SVM_INST_SUB_U: return false;
    case SVM_INST_MULT_U: return false;
    case SVM_INST_DIV_U: return false;

    case SVM_INST_ADD_F: return false;
    case SVM_INST_SUB_F: return false;
    case SVM_INST_MULT_F: return false;
    case SVM_INST_DIV_F: return false;

    case SVM_INST_EQ: return false;
    case SVM_INST_NOT_EQ: return false;

    case SVM_INST_GT_I: return false;
    case SVM_INST_GT_EQ_I: return false;
    case SVM_INST_LT_I: return false;
    case SVM_INST_LT_EQ_I: return false;

    case SVM_INST_GT_U: return false;
    case SVM_INST_GT_EQ_U: return false;
    case SVM_INST_LT_U: return false;
    case SVM_INST_LT_EQ_U: return false;

    case SVM_INST_GT_F: return false;
    case SVM_INST_GT_EQ_F: return false;
    case SVM_INST_LT_F: return false;
    case SVM_INST_LT_EQ_F: return false;

    case SVM_INST_JMP: return true;
    case SVM_INST_JNZ: return true;

    case SVM_INST_CALL: return true;
    case SVM_INST_RET: return false;

    case SVM_INST_ALLOC: return true;
    case SVM_INST_FREE: return false;
    case SVM_INST_READ: return false;
    case SVM_INST_WRITE: return false;
    default:
      return false;
  }
}
