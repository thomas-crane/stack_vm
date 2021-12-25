#include "svm/err.h"

const char *svm_err_to_string(svm_err_t err)
{
  switch (err) {
    case SVM_ERR_OK: return "SVM_ERR_OK";

    case SVM_ERR_STACK_OVERFLOW: return "SVM_ERR_STACK_OVERFLOW";
    case SVM_ERR_STACK_UNDERFLOW: return "SVM_ERR_STACK_UNDERFLOW";

    case SVM_ERR_ILLEGAL_INSTRUCTION: return "SVM_ERR_ILLEGAL_INSTRUCTION";
    case SVM_ERR_IP_OVERFLOW: return "SVM_ERR_IP_OVERFLOW";

    case SVM_ERR_CALL_STACK_OVERFLOW: return "SVM_ERR_CALL_STACK_OVERFLOW";
    case SVM_ERR_CALL_STACK_UNDERFLOW: return "SVM_ERR_CALL_STACK_UNDERFLOW";

    case SVM_ERR_ADDR_LIST_FULL: return "SVM_ERR_ADDR_LIST_FULL";
    case SVM_ERR_ILLEGAL_FREE: return "SVM_ERR_ILLEGAL_FREE";
    default:
      return "Unknown error";
      break;
  }
}
