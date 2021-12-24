#ifndef HDR_SVM_VALUE_H
#define HDR_SVM_VALUE_H

#include <stdint.h>

typedef union {
  int64_t as_i64;
  uint64_t as_u64;
  double as_f64;
  void* as_ptr;
} svm_value_t;

#define SVM_VALUE_I64(value) ((svm_value_t){.as_i64 = (value)})
#define SVM_VALUE_U64(value) ((svm_value_t){.as_u64 = (value)})
#define SVM_VALUE_F64(value) ((svm_value_t){.as_f64 = (value)})
#define SVM_VALUE_PTR(value) ((svm_value_t){.as_ptr = (value)})

#endif // HDR_SVM_VALUE_H
