// Copyright 2022 Dobrica Nicoleta Adriana
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structs.h"
#include "ht_utils.h"
#include "book_utils.h"
#include "user_utils.h"

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

void ADD_DEF(hashtable_t **library) {
	char garbage, key[MAX_DEF_LEN], val[MAX_DEF_LEN], book_name[MAX_BOOK_LEN];
	scanf("%c", &garbage);
	get_book_name(book_name);
	scanf("%s %s", key, val);
	int ok = 0;

	if (ht_has_key(*library, book_name) == 0) {
		printf("%s", BOOK_NOT_FOUND);
		ok = 1;
	}
	if (ok == 0) {
		book_t *b = (book_t *)ht_get(*library, book_name);
		if (check_for_resize(b->book)) {
			hashtable_t *defs = b->book;
			RESIZE_HASHTABLE(&defs, 1);
			b->book = defs;
		}
		ht_put(b->book, key, strlen(key) + 1, val, strlen(val) + 1);
	}
}

void GET_DEF(hashtable_t **library) {
	char garbage, key[MAX_DEF_LEN], book_name[MAX_BOOK_LEN];
	int ok = 0;
	scanf("%c", &garbage);
	get_book_name(book_name);
	scanf("%s", key);

	if (ht_has_key(*library, book_name) == 0) {
		printf("%s", BOOK_NOT_FOUND);
		ok = 1;
	}
	if (ok == 0) {
		book_t *b = (book_t *)ht_get(*library, book_name);
		hashtable_t *defs = (hashtable_t *)b->book;
		if (ht_has_key(defs, key) == 0) {
			printf("%s", DEF_NOT_FOUND);
		} else {
			char *val = (char *)ht_get(defs, key);
			printf("%s\n", val);
		}
	}
}

void RMV_DEF(hashtable_t **library) {
	char garbage, key[MAX_DEF_LEN], book_name[MAX_BOOK_LEN];
	scanf("%c", &garbage);
	get_book_name(book_name);
	scanf("%s", key);

	int ok = 0;
	if (ht_has_key(*library, book_name) == 0) {
		printf("%s", BOOK_NOT_FOUND);
		ok = 1;
	}
	if (ok == 0) {
		book_t *b = (book_t *)ht_get(*library, book_name);
		hashtable_t *def = (hashtable_t *)b->book;
		if (ht_has_key(def, key) == 0) {
			printf("%s", DEF_NOT_FOUND);
		} else {
			ht_remove_entry(def, key);
		}
	}
}
