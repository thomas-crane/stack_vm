#ifndef HDR_SVM_LABEL_LIST_H
#define HDR_SVM_LABEL_LIST_H

#include <stdint.h>

typedef struct _label_list {
  struct _label_list* next;
  char *label;
  uint64_t address;
} svm_label_list_t;

svm_label_list_t *svm_label_list_new(const char *label, uint64_t address);
void svm_label_list_free(svm_label_list_t *list);

void svm_label_list_append(svm_label_list_t *list, svm_label_list_t *node);

svm_label_list_t *svm_label_list_find(svm_label_list_t *list, const char *label);

void svm_label_list_print(svm_label_list_t *list);

#endif // HDR_SVM_LABEL_LIST_H
