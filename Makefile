CFLAGS := -Werror -Wall -Wextra -Wpedantic

svm: src/svm.c
	$(CC) -o $@ $< $(CFLAGS)
