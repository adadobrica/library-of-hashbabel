//Copyright 2022 Dobrica Nicoleta Adriana
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structs.h"

ll_t* list_create(int data_size) {
	ll_t *list = malloc(sizeof(ll_t));
	list->data_size = data_size;
	list->size = 0;
	list->head = NULL;
	return list;
}

void free_list(ll_t **list) {
	node_t *aux = (*list)->head;
	while (aux) {
		node_t *next = aux->next;
		free(aux->data);
		free(aux);
		aux = next;
	}
	free(*list);
	*list = NULL;
}

node_t* get_nth_node(ll_t *list, int n) {
	node_t *curr = list->head;
	while (curr != NULL && n-- > 0) {
		curr = curr->next;
	}
	return curr;
}

void add_nth_node(ll_t *list, int n, const void *value) {
	node_t *prev, *curr;
    node_t* new_node;

    if (!list) {
        return;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(*new_node));
    new_node->data = malloc(list->data_size);
    memmove(new_node->data, value, list->data_size);

    new_node->next = curr;

    if (prev == NULL) {
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}

node_t* remove_nth_node(ll_t *list, int n) {
	node_t *prev, *curr;

    if (n > list->size - 1) {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;
	return curr;
}

int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

unsigned int
hash_function_string(void *a)
{
	unsigned char *puchar_a = (unsigned char *)a;
	unsigned int hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c;

	return hash;
}

hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
	hashtable_t *htable = malloc(sizeof(hashtable_t));
    htable->buckets = malloc(hmax * sizeof(ll_t *));
    for (unsigned int i = 0; i < hmax; i++) {
        htable->buckets[i] = list_create(sizeof(info));
    }
    htable->hmax = hmax;
    htable->size = 0;
    htable->hash_function = hash_function;
    htable->compare_function = compare_function;
	return htable;
}

int
ht_has_key(hashtable_t *ht, void *key)
{
	int index = ht->hash_function(key) % ht->hmax;
	ll_t *bucket = ht->buckets[index];
	node_t *current = bucket->head;
	while (current) {
		info *new_data = (info *)current->data;
		if (ht->compare_function(key, new_data->key) == 0) {
			return 1;
		}
		current = current->next;
	}
	return 0;
}

void *
ht_get(hashtable_t *ht, void *key)
{
	int index = ht->hash_function(key) % ht->hmax;
	ll_t *bucket = ht->buckets[index];
	if (bucket->size == 0) {
		return NULL;
	}
	node_t *current = bucket->head;
	info *new_data = (info *)current->data;

	while (current) {
		if (ht->compare_function(key, new_data->key) != 0) {
			current = current->next;
			new_data = (info *)current->data;
		} else {
			return new_data->value;
		}
	}
	return NULL;
}

void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	int index;
	index = ht->hash_function(key) % ht->hmax;
	info *new_data = malloc(sizeof(info));

	node_t *current = ht->buckets[index]->head;
	while (current) {
		info *data = (info *)current->data;
		if (ht->compare_function(key, data->key) == 0) {
			void *tmp = realloc(data->value, value_size);
			data->value = tmp;
			memmove(data->value, value, value_size);
			free(new_data);
			return;
		}
		current = current->next;
	}
	new_data->key = malloc(key_size);
	new_data->value = malloc(value_size);
	memmove(new_data->key, key, key_size);
	memmove(new_data->value, value, value_size);
	add_nth_node(ht->buckets[index], 0, new_data);
	free(new_data);
	ht->size++;
}

void
ht_remove_entry(hashtable_t *ht, void *key)
{
	int index = ht->hash_function(key) % ht->hmax;
	ll_t *bucket = ht->buckets[index];
	int cnt = 0;
	node_t *current = bucket->head;
	info *new_data = (info *)current->data;
	if (ht_has_key(ht, key) == 0) {
		return;
	}
	while (current) {
		if (ht->compare_function(key, new_data->key)) {
			cnt++;
			current = current->next;
			new_data = (info *)current->data;
		} else {
			break;
		}
	}

	current = remove_nth_node(bucket, cnt);
	if (!bucket->size)
		bucket->head = NULL;
	info *data = (info *)current->data;
	free(data->value);
	free(data->key);
	free(current->data);
	free(current);
	ht->size--;
}

void
ht_free(hashtable_t **ht, int type)
{
    node_t *current;
	ll_t *bucket;
	if (*ht == NULL) {
		return;
	}

	for (unsigned int i = 0; i < (*ht)->hmax; i++) {
		bucket = (*ht)->buckets[i];
		current = bucket->head;
		while (current) {
			info *data = (info *)current->data;
			free(data->key);
            if (type == 2) {
			    free(data->value);
            } else if (type == 1) {
                book_t *book_data = data->value;
                ht_free(&book_data->book, 2);
                free(data->value);
            }
			current = current->next;
		}
		free_list(&bucket);
	}
	free((*ht)->buckets);
	free(*ht);
}

int check_for_resize(hashtable_t *ht) {
	int check = 0;
	float num_entries = ht->size;
	float load_factor = num_entries / ht->hmax;
	if (load_factor > 1) {
		check = 1;
	}
	return check;
}

void RESIZE_HASHTABLE(hashtable_t **ht, int type) {
	int new_size = (*ht)->hmax * 2;
	hashtable_t *new_ht = ht_create(new_size, hash_function_string,
			compare_function_strings);
	for (unsigned int i = 0; i < (*ht)->hmax; i++) {
		node_t *current = (*ht)->buckets[i]->head, *prev;
		while (current != NULL) {
            prev = current;
			info *data = (info *)current->data;
            int key_size = strlen((char *)data->key) + 1, value_size = 0;
			if (type == 1) {
				// definitii
				char *value = (char *)data->value;
				value_size = strlen(value) + 1;
			} else if (type == 2) {
				// carti
				value_size = sizeof(book_t);
			} else if (type == 3) {
				// useri
				value_size = sizeof(user_t);
			}
			ht_put(new_ht, data->key, key_size, data->value, value_size);
            free(data->key);
            free(data->value);
			current = current->next;
            free(prev->data);
            free(prev);
		}
        free((*ht)->buckets[i]);
	}
    free((*ht)->buckets);
    free(*ht);
    *ht = new_ht;
}
