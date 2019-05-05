//
// Created by Paul Motte on 2019-05-05.
//

#ifndef PTB_ARENA_H
#define PTB_ARENA_H

#include <stdint.h>
#include <memory.h>
#include "ptb_types.h"

#define PAGE_SIZE 4095

struct		Arena {
	byte	*region;
	uint32	size;
	uint32	current;
	Arena	*next;
};

Arena		*initArena();
Arena		*initArena(uint32 size);
void		*alloc(Arena *arena, uint32 size);
void		freeArena(Arena *arena);

#endif // PTB_ARENA_H
