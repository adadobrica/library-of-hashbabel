// Copyright 2022 Dobrica Nicoleta Adriana
#ifndef STRUCTS_H_
#define STRUCTS_H_

#define MAX_BOOK_LEN 40
#define MAX_DEF_LEN 20

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
	double rating;
	int purchases;
	int borrowed;
} book_t;


#endif  //  STRUCTS_H_
