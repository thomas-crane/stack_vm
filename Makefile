BIN_DIR := bin

SVM_LIB_SRC := src/err.c src/instructions.c src/label_list.c
SVM_LIB_HDRS := include/svm/err.h include/svm/instructions.h include/svm/value.h include/svm/label_list.h

CPPFLAGS := -Iinclude
CFLAGS := -Werror -Wall -Wextra -Wpedantic -Wswitch-enum

.PHONY: all clean

all: $(BIN_DIR)/svm $(BIN_DIR)/svmasm

release: CFLAGS += -O3
release: all

$(BIN_DIR)/%: src/%.c $(SVM_LIB_SRC) $(SVM_LIB_HDRS) | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(SVM_LIB_SRC)

$(BIN_DIR):
	mkdir -p $@

clean:
	rm -rv $(BIN_DIR)
