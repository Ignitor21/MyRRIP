/*
********************************************************************************
*                                                                              
*                                 Main RRIP file
*                                                          
********************************************************************************
*/

#include "rrip.h"
#include <stdio.h>

int main()
{
	long size = 0, n = 0, page = 0, answer = 0; 
	/*
	size - size of cache
	n - number of cache requests
	page - number of every page
	answer - final number of cache hits
	*/
	scanf("%ld %ld", &size, &n);

	struct hash_t* hash = create_hash();
	struct list_t* list = create_list(size);

	for (long i = 0; i < n; ++i)
	{
		scanf("%ld", &page);
		answer += rrip(hash, list, page);
	}

	printf("%ld\n", answer);

	free_list(list);
	free_hash(hash);

	return 0;
}