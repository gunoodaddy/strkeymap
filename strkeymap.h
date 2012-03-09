#ifndef __STRKEY_MAP_H__
#define __STRKEY_MAP_H__

#include <string.h>
#include <stdlib.h>
#include <search.h>

struct strkeymap_iterator;
struct strkey_value_internal;

typedef struct strkeymap {
	/* private */
	void * root;
	struct strkeymap_iterator *head;
	struct strkeymap_iterator *tail;
}strkeymap;


typedef struct strkeymap_iterator {
	char result; 	// 1 = succ, 0 = false
	char *first;	// key
	void **second;	// value's pointer

	/* private */
	strkeymap *map;
	struct strkeymap_iterator *next;
}strkeymap_iterator;


typedef struct strkey_value_internal {
	strkeymap *map;
	char *key;
	void *value;
}strkey_value_internal;


#define STRKEYMAP_CHECK_WHICH(which) \
	if(which == preorder || which == endorder) \
		return;

#define STRKV_FROM_NODE(node)	\
	((strkey_value_internal *)*(strkey_value_internal **)node)

#define TRACE(...)
#define DEBUG printf

inline void strkeymap_iterator_free(strkeymap *map);


//////////////////////////////////////////////////////////////////////////////////////////////
inline int _compareCallback(const void *l, const void *r) {
	const strkey_value_internal *ll = (strkey_value_internal *)l;
	const strkey_value_internal *lr = (strkey_value_internal *)r;

	TRACE("_compareCallback : %s, %s\n", ll->key, lr->key);

	return strcmp(ll->key, lr->key);
}  

inline int _deleteCompareCallback(const void *l, const void *r) {
	strkey_value_internal *ll = (strkey_value_internal *)l;
	strkey_value_internal *lr = (strkey_value_internal *)r;

	int ret = strcmp(ll->key, lr->key);
	if(0 == ret) {
		DEBUG("_deleteCompareCallback : %s, %p\n", lr->key, lr->value);
		free(lr->key);
		free(lr);
	}
	return ret;
}  

inline void _freeNodeCallback(void *nodep) {
	strkey_value_internal *kv = (strkey_value_internal *)nodep;
	DEBUG("_freeNodeCallback : %p, %p, %s, %p\n", nodep, kv, kv->key, kv->value);
	free(kv->key);
	free(kv);
}

inline void _iterationCallback(const void *nodep, const VISIT which, const int depth) {
	STRKEYMAP_CHECK_WHICH(which);

	strkey_value_internal *kv = STRKV_FROM_NODE(nodep);
	DEBUG("_iterationCallback : %s, %p : (%p, %d, %d)\n", kv->key, kv->value, nodep, which, depth);

	strkeymap_iterator *it = (strkeymap_iterator *)malloc(sizeof(strkeymap_iterator));
	it->result = 1;
	it->map = kv->map;
	it->first = kv->key;
	it->second = &kv->value;
	it->next = NULL;

	if(!it->map->head) {
		it->map->head = it;
		it->map->tail = it;
	} else {
		it->map->tail->next = it;
		it->map->tail = it;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////


inline strkeymap *strkeymap_new() {
	strkeymap *map = (strkeymap *)malloc(sizeof(strkeymap));
	map->root = NULL;
	map->head = map->tail = NULL;
	return map;
}

inline void strkeymap_free(strkeymap *map) {
	strkeymap_iterator_free(map);
	tdestroy(map->root, _freeNodeCallback);
	free(map);
}

inline int strkeymap_insert(strkeymap *map, const char *key, void *value) {
	strkey_value_internal _find;
	_find.key = (char *)key;
	void *ret = tfind(&_find, &map->root, _compareCallback); /* read */ 

	if(NULL == ret) {
		strkey_value_internal *item = (strkey_value_internal *)malloc(sizeof(strkey_value_internal));
		item->map = map;
		item->key = strdup(key);
		item->value = value;

		void *val = tsearch(item, &map->root, _compareCallback); /* insert */  
		if(!val) {
			return -1;
		}
		TRACE("insert : (%p) %s => %p, %p\n", map->root, item->key, value, val);
	} else {
		strkey_value_internal *kv = STRKV_FROM_NODE(ret);
		DEBUG("failed to insert, already key exist.. : %s\n", kv->key);
	}

	return 0;
}

inline strkeymap_iterator strkeymap_find(strkeymap *map, const char *key) {
	strkey_value_internal _find;
	_find.key = (char *)key;

	void *ret = tfind(&_find, &map->root, _compareCallback); // read 
	if(!ret) {
		static strkeymap_iterator it;
		it.result = 0;
		return it;
	}

	strkey_value_internal *kv = STRKV_FROM_NODE(ret);

	strkeymap_iterator it;
	it.result = 1;
	it.map = kv->map;
	it.first = kv->key;
	it.second = &kv->value;
	it.next = NULL;
	return it;
}

inline void strkeymap_erase(strkeymap *map, const char *key) {
	strkey_value_internal _find;
	_find.key = (char *)key;

	tdelete(&_find, &map->root, _deleteCompareCallback);
}

inline const strkeymap_iterator* strkeymap_iterator_new(strkeymap *map) {
	map->head = NULL;
	map->tail = NULL;
	twalk(map->root, _iterationCallback);

	strkeymap_iterator *_next = map->head;
	while(_next) {
		TRACE("NODE(%p:%s) - ", _next, _next->first);
		_next = _next->next;
	}
	TRACE("\n");
	return map->head;
}

inline strkeymap_iterator* strkeymap_iterator_next(const strkeymap_iterator *it) {
	if(it)
		return it->next;
	return NULL;
}

inline void strkeymap_iterator_free(strkeymap *map) {
	strkeymap_iterator *_next = map->head;
	map->head = map->tail = NULL;
	while(_next) {
		strkeymap_iterator *_prev = _next;
		_next = _next->next;
		free(_prev);
	}
}

#endif
