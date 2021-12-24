#ifndef HDR_SVM_ERR_H
#define HDR_SVM_ERR_H

typedef enum {
  SVM_ERR_OK,
  SVM_ERR_STACK_OVERFLOW,
  SVM_ERR_STACK_UNDERFLOW,
  SVM_ERR_ILLEGAL_INSTRUCTION,
  SVM_ERR_IP_OVERFLOW,
  SVM_ERR_CALL_STACK_OVERFLOW,
  SVM_ERR_CALL_STACK_UNDERFLOW,
} svm_err_t;

const char *svm_err_to_string(svm_err_t err);

#endif // HDR_SVM_ERR_H
