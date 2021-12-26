#include "svm/label_list.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

svm_label_list_t *svm_label_list_new(const char *label, uint64_t address)
{
  svm_label_list_t *label_list = malloc(sizeof(*label_list));
  label_list->next = NULL;
  label_list->label = strdup(label);
  label_list->address = address;

  return label_list;
}

void svm_label_list_free(svm_label_list_t *label_list)
{
  free(label_list->label);
  if (label_list->next != NULL) {
    svm_label_list_free(label_list->next);
  }

  free(label_list);
}


void svm_label_list_append(svm_label_list_t *list, svm_label_list_t *node)
{
  svm_label_list_t *current = list;
  while (current->next != NULL) {
    current = current->next;
  }

  current->next = node;
}

svm_label_list_t *svm_label_list_find(svm_label_list_t *list, const char *label)
{
  svm_label_list_t *current = list;
  do {
    if (strncmp(label, current->label, strlen(label)) == 0) {
      return current;
    }
  } while (current != NULL);

  return NULL;
}

void svm_label_list_print(svm_label_list_t *list)
{
  printf("Labels:\n");
  if (list == NULL) {
    printf("  [emtpy]\n");
  } else {
    svm_label_list_t *current = list;
    do {
      printf("  '%s': %lu\n", current->label, current->address);
      current = current->next;
    } while (current);
  }
}
