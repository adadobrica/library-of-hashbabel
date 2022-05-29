// Copyright 2022 Dobrica Nicoleta Adriana
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "/home/student/structs.h"
#include "/home/student/ht_utils.h"
#include "/home/student/book_utils.h"
#include "/home/student/user_utils.h"
#include "/home/student/defs_utils.h"

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

int main(void) {
	hashtable_t *library = ht_create(HMAX_LIBRARY, hash_function_string,
	compare_function_strings);
	hashtable_t *user = ht_create(HMAX_USERS, hash_function_string,
	compare_function_strings);
	book_t new_book;
	while (1) {
		char command[MAX_STRING_SIZE];
		scanf("%s", command);
		if (strcmp(command, "ADD_BOOK") == 0) {
			ADD_BOOK(&library, &new_book);
		} else if (strcmp(command, "GET_BOOK") == 0) {
			GET_BOOK(library);
		} else if (strcmp(command, "RMV_BOOK") == 0) {
			REMOVE_BOOK(&library);
		} else if (strcmp(command, "ADD_DEF") == 0) {
			ADD_DEF(&library);
		} else if (strcmp(command, "GET_DEF") == 0) {
			GET_DEF(&library);
		} else if (strcmp(command, "RMV_DEF") == 0) {
			RMV_DEF(&library);
		} else if (strcmp(command, "ADD_USER") == 0) {
			ADD_USER(&user);
		} else if (strcmp(command, "BORROW") == 0) {
			BORROW_BOOK(&user, &library);
		} else if (strcmp(command, "RETURN") == 0) {
			RETURN_BOOK(&user, &library);
		} else if (strcmp(command, "LOST") == 0) {
			LOST_BOOK(&user, &library);
		} else if (strcmp(command, "EXIT") == 0) {
			print_top_books(library);
			print_top_users(user);
			ht_free(&library, 1);
			ht_free(&user, 2);
			break;
		}
	}
	return 0;
}
