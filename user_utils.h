// Copyright 2022 Dobrica Nicoleta Adriana
#ifndef USER_UTILS_H_
#define USER_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structs.h"
#include "ht_utils.h"
#include "book_utils.h"

void print_top_users(hashtable_t *users);
void ADD_USER(hashtable_t **user);
void BORROW_BOOK(hashtable_t **user, hashtable_t **library);
void RETURN_BOOK(hashtable_t **user, hashtable_t **library);
void LOST_BOOK(hashtable_t **user, hashtable_t **library);

#endif  //  USER_UTILS_H_
