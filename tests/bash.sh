#!/bin/bash

RED='\033[0;31m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'
BLUE='\033[0;34m'

echo -e "${BLUE}Testing has started!${NC}"
echo 
echo -e "${GREEN}Recency friendly access pattern tests:${NC}"
for file in tests/recency_friendly_access_pattern/*.in; do
    printf "%s\n" $file
    printf "RRIP cache hits: "
    ./rrip < $file
    printf "LRU cache hits: "
    ./lru < $file
    printf "\n" 
done

echo -e "${GREEN}Thrashing access pattern tests:${NC}"
for file in tests/thrashing_access_pattern/*.in; do
    printf "%s\n" $file
    printf "RRIP cache hits: "
    ./rrip < $file
    printf "LRU cache hits: "
    ./lru < $file
    printf "\n" 
done

echo -e "${GREEN}Mixed access pattern(1) tests:${NC}"
for file in tests/mixed_access1_pattern/*.in; do
    printf "%s\n" $file
    printf "RRIP cache hits: "
    ./rrip < $file
    printf "LRU cache hits: "
    ./lru < $file
    printf "\n" 
done

echo -e "${GREEN}Mixed access pattern(2) tests:${NC}"
for file in tests/mixed_access2_pattern/*.in; do
    printf "%s\n" $file
    printf "RRIP cache hits: "
    ./rrip < $file
    printf "LRU cache hits: "
    ./lru < $file
    printf "\n" 
done

echo -e "${GREEN}Random pattern tests:${NC}"
for file in tests/random_tests/*.in; do
    printf "%s\n" $file
    printf "RRIP cache hits: "
    ./rrip < $file
    printf "LRU cache hits: "
    ./lru < $file
    printf "\n" 
done