
build: structs.h ht_utils.h book_utils.h defs_utils.h user_utils.h
	gcc library.c -o main ht_utils.c book_utils.c defs_utils.c user_utils.c -lm -Wall -Wextra -std=c99
run:
	./main

clean:
	rm -f main

.PHONY: pack clean
