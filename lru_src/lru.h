/*
********************************************************************************
*                                                                              
*                            Header file for LRU functions            
*                                                                             
********************************************************************************
*/

#pragma once

struct node_t; // node of double linked list
struct list_t; // double linked list
struct hash_node_t; // hash node with overflow list
struct hash_t; // hash with array implementation

// creates list of size size
struct list_t* create_list(const long size);
//creates hash
struct hash_t* create_hash();
// prints list 
void print_list(const struct list_t* list);
//frees list
void free_list(struct list_t* list);
//frees hash
void free_hash(struct hash_t* hash);
//gets hash
long get_hash(const long page);
// creates hash node and node of list_t with data == page
struct hash_node_t* create_hash_node(const long page);
//oust head from list and hash
void oust_head(struct hash_t* hash, struct list_t* list);
//implementation of cache miss
void cache_miss(struct hash_t* hash, struct list_t* list, const long index, const long page);
//implementation of cache hit
void cache_hit(struct list_t* list, struct node_t* node);
//main algorithm
long lru(struct hash_t* hash, struct list_t* list, const long page);