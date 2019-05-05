//
// Created by Paul Motte on 2019-05-05.
//

#include <stdio.h>
#include <stdlib.h>

#include "ptb_arena.h"
#include "ptb_types.h"

INTERNAL Arena	*_arena_create(uint32 size) {
	Arena *arena = (Arena *) calloc(1, sizeof(Arena));

	if(!arena) {
		return NULL;
	}

	arena->region = (byte *) calloc(size, sizeof(byte));
	arena->size = size;

	if(!arena->region) {
		free(arena);
		return NULL;
	}

	return arena;
}

Arena	*initArena() {
	return _arena_create(PAGE_SIZE);
}

Arena	*initArena(uint32 size) {
	return _arena_create(size > PAGE_SIZE ? size : PAGE_SIZE);
}

void	*alloc(Arena *arena, uint32 size) {
	Arena *last = arena;

	do {
		if((arena->size - arena->current) > size){
			arena->current += size;

			return arena->region + (arena->current - size);
		}

		last = arena;
	} while ((arena = arena->next) != NULL);

	Arena *next = _arena_create(size > PAGE_SIZE ? size : PAGE_SIZE);

	last->next = next;
	next->current += size;

	return next->region;
}

void	freeArena(Arena *arena) {
	Arena *next, *last = arena;

	do {
		next = last->next;
		free(last->region);
		free(last);
		last = next;
	} while (next != NULL);
}
