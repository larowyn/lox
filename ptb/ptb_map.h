//
// Created by Paul Motte on 2019-05-08.
//

#ifndef PTB_MAP_H
#define PTB_MAP_H

#include "ptb_types.h"
#include "ptb_substring.h"
#include "ptb_array.h"
#include "ptb_arena.h"

// @improvements: Base that on system page size to maximize perf
#define INITIAL_MAP_SIZE 256
#define LINEAR_PROBING_CHAIN_SIZE 8

struct				MapValue {
	union {
		char		*key;
		SubString	*keySubstring;
	};

	void			*data;
};

// If there is no collision MapContent will simply be a MapValue but if there is
// MapContent will become an Array or a List depending on useListForValues
struct				MapContent {
	union {
		MapValue	value;

		union {
			Array	*values;
			// List	*valuesList; // @todo: Allow use of a linked list if the table need fast suppression
		};
	};

	bool			isUsed;
};

struct				Map {
	MapContent		*table;
	Arena			*dataArena;
	uint32			size;
	uint32			length;
	uint32			dataSize;

	bool			disableLinearProbing;
	bool			useArenaToStoreData;
	bool			useSubstringForKey;
	// bool			useListForValues; // @todo
};

uint32				hashString(char *str);
uint32				hashSubString(SubString *substr);

Map					*initMap(uint32 dataSize);
Map					*initMap(uint32 dataSize, bool useArenaToStoreData);
Map					*initMap(uint32 dataSize, bool useArenaToStoreData, bool useSubstringForKey);

void				freeMap(Map *map);

void				*put(Map *map, char *key, void *value);
void				*put(Map *map, SubString *key, void *value);

void				*get(Map *map, char *key);
void				*get(Map *map, SubString *key);

bool				keyExist(Map *map, char *key);
bool				keyExist(Map *map, SubString *key);

#endif // PTB_MAP_H
