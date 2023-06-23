#include "lru.h"
#include <stdio.h>
#include <stdlib.h>

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