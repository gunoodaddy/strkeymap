#include <stdio.h>
#include "strkeymap.h"

int main() {
	strkeymap *map;
	map = strkeymap_new();

	strkeymap_insert(map, "1", (void *)1);
	strkeymap_insert(map, "1", (void *)11);
	strkeymap_insert(map, "2", (void *)2);
	strkeymap_insert(map, "3", (void *)3);
	strkeymap_insert(map, "4", (void *)4);
	strkeymap_insert(map, "5", (void *)5);
	strkeymap_insert(map, "6", (void *)6);
	strkeymap_insert(map, "7", (void *)7);
	strkeymap_iterator it = strkeymap_find(map, "7");
	printf("strkeymap_find : %s : %p\n", it.first, it.second);

	if(1) {
		const strkeymap_iterator* it = strkeymap_iterator_new(map);
		if(it) {
			do {
				printf("[ITER-%s:%p] => ", it->first, it->second);
				it = strkeymap_iterator_next(it);
			} while(it);
			printf("\n");
		}
		strkeymap_iterator_free(map);
	}

	if(1) {
		strkeymap_erase(map, "1");
		strkeymap_erase(map, "2");
		strkeymap_erase(map, "3");
		strkeymap_erase(map, "4");
	}

	strkeymap_free(map);

	printf("bye!\n");
	return 0;
}

