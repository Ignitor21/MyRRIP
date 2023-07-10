#pragma once

struct node_t;
struct list_t;
struct hash_node_t;
struct hash_t;

struct list_t* create_list(const long size);
struct hash_t* create_hash();
void print_list(const struct list_t* list);
void free_list(struct list_t* list);
void free_hash(struct hash_t* hash);
long get_hash(const long page);
struct hash_node_t* create_hash_node(const long page);
void delete_hash_node(struct hash_t* hash, const long key);
void list_replacement(struct list_t* list, struct hash_node_t* hash_node);
void cache_miss(struct hash_t* hash, struct list_t* list, const long index, const long page);
void cache_hit(struct list_t* list, struct node_t* node);
long rrip(struct hash_t* hash, struct list_t* list, const long page);