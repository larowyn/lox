//
// Created by Paul Motte on 2019-05-05.
//

#ifndef PTB_STACK_H
#define PTB_STACK_H

#include "ptb_types.h"
#include "ptb_arena.h"

 // @improvements: Base that on system page size to maximize perf
#define INITIAL_SIZE 64

struct		Stack {
	byte	*content;
	uint32	size;
	uint32	length;
	uint32	granularity;
	byte	firstAccessed; // Handle the first call to getNext
};

Stack		*initStack(uint32 granularity);
void		freeStack(Stack *stack);
void		*getStart(Stack *stack);
void		*getNext(Stack *stack);

#endif // PTB_STACK_H
