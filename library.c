#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define HMAX_LIBRARY 10
#define HMAX_USERS 10
#define HMAX_BOOKS 10
#define MAX_BOOK_LEN 40
#define MAX_STRING_SIZE 256
#define MAX_DEF_LEN 20
#define EPS 1e-3

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

typedef struct user_t {
	char user_name[MAX_DEF_LEN];
	int points;
	int banned;
	int borrow;
	int days;
	char book_name[MAX_BOOK_LEN];
} user_t;


typedef struct book_t {
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
    memmove(new_node->data, value, list->data_size);

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
hash_function_string(void *a)
{
	unsigned char *puchar_a = (unsigned char *)a;
	unsigned int hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = (( hash << 5u) + hash) + c;

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

void RESIZE_HASHTABLE(hashtable_t **ht, int type) {
	int new_size = (*ht)->hmax * 2;
	hashtable_t *new_ht = ht_create(new_size, hash_function_string, compare_function_strings);
	for (int i = 0; i < (*ht)->hmax; i++) {
		node_t *current = (*ht)->buckets[i]->head, *prev;
		while (current != NULL) {
            prev = current;
			info *data = (info *)current->data;
            int key_size = strlen((char *)data->key) + 1, value_size = 0;
			if (type == 1) { // definitii
				char *value = (char *)data->value;
				value_size = strlen(value) + 1;
			} else if (type == 2) { // carti
				value_size = sizeof(book_t);
			} else if (type == 3) { // useri
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


/*void RESIZE_HASHTABLE(hashtable_t *ht) {
	int new_size = ht->hmax * 2;
	ll_t **new_buckets = malloc(sizeof(ll_t *) * new_size);

	for (int i = 0; i < new_size; i++) {
		new_buckets[i] = list_create(sizeof(info));
	}
	for (int i = 0; i < ht->hmax; i++) {
		node_t *current = ht->buckets[i]->head;
		node_t *next = NULL, *prev = NULL;
		while (current) {
			next = current->next;
			info *data = (info *)current->data;
			ll_t *new_bucket = new_buckets[ht->hash_function(data->key) % new_size];
			add_nth_node(new_bucket, 0, current->data);
			free(current->data);
			free(current);
			current = next;
		}
		free(ht->buckets[i]);
	}
	free(ht->buckets);
	ht->buckets = new_buckets;
	ht->hmax = new_size;
}
*/

void print_book(hashtable_t *library, char book_name[MAX_BOOK_LEN]) {
	if (ht_has_key(library, book_name) == 0) {
		printf("%s", BOOK_NOT_FOUND);
	} else {
		 book_t *book = (book_t *)ht_get(library, book_name);
		 //char name[MAX_BOOK_LEN];
		 //char *rest = book->name;
		 //char *ptr = strtok_r(rest, "\"", &rest)
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

	for (int i = 0; i < library->hmax; i++) {
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
		printf(" Rating:%0.3f Purchases:%d\n", books[i].rating, books[i].purchases);
	}
	free(books);
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

			//char *rest = user_data->user_name;
			//char *ptr = strtok_r(rest, "\"", &rest);
			memmove(top_user_name[n_size++], user_data->user_name, strlen(user_data->user_name) + 1);
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
				memmove(aux_c, top_user_name[i], strlen(top_user_name[i]) + 1);
				memmove(top_user_name[i], top_user_name[j], strlen(top_user_name[j]) + 1);
				memmove(top_user_name[j], aux_c, strlen(aux_c) + 1);
			} else if (top_user_points[i] == top_user_points[j]) {
				if (strcmp(top_user_name[i], top_user_name[j]) > 0) {
					char aux_c[MAX_DEF_LEN];
					memmove(aux_c, top_user_name[i], strlen(top_user_name[i]) + 1);
					memmove(top_user_name[i], top_user_name[j], strlen(top_user_name[j]) + 1);
					memmove(top_user_name[j], aux_c, strlen(aux_c) + 1);

					//float aux_f = top_user_rating[i];
					//top_user_rating[i] = top_user_rating[j];
					//top_user_rating[j] = aux_f;
				}
			}
		}
	}

	printf("Users ranking:\n");
	for (int i = 0; i < p_size; i++) {
		if (top_user_points[i] < 0) {
			continue;
		}
		printf("%d. Name:%s Points:%d\n", i + 1, top_user_name[i], top_user_points[i]);
	}
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
	char book_name[MAX_BOOK_LEN], garbage, line[MAX_STRING_SIZE];
	new_book->purchases = 0;
	new_book->rating = 0;
	new_book->borrowed = 0;
	new_book->book = ht_create(HMAX_BOOKS, hash_function_string, compare_function_strings);
	
	scanf("%c", &garbage);
	get_book_name(book_name);
	scanf("%d", &def_number);
	//fgets(line, MAX_STRING_SIZE - 1, stdin);

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
	ht_put(*library, new_book->name, strlen(new_book->name) + 1, new_book, sizeof(book_t));
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
	//fgets(book_name, MAX_BOOK_LEN - 1, stdin);
	//char *rest = book_name;
	//char *ptr = strtok_r(rest, "\"", &rest);
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
		ht_put(*user, new_user.user_name, strlen(new_user.user_name) + 1, &new_user, sizeof(user_t));
	}
}

void BORROW_BOOK(hashtable_t **user, hashtable_t **library) {
	char garbage, line[MAX_STRING_SIZE], random_value[2] = "X", user_name[MAX_DEF_LEN], book_name[MAX_BOOK_LEN];
	int days_available;
	//scanf("%c", &garbage);
	//scanf("%s %s %d", user_name, book_name, &days_available);
	//fgets(line, MAX_STRING_SIZE - 1, stdin);
	//char *rest = line;
	//char *user_name = strtok_r(rest, " ", &rest);
	//char *book_name = strtok_r(NULL, "\"", &rest);
	//char *days = strtok_r(NULL, "\n", &rest);
	//days_available = atoi(days);
	
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
	char garbage, line[MAX_STRING_SIZE], book_name[MAX_BOOK_LEN], user_name[MAX_DEF_LEN];
	//scanf("%c", &garbage);
	int rating, days_since_borrow;
	//fgets(line, MAX_STRING_SIZE - 1, stdin);
	//char *rest = line;
	//char *user_name = strtok_r(rest, " ", &rest);
	//char *book_name_string = strtok_r(NULL, "\"", &rest);
	//char *days = strtok_r(NULL, " ", &rest);
	//char *r = strtok_r(NULL, " ", &rest);
	//memmove(book_name, book_name_string, strlen(book_name_string) + 1);
	//days_since_borrow = atoi(days);
	//rating = atoi(r);

	//scanf("%s %s %d %d", user_name, book_name, &days_since_borrow, &rating);
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
				b->rating = (b->rating * (b->purchases - 1) + rating) / b->purchases;
				b->borrowed = 0;
				printf("The user %s has been banned from this library.\n", user_name);
			} else  {
				u->borrow = 0;
				book_t *b = (book_t *)ht_get(*library, book_name);
				b->purchases = b->purchases + 1;
				b->rating = (b->rating * (b->purchases - 1) + rating) / b->purchases;
				b->borrowed = 0;
			}
		}
	}
}

void LOST_BOOK(hashtable_t **user, hashtable_t **library) {
	char garbage, line[MAX_STRING_SIZE], user_name[MAX_DEF_LEN], book_name[MAX_BOOK_LEN];
	//scanf("%c", &garbage);
	//fgets(line, MAX_STRING_SIZE - 1, stdin);
	//char *rest = line;
	//char *user_name = strtok_r(rest, " ", &rest);
	//char *book_name = strtok_r(rest, "\"", &rest);
	//scanf("%s %s", user_name, book_name);
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
				printf("The user %s has been banned from this library.\n", user_name);
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

int main(void) {
	hashtable_t *library = ht_create(HMAX_LIBRARY, hash_function_string, compare_function_strings);
	hashtable_t *user = ht_create(HMAX_USERS, hash_function_string, compare_function_strings);
	char book_name[MAX_BOOK_LEN], user_name[MAX_DEF_LEN];
	book_t new_book;
	while (1) {
		char command[MAX_STRING_SIZE];
		scanf("%s", command);
		//printf("%s\n", command);
		if (strcmp(command, "ADD_BOOK") == 0) {
			ADD_BOOK(&library, &new_book);
		} else if (strcmp(command, "GET_BOOK") == 0) {
			GET_BOOK(library);
		} else if (strcmp(command, "RMV_BOOK") == 0) {
			REMOVE_BOOK(&library);
		} else if (strcmp(command, "ADD_DEF") == 0) {
			char garbage, key[MAX_DEF_LEN], val[MAX_DEF_LEN];
			//fgets(line, MAX_STRING_SIZE - 1, stdin);
			//char *rest = line;
			//char *garb = strtok_r(rest, "\"", &rest);
			//char *book_name = strtok_r(NULL, "\"", &rest);
			//char *key_string = strtok_r(NULL, " ", &rest);
			//char *val_string = strtok_r(NULL, " \n", &rest);
			//char val[MAX_DEF_LEN], key[MAX_DEF_LEN];

			//memmove(val, val_string, strlen(val_string) + 1);
			//memmove(key, key_string, strlen(key_string) + 1);
			scanf("%c", &garbage);
			get_book_name(book_name);
			scanf("%s %s", key, val);
			int ok = 0;

			if (ht_has_key(library, book_name) == 0) {
				printf("%s", BOOK_NOT_FOUND);
				ok = 1;
			}
			if (ok == 0) {
				book_t *b = (book_t *)ht_get(library, book_name);
				if (check_for_resize(b->book)) {
					hashtable_t *defs = b->book;
					RESIZE_HASHTABLE(&defs, 1);
					b->book = defs;
				}
				ht_put(b->book, key, strlen(key) + 1, val, strlen(val) + 1);
			}
		} else if (strcmp(command, "GET_DEF") == 0) {
			char garbage, key[MAX_DEF_LEN];
			int ok = 0;
			scanf("%c", &garbage);
			get_book_name(book_name);
			scanf("%s", key);
			//fgets(line, MAX_STRING_SIZE - 1, stdin);
			//char *rest = line;
			//char *garbage = strtok_r(rest, "\"", &rest);
			//char *book_name = strtok_r(NULL, "\"", &rest);
			//char *key_string = strtok_r(NULL, " \n", &rest);
			//char key[MAX_DEF_LEN];
			//memmove(key, key_string, strlen(key_string) + 1);

			if (ht_has_key(library, book_name) == 0) {
				printf("%s", BOOK_NOT_FOUND);
				ok = 1;
			} 
			if (ok == 0) {
				book_t *b = (book_t *)ht_get(library, book_name);
				hashtable_t *defs = (hashtable_t *)b->book;
				if (ht_has_key(defs, key) == 0) {
					printf("%s", DEF_NOT_FOUND);
				} else {
					char *val = (char *)ht_get(defs, key);
					printf("%s\n", val);
				}
			}
		} else if (strcmp(command, "RMV_DEF") == 0) {
			char garbage, key[MAX_DEF_LEN];
			scanf("%c", &garbage);
			get_book_name(book_name);
			scanf("%s", key);
			//fgets(line, MAX_STRING_SIZE - 1, stdin);
			//char *rest = line;
			//char *garb = strtok_r(rest, "\"", &rest);
			//char *book_name = strtok_r(NULL, "\"", &rest);
			//char *key_string = strtok_r(NULL, " \n", &rest);
			//memmove(key, key_string, strlen(key_string) + 1);

			int ok = 0;
			if (ht_has_key(library, book_name) == 0) {
				printf("%s", BOOK_NOT_FOUND);
				ok = 1;
			}
			if (ok == 0) {
				book_t *b = (book_t *)ht_get(library, book_name);
				hashtable_t *def = (hashtable_t *)b->book;
				if (ht_has_key(def, key) == 0) {
					printf("%s",DEF_NOT_FOUND);
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