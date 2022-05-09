// Copyright 2022 Dobrica Nicoleta Adriana
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structs.h"
#include "ht_utils.h"

#define HMAX_LIBRARY 10
#define HMAX_USERS 10
#define HMAX_BOOKS 10
#define MAX_BOOK_LEN 40
#define MAX_STRING_SIZE 256
#define ARR_MAX 1000
#define MAX_DEF_LEN 20
#define EPS 1e-3

#define USER_REGISTERED "User is already registered.\n"
#define USER_NOT_REGISTERED "You are not registered yet.\n"
#define BANNED_USER "You are banned from this library.\n"
#define BOOK_NOT_FOUND "The book is not in the library.\n"
#define BORROWED_BOOK "The book is borrowed.\n"
#define USER_BORROWED "You have already borrowed a book.\n"
#define DEF_NOT_FOUND "The definition is not in the book.\n"

void print_book(hashtable_t *library, char book_name[MAX_BOOK_LEN]) {
	if (ht_has_key(library, book_name) == 0) {
		printf("%s", BOOK_NOT_FOUND);
	} else {
		 book_t *book = (book_t *)ht_get(library, book_name);
		 printf("Name:");
		 for (int i = 1, len = strlen(book->name); i < len - 2; i++) {
			 printf("%c", book->name[i]);
		 }
		 printf(" Rating:%0.3f Purchases:%d\n", book->rating, book->purchases);
	}
}

int cmp(const void *a, const void *b) {
	book_t x = *(book_t *)a;
	book_t y = *(book_t *)b;
	if (fabs(x.rating - y.rating) < EPS && x.purchases == y.purchases) {
		return strcmp(x.name, y.name);
	}
	if (fabs(x.rating - y.rating) < EPS) {
		return y.purchases - x.purchases;
	}
	return ceil(y.rating - x.rating);
}

void print_top_books(hashtable_t *library) {
	int size = 0;
	book_t *books = malloc(library->size * sizeof(book_t));

	for (unsigned  int i = 0; i < library->hmax; i++) {
		node_t *current = library->buckets[i]->head;
		while (current) {
			info *data = (info *)current->data;
			book_t *book_data = (book_t *)data->value;
			books[size++] = *book_data;
			current = current->next;
		}
	}
	qsort(books, library->size, sizeof(book_t), cmp);
	printf("Books ranking:\n");
	for (int i = 0; i < size; i++) {
		printf("%d. Name:", i + 1);
		for (int j = 1, len = strlen(books[i].name); j < len - 2; j++) {
			printf("%c", books[i].name[j]);
		}
		printf(" Rating:%0.3f Purchases:%d\n", books[i].rating,
				books[i].purchases);
	}
	free(books);
}
void get_book_name(char book_name[MAX_STRING_SIZE]) {
	int valid = 0, len = 0;
	char buff[MAX_STRING_SIZE] = {0}, *ch = " ";
	while (valid == 0) {
		scanf("%s", buff);
		if (buff[strlen(buff) - 1] == '\"') {
			valid = 1;
		}
		memmove(book_name + len, buff, strlen(buff) + 1);
		len += strlen(buff);
		memmove(book_name + len, ch, strlen(ch) + 1);
		len++;
	}
}

void ADD_BOOK(hashtable_t **library, book_t *new_book) {
	int num = 0, def_number;
	char book_name[MAX_BOOK_LEN], garbage;
	new_book->purchases = 0;
	new_book->rating = 0;
	new_book->borrowed = 0;
	new_book->book = ht_create(HMAX_BOOKS, hash_function_string,
	compare_function_strings);
	scanf("%c", &garbage);
	get_book_name(book_name);
	scanf("%d", &def_number);

	if (ht_has_key(*library, book_name)) {
		book_t *b = (book_t *)ht_get(*library, book_name);
		hashtable_t *defs = (hashtable_t *)b->book;
		ht_free(&defs, 2);
		ht_remove_entry(*library, book_name);
	}


	memmove(new_book->name, book_name, strlen(book_name) + 1);
	while (num != def_number) {
		char key[MAX_BOOK_LEN], val[MAX_DEF_LEN];
		scanf("%s %s", key, val);

		int ok = check_for_resize(new_book->book);
		if (ok == 1) {
			hashtable_t *defs = new_book->book;
			RESIZE_HASHTABLE(&defs, 1);
			new_book->book = defs;
		}
		ht_put(new_book->book, key, strlen(key) + 1, val, strlen(val) + 1);
		num++;
	}
	int ok = check_for_resize(*library);
	if (ok == 1)
		RESIZE_HASHTABLE(library, 2);
	ht_put(*library, new_book->name, strlen(new_book->name) + 1, new_book,
	sizeof(book_t));
}

void GET_BOOK(hashtable_t *library) {
	char book_name[MAX_BOOK_LEN], garbage;
	scanf("%c", &garbage);
	get_book_name(book_name);
	if (ht_has_key(library, book_name) == 0) {
		printf("%s", BOOK_NOT_FOUND);
	} else {
		print_book(library, book_name);
	}
}

void REMOVE_BOOK(hashtable_t **library) {
	char book_name[MAX_BOOK_LEN], garbage;
	scanf("%c", &garbage);
	get_book_name(book_name);
	if (ht_has_key(*library, book_name) == 0) {
		printf("%s", BOOK_NOT_FOUND);
	} else {
		book_t *b = (book_t *)ht_get(*library, book_name);
		hashtable_t *defs = (hashtable_t *)b->book;
		ht_free(&defs, 2);
		ht_remove_entry(*library, book_name);
	}
}
