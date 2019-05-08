//
// Created by Paul Motte on 2019-05-08.
//

/*
 * Resources:
 *
 * - djb2 hash: https://github.com/petewarden/c_hashmap/blob/master/hashmap.c
 * - example of implementation: https://github.com/petewarden/c_hashmap/blob/master/hashmap.c
 */

/*
 * @Todo List:
 *
 * - Do testing and shit, for now its totally experimental
 *
 * - Split this map into specialized maps:
 * 		- Simple map of pointer to data with linearProbing
 * 		- Simple map of pointer to data with array/list
 * 		- Write a map generator that will generate map for a specific data type
 * 		- This actual map should probably become the PrototypingMap which could handle
 * 		  all types of map even if its not very efficient
 *
 * - Map should be an array where we do a dumb iteration search until it reaches a
 *   certain size, figure out a good size from which to do the switch and implement it
 */

#include <stdio.h>
#include <stdlib.h>

#include "ptb_map.h"
#include "ptb_types.h"
#include "ptb_standard.h"

// djb2 algorithm
// See: http://www.cse.yorku.ca/~oz/hash.html
uint32				hashString(char *str) {
	uint32			hash = 5381;
	int32			c;

	while ((c = (int32)*str++)) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	return hash;
}

uint32				hashSubString(SubString *substr) {
	uint32			hash = 5381;
	int32			c;

	for (int i = 0; i < substr->length; i++) {
		c = (int32)substr->start[i];

		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	return hash;
}

INTERNAL Map		*allocMap(uint32 dataSize, bool useArenaToStoreData, bool useSubstringForKey) {
	ASSERT(useArenaToStoreData ? true : dataSize <= sizeof(void *)) // @todo

	Map				*map = (Map *) calloc(1, sizeof(Map));

	if(!map) {
		return NULL;
	}

	map->length = 0;
	map->size = INITIAL_MAP_SIZE;
	map->dataSize = dataSize;
	map->table = (MapContent *)calloc(INITIAL_MAP_SIZE, sizeof(MapContent));

	if(!map->table) {
		free(map);
		return NULL;
	}

	map->disableLinearProbing = false; // @todo

	if (useArenaToStoreData) {
		map->useArenaToStoreData = useArenaToStoreData;

		ASSERT((uint64)(UINT32_MAX) > (uint64)(INITIAL_MAP_SIZE * dataSize)) // @todo @bug
		map->dataArena = initArena(INITIAL_MAP_SIZE * dataSize);
	}

	map->useSubstringForKey = useSubstringForKey;

	return map;
}

Map					*initMap(uint32 dataSize) {
	return allocMap(dataSize, false, false);
}

Map					*initMap(uint32 dataSize, bool useArenaToStoreData) {
	return allocMap(dataSize, useArenaToStoreData, false);
}

Map					*initMap(uint32 dataSize, bool useArenaToStoreData, bool useSubstringForKey) {
	return allocMap(dataSize, useArenaToStoreData, useSubstringForKey);
}

void				freeMap(Map *map) {
	if (map->useArenaToStoreData && map->dataArena) {
		freeArena(map->dataArena);
	}

	free(map->table);
	free(map);
}

INTERNAL uint32		indexFromKey(Map *map, void *key) {
	if (map->length == map->size / 2) { // @todo: figure out the best way to know when the map should resize
		// @todo: resize Map
		ASSERT(false)
	}

	// ----- Hashing -----
	uint32			hash;

	if (map->useSubstringForKey) {
		hash = hashSubString((SubString *)key);
	} else {
		hash = hashString((char *)key);
	}

	uint32			index = hash % map->size;

	// ----- Linear Probing -----
	if (!map->disableLinearProbing) {
		for (int offset = 0; offset < LINEAR_PROBING_CHAIN_SIZE; offset++) {
			MapContent current = map->table[index + offset];

			if (
				!current.isUsed
				|| (
					map->useSubstringForKey
						? substrEqual((SubString *)key, current.value.keySubstring)
						: strEqual((char *)key, current.value.key)
				)
			) {
				return index + offset;
			}
		}

		// @todo: resize Map and rehash the whole thing, then return indexFromKey
		return -1;
	} else {
		// @todo: handle Array/List push
	}

	return index;
}

INTERNAL void		*_put(Map *map, void *key, void *value) {
	int32			index = indexFromKey(map, key);
	MapContent		*content = &map->table[index];

	content->isUsed = true;

	if (map->useSubstringForKey) {
		content->value.keySubstring = (SubString *)key;
	} else {
		content->value.key = (char *)key;
	}

	if (map->useArenaToStoreData) {
		content->value.data = alloc(map->dataArena, map->dataSize);

		memCopy(content->value.data, value, map->dataSize);
	} else {
		content->value.data = value;
	}

	map->length++;

	return content->value.data;
}

void				*put(Map *map, char *key, void *value) {
	return _put(map, (void *)key, value);
}

void				*put(Map *map, SubString *key, void *value) {
	return _put(map, (void *)key, value);
}

INTERNAL void		*_get(Map *map, void *key) {
	int32			index = indexFromKey(map, key);
	MapContent		*content = &map->table[index];

	return content->isUsed ? content->value.data : NULL;
}

void				*get(Map *map, char *key) {
	return _get(map, (void *)key);
}

void				*get(Map *map, SubString *key) {
	return _get(map, (void *)key);
}

INTERNAL bool		_keyExist(Map *map, void *key) {
	int32			index = indexFromKey(map, key);
	MapContent		*content = &map->table[index];

	return content->isUsed;
}

bool				keyExist(Map *map, char *key) {
	return _keyExist(map, (void *)key);
}

bool				keyExist(Map *map, SubString *key) {
	return _keyExist(map, (void *)key);
}