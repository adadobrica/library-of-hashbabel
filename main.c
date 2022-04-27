#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HMAX 10
#define MAX_BOOK_LEN 40
#define MAX_STRING_SIZE 256
#define MAX_DEF_LEN 20

#define USER_REGISTERED "User is already registered.\n"
#define USER_NOT_REGISTERED "You are not registered yet.\n"
#define BANNED_USER "You are banned from this library.\n"
#define BOOK_NOT_FOUND "The book is not in the library.\n"
#define BORROWED_BOOK "The book is borrowed.\n"
#define USER_BORROWED "You have already borrowed a book.\n"

typedef struct node_t
{
	void* data;
	struct node_t *next, *prev;
} node_t;

typedef struct ll_t
{
	node_t *head;
	int data_size;
	int size;
} ll_t;

typedef struct info info;
struct info {
	void *key;
	void *value;
};

typedef struct hashtable_t hashtable_t;
struct hashtable_t {
	ll_t **buckets; 
	unsigned int size;
	unsigned int hmax; 
	unsigned int (*hash_function)(void*);
	int (*compare_function)(void*, void*);
};


typedef struct def_t {
	char def_key[MAX_DEF_LEN];
	char def_val[MAX_DEF_LEN];
} def_t;

typedef struct user_t {
	char user_name[MAX_DEF_LEN];
	int points;
	int banned;
	int borrow;
} user_t;


typedef struct book_t {
	def_t definitions;
	hashtable_t *book;
	char name[MAX_BOOK_LEN];
	float rating;
	int purchases;
} book_t;

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

void free_main_list(ll_t **list) {
	node_t *current = (*list)->head;
	ll_t *current_list;
	while (current) {
		current_list = *(ll_t **)current->data;
		free_list(&current_list);
		current = current->next;
	}
	free_list(list);
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
    memcpy(new_node->data, value, list->data_size);

    new_node->next = curr;

    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}

node_t* remove_nth_node(ll_t *list, int n) {
	node_t *prev, *curr;

    /* n >= list->size - 1 inseamna eliminarea nodului de la finalul listei. */
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
        /* Adica n == 0. */
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;
	return curr;
    free(curr->data);
    free(curr);
}

int
compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

unsigned int
hash_function_int(void *a)
{
	/*
	 * Credits: https://stackoverflow.com/a/12996028/7883884
	 */
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int
hash_function_string(void *a)
{
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char*) a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
	hashtable_t *htable = malloc(sizeof(hashtable_t));
    htable->buckets = malloc(hmax * sizeof(ll_t *));
    for (int i = 0; i < hmax; i++) {
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
	node_t *current = bucket->head;
	info *new_data = (info *)current->data;
	while (ht->compare_function(key, new_data->key) != 0) {
		if (current != NULL) {
        	current = current->next;
        	new_data = (info *)current->data;
        }
	}
	return new_data->value;
}

void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	int index;
	index = ht->hash_function(key) % ht->hmax;
	info *new_data = malloc(sizeof(info));
	if (ht_has_key(ht, key)) {
		void *val = ht_get(ht, key);
		memcpy(val, value, value_size);
		return;
	}
	new_data->key = malloc(key_size);
	new_data->value = malloc(value_size);
	memcpy(new_data->key, key, key_size);
	memcpy(new_data->value, value, value_size);
	add_nth_node(ht->buckets[index], 0, new_data);
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
	while (ht->compare_function(key, new_data->key)) {
		cnt++;
		current = current->next;
	}
	current = remove_nth_node(bucket, cnt);
	info *data = (info *)current->data;
	free(data->value);
	free(data->key);
	free(current->data);
	free(current);
	ht->size--;
}

void
ht_free(hashtable_t *ht)
{	
    node_t *current;
	ll_t *bucket;
	for (int i = 0; i < ht->hmax; i++) {
		bucket = ht->buckets[i];
		current = bucket->head;
		while (current) {
			info *data = (info *)current->data;
			free(data->key);
			free(data->value);
			current = current->next;
		}
		free_list(&bucket);
	}
	free(ht->buckets);
	free(ht);
}

void print_book(hashtable_t *library, char book_name[MAX_BOOK_LEN]) {
	if (ht_has_key(library, book_name) == 0) {
		printf(BOOK_NOT_FOUND);
	} else {
		 book_t *book = (book_t *)ht_get(library, book_name);
		 printf("%f %d", book.rating, book.purchases);
	}	
}


int main(void) {
	hashtable_t *library = ht_create(HMAX, hash_function_string, compare_function_strings);
	//hashtable_t *book = ht_create(HMAX, hash_function_string, compare_function_strings);
	hashtable_t *user = ht_create(HMAX, hash_function_string, compare_function_strings);
	int def_number, num = 0, days_available;
	char book_name[MAX_BOOK_LEN], user_name[MAX_DEF_LEN];
	while (1) {
		char command[MAX_STRING_SIZE];
		scanf("%s", command);
		if (strcmp(command, "ADD_BOOK") == 0) {
			book_t new_book;
			new_book.purchases = 0;
			new_book.rating = 0;
			new_book.book = ht_create(HMAX, hash_function_string, compare_function_strings);
			scanf("%s %d", new_book.name, &def_number);

			while (num != def_number) {
				scanf("%s %s", new_book.definitions.def_key, new_book.definitions.def_val);
				ht_put(new_book.book, new_book.definitions.def_key, strlen(new_book.definitions.def_key) + 1, &new_book.definitions, sizeof(book_t));
				num++;
			}
			
			ht_put(library, new_book.name, strlen(new_book.name) + 1, &new_book, sizeof(book_t));
		} else if (strcmp(command, "EXIT") == 0) {
			break;
		} else if (strcmp(command, "GET_BOOK") == 0) {
			scanf("%s", book_name);
			print_book(library, book_name);
		} else if (strcmp(command, "RMV_BOOK") == 0) {
			scanf("%s", book_name);
			ht_remove_entry(library, book_name);
		} else if (strcmp(command, "ADD_DEF") == 0) {
			def_t input;
			scanf("%s %s %s", book_name, input.def_key, input.def_val);
			book_t *b = (book_t *)ht_get(library, book_name);
			ht_put(b.book, input.def_key, strlen(input.def_key) + 1, &input, sizeof(def_t));
		} else if (strcmp(command, "GET_DEF") == 0) {
			def_t input;
			scanf("%s %s", book_name, input.def_key);
			book_t *b = (book_t *)ht_get(library, book_name);
			b->book = (hashtable_t *)ht_get(b->book, input.def_key);
			printf("%s\n", b->definitions->def_val);
		} else if (strcmp(command, "RMV_DEF") == 0) {
			def_t input;
			scanf("%s %s", book_name, input.def_key);
			book_t *b = ht_get(library, book_name);
			ht_remove_entry(b->book, input.def_key);
		} else if (strcmp(command, "ADD_USER") == 0) {
			user_t new_user;
			scanf("%s", new_user.user_name);
			if (ht_has_key(user, new_user.user_name) == 1) {
				printf(USER_REGISTERED);
			} else {
				new_user.banned = 0;
				new_user.borrow = 0;
				new_user.points = 100;
				ht_put(user, new_user.user_name, strlen(new_user.user_name) + 1, &new_user, sizeof(user_t));
			}
		} else if (strcmp(command, "BORROW") == 0) {
			scanf("%s %s %d", user_name, book_name, &days_available);
			if (ht_has_key(user, user_name) == 0) {
				printf(USER_NOT_REGISTERED);
			} else {
				int valid = 1;
				user_t *u = (user_t *)ht_get(user, user_name);
				if (u->banned == 1) {
					printf(BANNED_USER);
					valid = 0;
				} else if (u->borrow == 1) {
					printf(USER_BORROWED);
					valid = 0;
				}
				if (ht_has_key(library, book_name) == 0) {
					printf(BOOK_NOT_FOUND);
				} else if (valid == 1) {
					u->borrow = 1;
				}
			}
		}  	
	}
	return 0;
}
