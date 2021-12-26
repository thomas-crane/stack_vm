#include "svm/svm.h"
#include "svm/label_list.h"
#include "svm/instructions.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

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

static bool parse_f64(const char *token, double *f64)
{
  char *endptr;

  errno = 0;
  double value = strtod(token, &endptr);

  if (errno != 0) {
    return false;
  }

  if (endptr == token) {
    return false;
  }

  *f64 = value;
  return true;
}

static bool parse_i64(const char *token, int64_t *i64)
{
  char *endptr;

  errno = 0;
  int64_t value = strtol(token, &endptr, 10);

  if (errno != 0) {
    return false;
  }

  if (endptr == token) {
    return false;
  }

  *i64 = value;
  return true;
}

static bool parse_u64(const char *token, uint64_t *u64)
{
  char *endptr;

  errno = 0;
  uint64_t value = strtoul(token, &endptr, 10);

  if (errno != 0) {
    return false;
  }

  if (endptr == token) {
    return false;
  }

  *u64 = value;
  return true;
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

  int exitcode = 0;
  char line[256];
  uint64_t lineno = 0;
  while (fgets(line, sizeof(line), fd)) {
    lineno++;
    char *token = strtok(line, " \n");
    if (token == NULL) {
      exitcode = 1;
      goto cleanup;
    }

    // Ignore labels.
    if (token[strlen(token) - 1] == ':') {
      continue;
    }

    // Check for instruction.
    svm_instruction_type_t type;
    if (!svm_instruction_type_from_string(token, &type)) {
      fprintf(stderr, "Error: %s:%lu\n", input_file, lineno);
      fprintf(stderr, "  Expected an instruction, got '%s'\n", token);
      exitcode = 1;
      goto cleanup;
    }

    if (svm_instruction_type_needs_operand(type)) {
      token = strtok(NULL, " \n");
      if (token == NULL) {
        exitcode = 1;
        goto cleanup;
      }

      svm_value_t value = {0};
      // Check if we should be looking up a label.
      if (svm_instruction_type_needs_label_operand(type)) {
        // parse label.
        svm_label_list_t *label = svm_label_list_find(labels, token);
        if (label == NULL) {
          fprintf(stderr, "Error: %s:%lu\n", input_file, lineno);
          fprintf(stderr, "  Unknown label '%s'\n", token);
          exitcode = 1;
          goto cleanup;
        }

        value.as_u64 = label->address;
        continue;
      }
      
      switch (token[strlen(token) - 1]) {
        case 'f':
          if (!parse_f64(token, &value.as_f64)) {
            fprintf(stderr, "Error: %s:%lu\n", input_file, lineno);
            fprintf(stderr, "  Cannot parse f64 '%s'\n", token);
            exitcode = 1;
            goto cleanup;
          }
          break;
        case 'u':
          if (!parse_u64(token, &value.as_u64)) {
            fprintf(stderr, "Error: %s:%lu\n", input_file, lineno);
            fprintf(stderr, "  Cannot parse u64 '%s'\n", token);
            exitcode = 1;
            goto cleanup;
          }
          break;
        default:
          if (!parse_i64(token, &value.as_i64)) {
            fprintf(stderr, "Error: %s:%lu\n", input_file, lineno);
            fprintf(stderr, "  Cannot parse i64 '%s'\n", token);
            exitcode = 1;
            goto cleanup;
          }
          break;
      }
    }
  }

cleanup:
  if (labels != NULL) {
    svm_label_list_free(labels);
  }
  fclose(fd);

  return exitcode;
}
