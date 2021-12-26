#include "svm/svmasm.h"

#include "svm/svm.h"
#include "svm/label_list.h"
#include "svm/instructions.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

static void usage()
{
  fprintf(stderr, "Usage: svmasm [FILE]\n");
  fprintf(stderr, "Compile the given svm assembly file into an svm binary.\n");
}

static svm_label_list_t *translate_labels(FILE *fd)
{
  char line[256];
  svm_label_list_t* list = NULL;
  uint32_t lineno = 0;
  uint64_t num_labels = 0;

  while (fgets(line, sizeof(line), fd)) {
    char *token = strtok(line, " \n");
    if (token == NULL) {
      continue;
    }
    int token_len = strlen(token);
    if (token[token_len - 1] == ':') {
      // Cut off the ':'.
      token[token_len - 1] = '\0';
      if (list == NULL) {
        list = svm_label_list_new(token, lineno - num_labels);
      } else {
        svm_label_list_t *node = svm_label_list_new(token, lineno - num_labels);
        svm_label_list_append(list, node);
      }
      num_labels++;
    }
    lineno++;
  }
  rewind(fd);

  return list;
}

int main (int argc, char *argv[])
{
  for (int i = 0; i < argc; i++) {
    if (strncmp(argv[i], "--help", 6) == 0) {
      usage();
      return 0;
    }
  }

  if (argc < 2) {
    fprintf(stderr, "Error: No input file.\n");
    usage();
    return 1;
  }
  if (argc > 2) {
    fprintf(stderr, "Error: Too many arguments.\n");
    usage();
    return 1;
  }

  const char *input_file = argv[1];
  FILE *fd = fopen(input_file, "r");
  if (fd == NULL) {
    fprintf(stderr, "Error: Failed to open input file '%s'\n", input_file);
    return 1;
  }

  svm_label_list_t *labels = translate_labels(fd);
  if (labels != NULL) {
    svm_label_list_print(labels);
  }

  char line[256];
  uint32_t lineno = 0;
  while (fgets(line, sizeof(line), fd)) {
    char *token = strtok(line, " \n");
    while (token) {
      svm_instruction_type_t type;
      if (svm_instruction_type_from_string(token, &type)) {
        printf("Got instruction type: %s\n", svm_instruction_type_to_string(type));
      }

      token = strtok(NULL, " \n");
    }

    lineno++;
  }

  if (labels != NULL) {
    svm_label_list_free(labels);
  }
  fclose(fd);

  return 0;
}
