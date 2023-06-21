#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

long DIVISOR = 100003;

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
	long pages_cached;
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

struct list_t* create_list()
{
	struct list_t* list = calloc(1, sizeof(struct list_t));
	assert(list != NULL && "Heap overflow");

	list->head = NULL;
	list->tail = NULL;
	list->pages_cached = 0;

	return list;
}

struct hash_t* create_hash()
{
	struct hash_t* hash = calloc(1, sizeof(struct hash_t));
	assert(hash != NULL && "Heap overflow");

	hash->arr = calloc(DIVISOR, sizeof(struct hash_node_t*));
	assert(hash->arr != NULL && "Heap overflow!");

	hash->len = DIVISOR;
	hash->nodes_hashed = 0;

	return hash;
}

void print_list(const struct list_t* list)
{
	struct node_t* cur = list->head;

	if (list == NULL)
	{
		printf("List is a NULL pointer\n");
		return;
	}

	if (list->head == NULL)
	{
		printf("Empty list\n");
		return;
	}


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
	struct node_t* top = NULL;
	struct node_t* next = NULL;
	top = list->head;

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
	struct hash_node_t* cur = NULL;
	struct hash_node_t* next = NULL;

	for (long i = 0; i < DIVISOR; ++i)
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
	return page % DIVISOR;
}

struct hash_node_t* create_hash_node(const long page)
{
	struct hash_node_t* hash_node = calloc(1, sizeof(struct hash_node_t));
	assert(hash_node != NULL);
	hash_node->node = calloc(1, sizeof(struct node_t));
	assert(hash_node->node != NULL);

	hash_node->next = NULL;
	hash_node->prev = NULL;

	hash_node->node->data = page;
	hash_node->node->next = NULL;
	hash_node->node->prev = NULL;

	return hash_node;
} 

void oust_head(struct hash_t* hash, struct list_t* list)
{
	long key = list->head->data;
	long index = get_hash(key);
	struct hash_node_t* cur_hash_node = hash->arr[index];

	assert(hash->arr != NULL && list != NULL);

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

void cache_miss(struct hash_t* hash, struct list_t* list, const long index, const long page, const long len)
{
	struct hash_node_t* cur_hash_node = NULL;
	struct node_t* node = NULL;
	struct hash_node_t* hash_node = NULL;

	assert(hash->arr != NULL && list != NULL);

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

		if(list->pages_cached == len)
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

	if(list->pages_cached == len)
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
	assert(list != NULL && node != NULL);

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

	if (node == list->tail)
		return;

	node->prev->next = node->next;
	node->next->prev = node->prev;
	node->next = NULL;
	node->prev = list->tail;
	list->tail->next = node;
	list->tail = node;
	return;        
}

void rehash(struct hash_node_t** hash_list)
{


}

long lru(struct hash_t* hash, struct list_t* list, const long page, const long len)
{
	struct hash_node_t* cur_hash_node = NULL;
	long index = 0;

	assert(hash != NULL && list != NULL);

	index = get_hash(page);
	cur_hash_node = hash->arr[index];

	if (cur_hash_node == NULL)
	{
		cache_miss(hash, list, index, page, len);
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

	cache_miss(hash, list, index, page, len);
	return 0;

}

int main()
{
	long len = 0, n = 0, page = 0, answer = 0;
	struct list_t* list = create_list();
	struct hash_t* hash = create_hash();

	scanf("%ld %ld", &len, &n);

	if (len == 1)
	{
		printf("0");
		return 0;
	}

	for (long i = 0; i < n; ++i)
	{
		scanf("%ld", &page);
		answer += lru(hash, list, page, len);
		//print_list(list);
	}

	printf("%ld\n", answer);
	free_list(list);
	free_hash(hash);

	return 0;
}
