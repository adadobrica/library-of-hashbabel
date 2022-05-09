// Copyright 2022 Dobrica Nicoleta Adriana
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structs.h"
#include "ht_utils.h"
#include "book_utils.h"

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

void print_top_users(hashtable_t *users) {
	char top_user_name[ARR_MAX][MAX_BOOK_LEN];
	int top_user_points[ARR_MAX];
	int n_size = 0, p_size = 0;

	for (unsigned int i = 0; i < users->hmax; i++) {
		node_t *current = users->buckets[i]->head;
		while (current) {
			info *data = (info *)current->data;
			user_t *user_data = (user_t *)data->value;

			memmove(top_user_name[n_size++], user_data->user_name,
					strlen(user_data->user_name) + 1);
			top_user_points[p_size++] = user_data->points;
			current = current->next;
		}
	}

	for (int i = 0; i < p_size; i++) {
		for (int j = i + 1; j < p_size; j++) {
			if (top_user_points[i] < top_user_points[j]) {
				int aux_i = top_user_points[i];
				top_user_points[i] = top_user_points[j];
				top_user_points[j] = aux_i;

				char aux_c[MAX_DEF_LEN];
				memmove(aux_c, top_user_name[i],
						strlen(top_user_name[i]) + 1);
				memmove(top_user_name[i], top_user_name[j],
						strlen(top_user_name[j]) + 1);
				memmove(top_user_name[j], aux_c, strlen(aux_c) + 1);
			} else if (top_user_points[i] == top_user_points[j]) {
				if (strcmp(top_user_name[i], top_user_name[j]) > 0) {
					char aux_c[MAX_DEF_LEN];
					memmove(aux_c, top_user_name[i],
							strlen(top_user_name[i]) + 1);
					memmove(top_user_name[i], top_user_name[j],
							strlen(top_user_name[j]) + 1);
					memmove(top_user_name[j], aux_c, strlen(aux_c) + 1);
				}
			}
		}
	}

	printf("Users ranking:\n");
	for (int i = 0; i < p_size; i++) {
		if (top_user_points[i] < 0) {
			continue;
		}
		printf("%d. Name:%s Points:%d\n", i + 1, top_user_name[i],
				top_user_points[i]);
	}
}


void ADD_USER(hashtable_t **user) {
	user_t new_user;
	scanf("%s", new_user.user_name);
	if (ht_has_key(*user, new_user.user_name) == 1) {
		printf("%s", USER_REGISTERED);
	} else {
		new_user.banned = 0;
		new_user.borrow = 0;
		new_user.points = 100;
		int ok = check_for_resize(*user);
		if (ok == 1)
			RESIZE_HASHTABLE(user, 3);
		ht_put(*user, new_user.user_name, strlen(new_user.user_name) + 1,
		&new_user, sizeof(user_t));
	}
}

void BORROW_BOOK(hashtable_t **user, hashtable_t **library) {
	char garbage, random_value[2] = "X", user_name[MAX_DEF_LEN],
	book_name[MAX_BOOK_LEN];
	int days_available;
	scanf("%s", user_name);
	scanf("%c", &garbage);
	get_book_name(book_name);
	scanf("%d", &days_available);

	if (ht_has_key(*user, user_name) == 0) {
		printf("%s", USER_NOT_REGISTERED);
	} else {
		int valid = 1;
		user_t *u = (user_t *)ht_get(*user, user_name);
		if (u->banned == 1) {
			printf("%s", BANNED_USER);
			valid = 0;
		} else if (u->borrow == 1) {
			printf("%s", USER_BORROWED);
			valid = 0;
		} else if (ht_has_key(*library, book_name) == 0) {
			printf("%s", BOOK_NOT_FOUND);
			valid = 0;
		} else if (valid == 1){
			book_t *b = (book_t *)ht_get(*library, book_name);
			if (b->borrowed == 1) {
				printf("%s", BORROWED_BOOK);
				memmove(u->book_name, random_value, strlen(random_value) + 1);
			} else if (valid == 1 && b->borrowed == 0) {
				u->borrow = 1;
				u->days = days_available;
				b->borrowed = 1;
				memmove(u->book_name, book_name, strlen(book_name) + 1);
			}
		}
	}
}

void RETURN_BOOK(hashtable_t **user, hashtable_t **library) {
	char garbage, book_name[MAX_BOOK_LEN], user_name[MAX_DEF_LEN];
	int rating, days_since_borrow;
	scanf("%s", user_name);
	scanf("%c", &garbage);
	get_book_name(book_name);
	scanf("%d %d", &days_since_borrow, &rating);

	user_t *u = (user_t *)ht_get(*user, user_name);
	int valid = 0;
	if (u->banned == 1) {
		printf("%s", BANNED_USER);
		valid = 1;
	}
	int check = 0;
	if (valid == 0) {
		if (u->borrow == 0 || strcmp(u->book_name, book_name) != 0) {
			printf("You didn't borrow this book.\n");
			check = 1;
		}
		if (check == 0) {
			if (days_since_borrow - u->days > 0) {
				int lost_points = (days_since_borrow - u->days) * 2;
				u->points = u->points - lost_points;
			}
			if (days_since_borrow - u->days < 0) {
				u->points = u->points + u->days - days_since_borrow;
			}
			if (u->points < 0) {
				u->banned = 1;
				u->borrow = 0;
				book_t *b = (book_t *)ht_get(*library, book_name);
				b->purchases = b->purchases + 1;
				b->rating = (b->rating * (b->purchases - 1) + rating) /
				b->purchases;
				b->borrowed = 0;
				printf("The user %s has been banned from this library.\n",
				user_name);
			} else  {
				u->borrow = 0;
				book_t *b = (book_t *)ht_get(*library, book_name);
				b->purchases = b->purchases + 1;
				b->rating = (b->rating * (b->purchases - 1) + rating) /
				b->purchases;
				b->borrowed = 0;
			}
		}
	}
}

void LOST_BOOK(hashtable_t **user, hashtable_t **library) {
	char garbage, user_name[MAX_DEF_LEN], book_name[MAX_BOOK_LEN];
	scanf("%s", user_name);
	scanf("%c", &garbage);
	get_book_name(book_name);

	int valid = 1;
	if (ht_has_key(*user, user_name) == 0) {
		printf("%s", USER_NOT_REGISTERED);
		valid = 0;
	}
	if (valid == 1) {
		user_t *u = (user_t *)ht_get(*user, user_name);
		if (u->banned == 1) {
			printf("%s", BANNED_USER);
		} else {
			u->points = u->points - 50;
			if (u->points < 0) {
				u->banned = 1;
				printf("The user %s has been banned from this library.\n",
				user_name);
			}
			u->borrow = 0;
			if (ht_has_key(*library, book_name) == 0) {
				printf("%s", BOOK_NOT_FOUND);
				return;
			}
			book_t *b = (book_t *)ht_get(*library, book_name);
			hashtable_t *defs = (hashtable_t *)b->book;
			ht_free(&defs, 2);
			ht_remove_entry(*library, book_name);
		}
	}
}
