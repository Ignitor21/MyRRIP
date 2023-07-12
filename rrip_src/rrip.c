/*
********************************************************************************
*                                                                              
*        File with implementation of utility functions for RRIP policy            
*                  (Re-rederence interval prediction)  
*                                                           
********************************************************************************
*/

#include "rrip.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

const long HASH_SIZE = 131072;
const unsigned DISTANT_RRPV = 3;

struct node_t
{
	struct node_t* next;
	struct node_t* prev;
	long data;
	unsigned rrpv:2; //re-reference prediction value
};

struct list_t
{
	struct node_t* head;
	struct node_t* tail;
        struct node_t* fst_dst; // pointer to the first element with distant rrpv
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
	long size;
};

struct list_t* create_list(const long size) 
{
	assert(size > 0 && "Size is negative");

	struct list_t* list = calloc(1, sizeof(struct list_t));
	struct node_t* cur = NULL;
	struct node_t* next = NULL;

	if (list == NULL)
	{
		fprintf(stderr, "Memory exhausted\n");
		abort();
	}

	cur = calloc(1, sizeof(struct node_t));

	if (cur == NULL)
	{
		fprintf(stderr, "Memory exhausted\n");
		abort();
	}
	
	cur->data = -1;
	cur->next = NULL;
	cur->prev = NULL;
	cur->rrpv = DISTANT_RRPV;

	list->head = cur;
	list->fst_dst = cur;
	list->size = size;

	for (int i = 0; i < size - 1; ++i)
	{
		next = calloc(1, sizeof(struct node_t));
		if (cur == NULL)
		{
			fprintf(stderr, "Memory exhausted\n");
			abort();
		}

		cur->next = next;
		next->prev = cur;
		next->next = NULL;
		next->data = -1;
		next->rrpv = DISTANT_RRPV;
		cur = next;
	}

	list->tail = cur;
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

	hash->size = HASH_SIZE;

	return hash;
}

void print_list(const struct list_t* list) 
{
	assert(list && "list is a NULL pointer");
	assert(list->head && "list is empty");

	struct node_t* cur = list->head;

	while (cur != NULL && cur->data != -1)
	{
		printf("%3ld(%u) ", cur->data, cur->rrpv);
		cur = cur->next;
	}

	if (list->fst_dst != NULL && list->fst_dst->data != -1)
		printf("first distant: %ld", list->fst_dst->data);
	else
		printf("first distant: NULL");

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

	for (long i = 0; i < hash->size; ++i)
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
	return page & (HASH_SIZE - 1); // bit operation instead of %
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

	hash_node->node->next = NULL;
	hash_node->node->prev = NULL;
	hash_node->node->data = page;
	hash_node->node->rrpv = DISTANT_RRPV;

	return hash_node;
} 

void delete_hash_node(struct hash_t* hash, const long key) 
{
	assert(hash && "hash is a NULL pointer");
	assert(hash->arr && "hash is empty");
	assert(key > 0 && "key is negative");

	long key_index = get_hash(key);
	struct hash_node_t* cur_hash_node = hash->arr[key_index];

	while(cur_hash_node->node->data != key)
	{
		if (cur_hash_node == NULL)
		{
			fprintf(stderr, "No page %ld in hash\n", key);
			return;
		}
		cur_hash_node = cur_hash_node->next;
	}

	if(cur_hash_node->prev == NULL)
	{
		hash->arr[key_index] = cur_hash_node->next;

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

void list_replacement(struct list_t* list, struct hash_node_t* hash_node)
{
	assert(list && "list is a NULL pointer");
	assert(list->head && "list is empty");
	assert(hash_node && "hash node is a NULL pointer");

	struct node_t* node = hash_node->node;
	struct node_t* cur = NULL;

	if (list->fst_dst == list->head)
	{
		node->next = list->head->next;
		node->prev = NULL;

		if (list->head->next != NULL)
			list->head->next->prev = node;
		
		list->head = node;
	}

	else if (list->fst_dst == list->tail)
	{
		node->prev = list->tail->prev;
		node->next = NULL;
		list->fst_dst->prev->next = node;
		list->tail = node;
	}

	else
	{
		node->next = list->fst_dst->next;
		node->prev = list->fst_dst->prev;
		list->fst_dst->next->prev = node;
		list->fst_dst->prev->next = node;
	}

	node->rrpv = 2;
	free(list->fst_dst);
	cur = node;

	while (cur != NULL && cur->rrpv != DISTANT_RRPV)
		cur = cur->next;
		
	list->fst_dst = cur;

	return;
}

void cache_miss(struct hash_t* hash, struct list_t* list, const long index, const long page)
{	
	assert(hash && "hash is a NULL pointer");
	assert(hash->arr && "hash is empty");
	assert(list && "list is a NULL pointer");
	assert(list->head && "list is empty");

	struct hash_node_t* new_hash_node = NULL;
	struct hash_node_t* hash_node = hash->arr[index];
	struct node_t* cur = NULL;

	new_hash_node = create_hash_node(page);
	hash->arr[index] = new_hash_node;
	new_hash_node->next = hash_node;

	if (hash_node != NULL)
		hash_node->prev = new_hash_node;

	while (list->fst_dst == NULL)
	{
		cur = list->head;
		while (cur != NULL)
		{
			++(cur->rrpv);
			if (cur->rrpv == DISTANT_RRPV && list->fst_dst == NULL)
				list->fst_dst = cur;
			cur = cur->next;
		}
	}

	if (list->fst_dst->data > 0)
		delete_hash_node(hash, list->fst_dst->data);

	list_replacement(list, new_hash_node);
	return;
}

void cache_hit(struct list_t* list, struct node_t* node)
{
	assert(list && "list is a NULL pointer");
	assert(list->head && "list is empty");
	assert(node && "node is a NULL pointer");

	node->rrpv = 0;

	if (list->fst_dst == node)
	{
		struct node_t* cur = node;

		while (cur != NULL && cur->rrpv != DISTANT_RRPV)
			cur = cur->next;
		
		list->fst_dst = cur;
	}

	return;
}

long rrip(struct hash_t* hash, struct list_t* list, const long page)
{
	assert(hash && "hash is a NULL pointer");
	assert(hash->arr && "hash is empty");
	assert(list && "list is a NULL pointer");

	long index = get_hash(page);
	struct hash_node_t* cur_hash_node = hash->arr[index];

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
