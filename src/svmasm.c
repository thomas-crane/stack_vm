#include "svm/svm.h"
#include "svm/label_list.h"
#include "svm/instructions.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <libgen.h>

static void usage()
{
  fprintf(stderr, "Usage: svmasm [FILE]\n");
  fprintf(stderr, "Compile the given svm assembly file into an svm binary.\n");
}

static svm_label_list_t *translate_labels(FILE *in_fd)
{
  char line[256];
  svm_label_list_t* list = NULL;
  uint64_t instruction_cnt = 0;
  uint64_t num_labels = 0;

  while (fgets(line, sizeof(line), in_fd)) {
    // Skip empty lines.
    if (strlen(line) == 1) {
      continue;
    }

    char *token = strtok(line, " \n");
    if (token == NULL) {
      continue;
    }
    int token_len = strlen(token);
    if (token[token_len - 1] == ':') {
      // Cut off the ':'.
      token[token_len - 1] = '\0';
      if (list == NULL) {
        list = svm_label_list_new(token, instruction_cnt - num_labels);
      } else {
        svm_label_list_t *node = svm_label_list_new(token, instruction_cnt - num_labels);
        svm_label_list_append(list, node);
      }
      num_labels++;
    }
    // Check for comment. 
    if (token[0] == ';') {
      continue;
    }
    instruction_cnt++;
  }
  rewind(in_fd);

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

  char *input_file = argv[1];
  char output_file[256] = {0};

  // Copy the input file name so we can strip the extension off.
  char *in_name = strdup(input_file);
  int in_len = strlen(in_name);
  for (int64_t i = in_len - 1; i >= 0; i--) {
    if (in_name[i] == '.') {
      memset(&in_name[i], 0, in_len - i - 1);
      break;
    }
  }
  // Add the new extension for the output file name.
  sprintf(output_file, "%s.svmo", in_name);
  free(in_name);

  FILE *in_fd = fopen(input_file, "r");
  if (in_fd == NULL) {
    fprintf(stderr, "Error: Failed to open input file '%s'\n", input_file);
    return 1;
  }

  FILE *out_fd = fopen(output_file, "w");
  if (out_fd == NULL) {
    fprintf(stderr, "Error: Failed to open output file '%s'\n", output_file);
    fclose(in_fd);
    return 1;
  }

  svm_label_list_t *labels = translate_labels(in_fd);

  int exitcode = 0;
  char line[256];
  uint64_t lineno = 0;
  while (fgets(line, sizeof(line), in_fd)) {
    lineno++;
    // Skip empty lines.
    if (strlen(line) == 1) {
      continue;
    }

    char *token = strtok(line, " \n");
    if (token == NULL) {
      fprintf(stderr, "Error: %s:%lu\n", input_file, lineno);
      fprintf(stderr, "  Expected a token, got '%s'\n", token);
      exitcode = 1;
      goto cleanup;
    }

    // Skip comments.
    if (token[0] == ';') {
      continue;
    }

    // Ignore labels.
    if (token[strlen(token) - 1] == ':') {
      continue;
    }

    // Read an instruction.
    svm_instruction_type_t type;
    if (!svm_instruction_type_from_string(token, &type)) {
      fprintf(stderr, "Error: %s:%lu\n", input_file, lineno);
      fprintf(stderr, "  Expected an instruction, got '%s'\n", token);
      exitcode = 1;
      goto cleanup;
    }

    // Write the instruction to the output.
    uint64_t type_value = (uint64_t)type;
    if (fwrite(&type_value, sizeof(type_value), 1, out_fd) == 0) {
      fprintf(stderr, "Error: %s:%lu\n", input_file, lineno);
      fprintf(stderr, "  Cannot write type value to output file.");
      exitcode = 1;
      goto cleanup;
    }

    // Go to the next line if we don't need an operand.
    if (!svm_instruction_type_needs_operand(type)) {
      continue;
    }

    token = strtok(NULL, " \n");
    if (token == NULL) {
      fprintf(stderr, "Error: %s:%lu\n", input_file, lineno);
      fprintf(stderr, "  Expected an operand after '%s' instruction.\n", svm_instruction_type_to_string(type));
      exitcode = 1;
      goto cleanup;
    }

    svm_value_t value = {0};

    // Check if we should be looking up a label.
    if (svm_instruction_type_needs_label_operand(type)) {
      // Parse label.
      svm_label_list_t *label = svm_label_list_find(labels, token);
      if (label == NULL) {
        fprintf(stderr, "Error: %s:%lu\n", input_file, lineno);
        fprintf(stderr, "  Unknown label '%s'\n", token);
        exitcode = 1;
        goto cleanup;
      }

      value.as_u64 = label->address;
    } else {
      // If its not a label then use the letter after the number to work out what kind of number it is.
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

    // Write the value to the output.
    if (fwrite(&value.as_u64, sizeof(value), 1, out_fd) == 0) {
      fprintf(stderr, "Error: %s:%lu\n", input_file, lineno);
      fprintf(stderr, "  Cannot write operand value to output file.");
      exitcode = 1;
      goto cleanup;
    }
  }

cleanup:
  svm_label_list_free(labels);
  fclose(in_fd);
  fclose(out_fd);

  return exitcode;
}
