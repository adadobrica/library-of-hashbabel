// Copyright 2022 Dobrica Nicoleta Adriana
#ifndef BOOK_UTILS_H_
#define BOOK_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structs.h"
#include "ht_utils.h"

#define MAX_BOOK_LEN 40
#define MAX_STRING_SIZE 256

void print_book(hashtable_t *library, char book_name[MAX_BOOK_LEN]);
int cmp(const void *a, const void *b);
void print_top_books(hashtable_t *library);
void get_book_name(char book_name[MAX_STRING_SIZE]);
void ADD_BOOK(hashtable_t **library, book_t *new_book);
void GET_BOOK(hashtable_t *library);
void REMOVE_BOOK(hashtable_t **library);

#endif  //  BOOK_UTILS_H_
