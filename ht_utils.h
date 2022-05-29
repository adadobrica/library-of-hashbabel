// Copyright 2022 Dobrica Nicoleta Adriana
#ifndef HT_UTILS_H_
#define HT_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "/home/student/structs.h"

ll_t* list_create(int data_size);
void free_list(ll_t **list);
node_t* get_nth_node(ll_t *list, int n);
void add_nth_node(ll_t *list, int n, const void *value);
node_t* remove_nth_node(ll_t *list, int n);
int compare_function_strings(void *a, void *b);
unsigned int hash_function_string(void *a);
hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*));
int ht_has_key(hashtable_t *ht, void *key);
void * ht_get(hashtable_t *ht, void *key);
void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size);
void ht_remove_entry(hashtable_t *ht, void *key);
void ht_free(hashtable_t **ht, int type);
int check_for_resize(hashtable_t *ht);
void RESIZE_HASHTABLE(hashtable_t **ht, int type);

#endif  //  HT_UTILS_H_
