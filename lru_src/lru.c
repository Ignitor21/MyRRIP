/*
********************************************************************************
*                                                                              
*        File with implementation of utility functions for LRU cache policy            
*                            (Least recently used)  
*                                                           
********************************************************************************
*/

#include "lru.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

const long HASH_SIZE = 131072;

struct node_t
{
	struct node_t* next;
	struct node_t* prev;
	long data;
};

struct list_t
{
	struct node_t* head;
	struct node_t* tail;
	long pages_cached; // number of cached pages
	long size;
};

struct hash_node_t
{
	struct hash_node_t* next;
	struct hash_node_t* prev;
	struct node_t* node;
};

struct hash_t 
{
	struct hash_node_t** arr;
	long len;
	long nodes_hashed; // number of hashed nodes
};

struct list_t* create_list(const long size)
{
	struct list_t* list = calloc(1, sizeof(struct list_t));

	if (list == NULL)
	{
		fprintf(stderr, "Memory exhausted\n");
		abort();
	}

	list->head = NULL;
	list->tail = NULL;
	list->pages_cached = 0;
	list->size = size;
	return list;
}

struct hash_t* create_hash()
{
	struct hash_t* hash = calloc(1, sizeof(struct hash_t));

	if (hash == NULL)
	{
		fprintf(stderr, "Memory exhausted\n");
		abort();
	}

	hash->arr = calloc(HASH_SIZE, sizeof(struct hash_node_t*));

	if (hash->arr == NULL)
	{
		fprintf(stderr, "Memory exhausted\n");
		abort();
	}

	hash->len = HASH_SIZE;
	hash->nodes_hashed = 0;

	return hash;
}

void print_list(const struct list_t* list)
{
	assert(list && "list is a NULL pointer");
	assert(list->head && "list is empty");

	struct node_t* cur = list->head;

	while (cur != NULL)
	{
		printf("%ld ", cur->data);
		cur = cur->next;
	}

	printf("\n");
	return;
}

void free_list(struct list_t* list)
{
	assert(list && "list is a NULL pointer");
	assert(list->head && "list is empty");
	
	struct node_t* top = list->head;
	struct node_t* next = NULL;

	while (top != NULL)
	{
		next = top->next;
		free(top);
		top = next;
	}

	free(list);
}

void free_hash(struct hash_t* hash)
{
	assert(hash && "hash is a NULL pointer");
	assert(hash->arr && "hash-table is empty");

	struct hash_node_t* cur = NULL;
	struct hash_node_t* next = NULL;

	for (long i = 0; i < hash->len; ++i)
	{
		if (hash->arr[i] == NULL)
			continue;

		cur = hash->arr[i];

		while (cur != NULL)
		{
			next = cur->next;
			free(cur);
			cur = next;
		}
	}

	free(hash->arr);
	free(hash);
}

long get_hash(const long page)
{
	return page & (HASH_SIZE - 1);
}

struct hash_node_t* create_hash_node(const long page)
{
	struct hash_node_t* hash_node = calloc(1, sizeof(struct hash_node_t));
	if (hash_node == NULL)
	{
		fprintf(stderr, "Memory exhausted\n");
		abort();
	}

	hash_node->node = calloc(1, sizeof(struct node_t));
	if (hash_node->node == NULL)
	{
		fprintf(stderr, "Memory exhausted\n");
		abort();
	}

	hash_node->next = NULL;
	hash_node->prev = NULL;

	hash_node->node->data = page;
	hash_node->node->next = NULL;
	hash_node->node->prev = NULL;

	return hash_node;
} 

void oust_head(struct hash_t* hash, struct list_t* list)
{
	assert(hash && "hash is a NULL pointer");
	assert(hash->arr && "hash is empty");
	assert(list && "list is a NULL pointer");
	assert(list->head && "list is empty");
	
	long key = list->head->data;
	long index = get_hash(key);
	struct hash_node_t* cur_hash_node = hash->arr[index];

	while(cur_hash_node->node->data != key)
	{
		cur_hash_node = cur_hash_node->next;
	}

	list->head = cur_hash_node->node->next;
	list->head->prev = NULL;
	free(cur_hash_node->node);

	if (cur_hash_node->prev == NULL)
	{
		hash->arr[index] = cur_hash_node->next;

		if (cur_hash_node->next != NULL)
			cur_hash_node->next->prev = NULL;
			
		free(cur_hash_node);
		return;
	}

	if (cur_hash_node->next == NULL)
	{
		cur_hash_node->prev->next = NULL;
		free(cur_hash_node);
		return;
	}

	cur_hash_node->next->prev = cur_hash_node->prev;
	cur_hash_node->prev->next = cur_hash_node->next;
	free(cur_hash_node);
	return;
}

void cache_miss(struct hash_t* hash, struct list_t* list, const long index, const long page)
{	
	assert(hash && "hash is a NULL pointer");
	assert(hash->arr && "hash is empty");
	assert(list && "list is a NULL pointer");

	struct hash_node_t* cur_hash_node = NULL;
	struct node_t* node = NULL;
	struct hash_node_t* hash_node = NULL;

	cur_hash_node = hash->arr[index];

	if (cur_hash_node == NULL)
	{
		cur_hash_node = create_hash_node(page);
		hash->arr[index] = cur_hash_node;
		node = cur_hash_node->node;

		if ((list->head == NULL) && (list->tail == NULL))
		{
			list->head = node;
			list->tail = node;   
			list->pages_cached += 1;
			hash->nodes_hashed += 1;
			return;            
		}

		if(list->pages_cached == list->size)
			oust_head(hash, list);
		else
		{
			list->pages_cached += 1;            
			hash->nodes_hashed += 1;
		}

		list->tail->next = node;
		node->prev = list->tail;
		list->tail = node;
		return;
	}

	hash_node = create_hash_node(page);
	hash->arr[index] = hash_node;
	cur_hash_node->prev = hash_node;
	hash_node->next = cur_hash_node;
	node = hash_node->node;

	if(list->pages_cached == list->size)
		oust_head(hash, list);
	else
	{
		list->pages_cached += 1;    
		hash->nodes_hashed += 1;
	}

	list->tail->next = node;
	node->prev = list->tail;
	list->tail = node;
	return;
}

void cache_hit(struct list_t* list, struct node_t* node)
{
	assert(list && "list is a NULL pointer");
	assert(list->head && "list is empty");
	assert(node && "node is a NULL pointer");

	if (node == list->tail)
		return;

	if (node == list->head)
	{
		list->head = node->next;
		list->head->prev = NULL;
		list->tail->next = node;
		node->prev = list->tail;
		list->tail = node;
		node->next = NULL;
		return;
	}

	node->prev->next = node->next;
	node->next->prev = node->prev;
	node->next = NULL;
	node->prev = list->tail;
	list->tail->next = node;
	list->tail = node;
	return;        
}

long lru(struct hash_t* hash, struct list_t* list, const long page)
{
	assert(hash && "hash is NULL pointer");
	assert(hash->arr && "hash is empty");
	assert(list && "list is NULL pointer");

	long index = get_hash(page);
	struct hash_node_t* cur_hash_node = hash->arr[index];

	if (cur_hash_node == NULL)
	{
		cache_miss(hash, list, index, page);
		return 0;
	}

	while(cur_hash_node != NULL)
	{
		if (cur_hash_node->node->data == page)
		{
			cache_hit(list, cur_hash_node->node);
			return 1;
		}

		cur_hash_node = cur_hash_node->next;
	}

	cache_miss(hash, list, index, page);
	return 0;

}
