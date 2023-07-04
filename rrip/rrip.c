#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

const long HASH_SIZE = 131072;

struct node_t
{
	struct node_t* next;
	struct node_t* prev;
	long data;
	unsigned rrpv:2;
};

struct list_t
{
	struct node_t* head;
	struct node_t* tail;
        struct node_t* fst_dst;
	long pages_cached;
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
	long nodes_hashed;
};

struct list_t* create_list(const long size)
{
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
	cur->rrpv = 3;

	list->head = cur;
	list->fst_dst = cur;
	list->size = size;
	list->pages_cached = 0;
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
		next->rrpv = 3;
		cur = next;
	}

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
		printf("%ld(%u) ", cur->data, cur->rrpv);
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

	hash_node->node->next = NULL;
	hash_node->node->prev = NULL;
	hash_node->node->data = page;
	hash_node->node->rrpv = 3;

	return hash_node;
} 

void cache_miss(struct hash_t* hash, struct list_t* list, const long index, const long page)
{	
	assert(hash && "hash is a NULL pointer");
	assert(hash->arr && "hash is empty");
	assert(list && "list is a NULL pointer");

	struct hash_node_t* cur_hash_node = hash->arr[index];
	struct node_t* cur = list->head;
	struct node_t* next = cur->next;
	struct hash_node_t* hash_node = NULL;

	if (cur_hash_node == NULL)
	{
		cur_hash_node = create_hash_node(page);
		hash->arr[index] = cur_hash_node;
	}

	while (list->fst_dst == NULL)
	{
		while (next != NULL)
		{
			++(cur->rrpv);
			if (cur->rrpv == 3 && list->fst_dst == NULL)
				list->fst_dst = cur;
			cur = next;
			next = next->next;
		}
	}

	// TO DO: replacement
	return;
}

long rrip(struct hash_t* hash, struct list_t* list, const long page)
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
			cur_hash_node->node->rrpv = 0;
			return 1;
		}

		cur_hash_node = cur_hash_node->next;
	}

	cache_miss(hash, list, index, page);
	return 0;
}
