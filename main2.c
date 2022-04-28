#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HMAX_LIBRARY 10
#define HMAX_USERS 10
#define HMAX_BOOKS 10
#define MAX_BOOK_LEN 40
#define MAX_STRING_SIZE 256
#define MAX_DEF_LEN 20

#define USER_REGISTERED "User is already registered.\n"
#define USER_NOT_REGISTERED "You are not registered yet.\n"
#define BANNED_USER "You are banned from this library.\n"
#define BOOK_NOT_FOUND "The book is not in the library.\n"
#define BORROWED_BOOK "The book is borrowed.\n"
#define USER_BORROWED "You have already borrowed a book.\n"
#define DEF_NOT_FOUND "The definition is not in the book.\n"

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
	int days;
	char book_name[MAX_BOOK_LEN];
} user_t;


typedef struct book_t {
	def_t definitions;
	hashtable_t *book;
	char name[MAX_BOOK_LEN];
	float rating;
	int purchases;
	int borrowed;
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

	/*while (ht->compare_function(key, new_data->key) != 0) {
		if (current != NULL) {
        	current = current->next;
        	new_data = (info *)current->data;
        }
	}
	*/
	while (current) {
		if (ht->compare_function(key, new_data->key) != 0) {
			current = current->next;
			new_data = (info *)current->data;
		} else {
			return new_data->value;
		}
	}
}

void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	int index;
	index = ht->hash_function(key) % ht->hmax;
	info *new_data = malloc(sizeof(info));
	int found = 0;

	node_t *current = ht->buckets[index]->head;
	while (current) {
		info *data = (info *)current->data;
		if (ht->compare_function(key, data->key) == 0) {
			void *tmp = realloc(data->value, value_size);
			data->value = tmp;
			memcpy(data->value, value, value_size);
			found = 1;
		}
		current = current->next;
		if (found == 1) {
			free(new_data);
			return;
		}
	}
	new_data->key = malloc(key_size);
	new_data->value = malloc(value_size);
	memcpy(new_data->key, key, key_size);
	memcpy(new_data->value, value, value_size);
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
ht_free(hashtable_t **ht, int type)
{	
    node_t *current;
	ll_t *bucket;
	for (int i = 0; i < (*ht)->hmax; i++) {
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

void RESIZE_HASHTABLE(hashtable_t **ht, int value_size) {
	int new_size = (*ht)->hmax * 2;
	hashtable_t *new_ht = ht_create(new_size, hash_function_string, compare_function_strings);
	for (int i = 0; i < (*ht)->hmax; i++) {
		node_t *current = (*ht)->buckets[i]->head, *prev;
		while (current) {
            prev = current;
			info *data = (info *)current->data;
            int key_size = strlen((char *)data->key);
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

void print_book(hashtable_t *library, char book_name[MAX_BOOK_LEN]) {
	if (ht_has_key(library, book_name) == 0) {
		printf(BOOK_NOT_FOUND);
	} else {
		 book_t *book = (book_t *)ht_get(library, book_name);
		 char name[MAX_BOOK_LEN];
		 char *rest = book->name;
		 char *ptr = strtok_r(rest, "\"", &rest);
		 printf("Name:%s Rating:%0.3f Purchases:%d\n", ptr, book->rating, book->purchases);
	}	
}

void print_top_books(hashtable_t *library) {
	char top_book_name[10][MAX_BOOK_LEN];
	int n_size = 0, r_size = 0, p_size = 0;
	float top_book_rating[10];
	int top_book_purchases[10];

	for (int i = 0; i < library->hmax; i++) {
		node_t *current = library->buckets[i]->head;
		while (current) {
			info *data = (info *)current->data;
			book_t *book_data = (book_t *)data->value;
			char *rest = book_data->name;
			char *ptr = strtok_r(rest, "\"", &rest);
			memcpy(top_book_name[n_size++], ptr, strlen(ptr) + 1);
			top_book_rating[r_size++] = book_data->rating;
			top_book_purchases[p_size++] = book_data->purchases;
			current = current->next;
		}
	}
	for (int i = 0; i < r_size; i++) {
		for (int j = i + 1; j < r_size; j++) {
			if (top_book_rating[i] < top_book_rating[j]) {
				float aux_f = top_book_rating[i];
				top_book_rating[i] = top_book_rating[j];
				top_book_rating[j] = aux_f;

				int aux_i = top_book_purchases[i];
				top_book_purchases[i] = top_book_purchases[j];
				top_book_purchases[j] = aux_i;

				char aux_c[MAX_BOOK_LEN];
				memcpy(aux_c, top_book_name[i], strlen(top_book_name[i]) + 1);
				memcpy(top_book_name[i], top_book_name[j], strlen(top_book_name[j]) + 1);
				memcpy(top_book_name[j], aux_c, strlen(aux_c) + 1);
			}
		}
	}
	printf("Top books rating:\n");
	for (int i = 0; i < r_size; i++) {
		printf("%d. Name:%s Rating:%0.3f Purchases:%d\n", i + 1, top_book_name[i], top_book_rating[i], top_book_purchases[i]);
	}
}

void print_top_users(hashtable_t *users) {
	char top_user_name[users->size][MAX_BOOK_LEN];
	int top_user_points[users->size];
	int n_size = 0, p_size = 0;

	for (int i = 0; i < users->hmax; i++) {
		node_t *current = users->buckets[i]->head;
		while (current) {
			info *data = (info *)current->data;
			user_t *user_data = (user_t *)data->value;

			char *rest = user_data->user_name;
			char *ptr = strtok_r(rest, "\"", &rest);
			memcpy(top_user_name[n_size++], ptr, strlen(ptr) + 1);
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
				memcpy(aux_c, top_user_name[i], strlen(top_user_name[i]) + 1);
				memcpy(top_user_name[i], top_user_name[j], strlen(top_user_name[j]) + 1);
				memcpy(top_user_name[j], aux_c, strlen(aux_c) + 1);
			}
		}
	}

	printf("Top users rating:\n");
	for (int i = 0; i < p_size; i++) {
		printf("%d. Name:%s Points:%d\n", i + 1, top_user_name[i], top_user_points[i]);
	}
}

void ADD_BOOK(hashtable_t **library, book_t new_book) {
	int num = 0;
	int def_number;
	new_book.purchases = 0;
	new_book.rating = 0;
	new_book.borrowed = 0;
	new_book.book = ht_create(HMAX_BOOKS, hash_function_string, compare_function_strings);
	scanf("%s %d", new_book.name, &def_number);

	while (num != def_number) {
		char key[30], val[30];
		scanf("%s %s", key, val);
		ht_put(new_book.book, key, strlen(key) + 1, val, strlen(val) + 1);
		num++;
	}
			
	ht_put(*library, new_book.name, strlen(new_book.name) + 1, &new_book, sizeof(book_t));
}

void GET_BOOK(hashtable_t *library) {
	char book_name[MAX_BOOK_LEN];
	scanf("%s", book_name);
	if (ht_has_key(library, book_name) == 0) {
		printf(BOOK_NOT_FOUND);
	} else {
		print_book(library, book_name);
	}
}

void REMOVE_BOOK(hashtable_t **library) {
	char book_name[MAX_BOOK_LEN];
	scanf("%s", book_name);
	if (ht_has_key(*library, book_name) == 0) {
		printf(BOOK_NOT_FOUND);
	} else {
		ht_remove_entry(*library, book_name);
	}
}

void ADD_USER(hashtable_t **user) {
	
	user_t new_user;
	scanf("%s", new_user.user_name);
	if (ht_has_key(*user, new_user.user_name) == 1) {
		printf(USER_REGISTERED);
	} else {
		new_user.banned = 0;
		new_user.borrow = 0;
		new_user.points = 100;
		int ok = check_for_resize(*user);
		if (ok == 1) {
            int value_size = sizeof(user_t);
			RESIZE_HASHTABLE(user, value_size);
		}
		ht_put(*user, new_user.user_name, strlen(new_user.user_name) + 1, &new_user, sizeof(user_t));
	}
}

void BORROW_BOOK(hashtable_t **user, hashtable_t **library) {
	char user_name[MAX_DEF_LEN], book_name[MAX_BOOK_LEN];
	int days_available;
	scanf("%s %s %d", user_name, book_name, &days_available);
	if (ht_has_key(*user, user_name) == 0) {
		printf(USER_NOT_REGISTERED);
	} else {
		int valid = 1;
		user_t *u = (user_t *)ht_get(*user, user_name);
		if (u->banned == 1) {
			printf(BANNED_USER);
			valid = 0;
		} else if (u->borrow == 1) {
			printf(USER_BORROWED);
			valid = 0;
		}
		if (ht_has_key(*library, book_name) == 0) {
			printf(BOOK_NOT_FOUND);
			valid = 0;
		} else {
			book_t *b = (book_t *)ht_get(*library, book_name);
			if (b->borrowed == 1) {
				printf(BORROWED_BOOK);
			} else if (valid == 1 && b->borrowed == 0) {
				u->borrow = 1;
				u->days = days_available;
				b->borrowed = 1;
				memcpy(u->book_name, book_name, strlen(book_name) + 1);
			}
		}
	}
}

void RETURN_BOOK(hashtable_t **user, hashtable_t **library) {
	char user_name[MAX_DEF_LEN], book_name[MAX_BOOK_LEN];
	int rating, days_since_borrow;
	scanf("%s %s %d %d", user_name, book_name, &days_since_borrow, &rating);
	user_t *u = (user_t *)ht_get(*user, user_name);
	int valid = 0;
	if (u->banned == 1) {
		printf(BANNED_USER);
		valid = 1;
	}

	if (valid == 0) {
		if (days_since_borrow - u->days > 0) {
			int lost_points = (days_since_borrow - u->days) * 2;
			u->points = u->points - lost_points;
		}
		if (days_since_borrow - u->days < 0) {
			u->points = u->points + u->days - days_since_borrow;
		}
		if (u->points < 0) {
			u->banned = 1;
			printf("The user %s has been banned.\n", user_name);
		}
		if (strcmp(u->book_name, book_name) != 0 || u->borrow == 0) {
			printf("You didn't borrow this book.\n");
		} else {
			u->borrow = 0;
			book_t *b = (book_t *)ht_get(*library, book_name);
			b->purchases = b->purchases + 1;
			b->rating = (b->rating + rating) / b->purchases;
			b->borrowed = 0;
		}
	}
}

void LOST_BOOK(hashtable_t **user, hashtable_t **library) {
	char user_name[MAX_DEF_LEN], book_name[MAX_BOOK_LEN];
	scanf("%s %s", user_name, book_name);
	int valid = 1;
	if (ht_has_key(*user, user_name) == 0) {
		printf(USER_NOT_REGISTERED);
		valid = 0;
	}
	if (valid == 1) {
		user_t *u = (user_t *)ht_get(*user, user_name);
		if (u->banned == 1) {
			printf(BANNED_USER);
		} else {
			u->points = u->points - 50;
			if (u->points < 0) {
				u->banned = 1;
				printf("The user %s has been banned.\n", user_name);
			}
			u->borrow = 0;
			ht_remove_entry(*library, book_name);
		}
	}
}

int main(void) {
	hashtable_t *library = ht_create(HMAX_LIBRARY, hash_function_string, compare_function_strings);
	hashtable_t *user = ht_create(HMAX_USERS, hash_function_string, compare_function_strings);
	char book_name[MAX_BOOK_LEN], user_name[MAX_DEF_LEN];
	book_t new_book;
	while (1) {
		char command[MAX_STRING_SIZE];
		scanf("%s", command);
		if (strcmp(command, "ADD_BOOK") == 0) {
			ADD_BOOK(&library, new_book);
		} else if (strcmp(command, "GET_BOOK") == 0) {
			GET_BOOK(library);
		} else if (strcmp(command, "RMV_BOOK") == 0) {
			REMOVE_BOOK(&library);
		} else if (strcmp(command, "ADD_DEF") == 0) {
			char key[30], val[30];
			scanf("%s %s %s", book_name, key, val);
			book_t *b = (book_t *)ht_get(library, book_name);
			ht_put(b->book, key, strlen(key) + 1, &val, strlen(val) + 1);
		} else if (strcmp(command, "GET_DEF") == 0) {
			char key[30];
			int ok = 0;
			scanf("%s %s", book_name, key);
			if (ht_has_key(library, book_name) == 0) {
				printf(BOOK_NOT_FOUND);
				ok = 1;
			} 
			if (ok == 0) {
				book_t *b = (book_t *)ht_get(library, book_name);
				hashtable_t *defs = (hashtable_t *)b->book;
				if (ht_has_key(defs, key) == 0) {
					printf(DEF_NOT_FOUND);
				} else {
					char *val = (char *)ht_get(defs, key);
					printf("%s\n", val);
				}
			}
		} else if (strcmp(command, "RMV_DEF") == 0) {
			char key[30];
			scanf("%s %s", book_name, key);
			int ok = 0;
			if (ht_has_key(library, book_name) == 0) {
				printf(BOOK_NOT_FOUND);
				ok = 1;
			}
			if (ok == 0) {
				book_t *b = (book_t *)ht_get(library, book_name);
				hashtable_t *def = (hashtable_t *)b->book;
				if (ht_has_key(def, key) == 0) {
					printf(DEF_NOT_FOUND);
				} else {
					ht_remove_entry(def, key);
				}
			}
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