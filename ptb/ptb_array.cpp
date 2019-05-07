//
// Created by Paul Motte on 2019-05-05.
//

#include <stdio.h>
#include <stdlib.h>

#include "ptb_array.h"
#include "ptb_types.h"
#include "ptb_standard.h"

Array			*initArray(uint32 granularity) {
	Array *stack = (Array *) calloc(1, sizeof(Array));

	if(!stack) {
		return NULL;
	}

	stack->length = 0;
	stack->size = INITIAL_SIZE;
	stack->granularity = granularity;
	stack->content = (byte *)calloc(INITIAL_SIZE, granularity);
	stack->firstAccessed = 0;

	if(!stack->content) {
		free(stack);
		return NULL;
	}

	return stack;
}

void			freeArray(Array *stack) {
	free(stack->content);
	free(stack);
}

// @todo @bug: Handle size * granularity > UINT32_MAX
INTERNAL Array	*resize(Array *stack) {
	// @improvements: Make the growing of the stack looks like a bell curve
	uint32		size = stack->size < UINT32_MAX / 2 ? stack->size * 2 : UINT32_MAX - 1;
	void		*previousContent = stack->content;

	ASSERT((uint64)(UINT32_MAX) > (uint64)(size * stack->granularity)) // @todo @bug

	stack->content = (byte *)calloc(size, stack->granularity);

	if(!stack->content) {
		free(previousContent);
		free(stack);

		return NULL;
	}

	memCopy(stack->content, previousContent, (stack->size * stack->granularity) / sizeof(byte));

	free(previousContent);

	stack->size = size;

	return stack;
}

void			*getStart(Array *stack) {
	return stack->content;
}

void			*getNext(Array *stack) {
	if (stack->length == stack->size && !resize(stack)) {
		return NULL;
	}

	if (!stack->firstAccessed) { // Handle the first call to getNext
		stack->firstAccessed = 1;
		return getStart(stack);
	}

	stack->length += 1;

	return stack->content + (stack->length * stack->granularity);
}
