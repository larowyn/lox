//
// Created by Paul Motte on 2019-05-05.
//

#ifndef PTB_ARRAY_H
#define PTB_ARRAY_H

#include "ptb_types.h"

// @improvements: Base that on system page size to maximize perf
#define INITIAL_SIZE 64

struct		Array {
	byte	*content;
	uint32	size;
	uint32	length;
	uint32	granularity;
};

Array		*initArray(uint32 granularity);
void		freeArray(Array *array);
void		*getStart(Array *array);
void		*getNext(Array *array);

#endif // PTB_ARRAY_H
