#include "svm/instructions.h"

#include <stdbool.h>
#include <string.h>

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
  // Using if instead of switch because we have -Wswitch-enum on.
  if (inst_type == SVM_INST_PUSH) return true;
  if (inst_type == SVM_INST_COPY) return true;
  if (inst_type == SVM_INST_SWAP) return true;
  if (inst_type == SVM_INST_JMP) return true;
  if (inst_type == SVM_INST_JNZ) return true;
  if (inst_type == SVM_INST_CALL) return true;
  if (inst_type == SVM_INST_ALLOC) return true;

  return false;
}

bool svm_instruction_type_needs_label_operand(svm_instruction_type_t inst_type)
{
  // Using if instead of switch because we have -Wswitch-enum on.
  if (inst_type == SVM_INST_JMP) return true;
  if (inst_type == SVM_INST_JNZ) return true;
  if (inst_type == SVM_INST_CALL) return true;

  return false;
}

bool svm_instruction_type_from_string(const char *str, svm_instruction_type_t *inst_type)
{
  if (strncmp(str, "nop", 3) == 0) { *inst_type = SVM_INST_NOP; return true; }
  if (strncmp(str, "halt", 4) == 0) { *inst_type = SVM_INST_HALT; return true; }

  if (strncmp(str, "push", 4) == 0) { *inst_type = SVM_INST_PUSH; return true; }
  if (strncmp(str, "pop", 3) == 0) { *inst_type = SVM_INST_POP; return true; }
  if (strncmp(str, "copy", 4) == 0) { *inst_type = SVM_INST_COPY; return true; }
  if (strncmp(str, "swap", 4) == 0) { *inst_type = SVM_INST_SWAP; return true; }

  if (strncmp(str, "addi", 4) == 0) { *inst_type = SVM_INST_ADD_I; return true; }
  if (strncmp(str, "subi", 4) == 0) { *inst_type = SVM_INST_SUB_I; return true; }
  if (strncmp(str, "multi", 5) == 0) { *inst_type = SVM_INST_MULT_I; return true; }
  if (strncmp(str, "divi", 4) == 0) { *inst_type = SVM_INST_DIV_I; return true; }

  if (strncmp(str, "addu", 4) == 0) { *inst_type = SVM_INST_ADD_U; return true; }
  if (strncmp(str, "subu", 4) == 0) { *inst_type = SVM_INST_SUB_U; return true; }
  if (strncmp(str, "multu", 5) == 0) { *inst_type = SVM_INST_MULT_U; return true; }
  if (strncmp(str, "divu", 4) == 0) { *inst_type = SVM_INST_DIV_U; return true; }

  if (strncmp(str, "addf", 4) == 0) { *inst_type = SVM_INST_ADD_F; return true; }
  if (strncmp(str, "subf", 4) == 0) { *inst_type = SVM_INST_SUB_F; return true; }
  if (strncmp(str, "multf", 5) == 0) { *inst_type = SVM_INST_MULT_F; return true; }
  if (strncmp(str, "divf", 4) == 0) { *inst_type = SVM_INST_DIV_F; return true; }

  if (strncmp(str, "eq", 2) == 0) { *inst_type = SVM_INST_EQ; return true; }
  if (strncmp(str, "neq", 3) == 0) { *inst_type = SVM_INST_NOT_EQ; return true; }

  if (strncmp(str, "gti", 3) == 0) { *inst_type = SVM_INST_GT_I; return true; }
  if (strncmp(str, "gtei", 4) == 0) { *inst_type = SVM_INST_GT_EQ_I; return true; }
  if (strncmp(str, "lti", 3) == 0) { *inst_type = SVM_INST_LT_I; return true; }
  if (strncmp(str, "ltei", 4) == 0) { *inst_type = SVM_INST_LT_EQ_I; return true; }

  if (strncmp(str, "gtu", 3) == 0) { *inst_type = SVM_INST_GT_U; return true; }
  if (strncmp(str, "gteu", 4) == 0) { *inst_type = SVM_INST_GT_EQ_U; return true; }
  if (strncmp(str, "ltu", 3) == 0) { *inst_type = SVM_INST_LT_U; return true; }
  if (strncmp(str, "lteu", 4) == 0) { *inst_type = SVM_INST_LT_EQ_U; return true; }

  if (strncmp(str, "gtf", 3) == 0) { *inst_type = SVM_INST_GT_F; return true; }
  if (strncmp(str, "gtef", 4) == 0) { *inst_type = SVM_INST_GT_EQ_F; return true; }
  if (strncmp(str, "ltf", 3) == 0) { *inst_type = SVM_INST_LT_F; return true; }
  if (strncmp(str, "ltef", 4) == 0) { *inst_type = SVM_INST_LT_EQ_F; return true; }

  if (strncmp(str, "jmp", 3) == 0) { *inst_type = SVM_INST_JMP; return true; }
  if (strncmp(str, "jnz", 3) == 0) { *inst_type = SVM_INST_JNZ; return true; }

  if (strncmp(str, "call", 4) == 0) { *inst_type = SVM_INST_CALL; return true; }
  if (strncmp(str, "ret", 3) == 0) { *inst_type = SVM_INST_RET; return true; }

  if (strncmp(str, "alloc", 5) == 0) { *inst_type = SVM_INST_ALLOC; return true; }
  if (strncmp(str, "free", 4) == 0) { *inst_type = SVM_INST_FREE; return true; }
  if (strncmp(str, "read", 4) == 0) { *inst_type = SVM_INST_READ; return true; }
  if (strncmp(str, "write", 5) == 0) { *inst_type = SVM_INST_WRITE; return true; }

  return false;
}
