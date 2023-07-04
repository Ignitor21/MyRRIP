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
void cache_miss(struct hash_t* hash, struct list_t* list, const long index, const long page);
long rrip(struct hash_t* hash, struct list_t* list, const long page);