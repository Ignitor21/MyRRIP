#pragma once

struct node_t;
struct list_t;
struct hash_node_t;
struct hash_t;

struct list_t* create_list();
struct hash_t* create_hash();
void print_list(const struct list_t* list);
void free_list(struct list_t* list);
void free_hash(struct hash_t* hash);
long get_hash(const long page);
struct hash_node_t* create_hash_node(const long page);
void oust_head(struct hash_t* hash, struct list_t* list);
void cache_miss(struct hash_t* hash, struct list_t* list, const long index, const long page, const long len);
void cache_hit(struct list_t* list, struct node_t* node);
long lru(struct hash_t* hash, struct list_t* list, const long page, const long len);