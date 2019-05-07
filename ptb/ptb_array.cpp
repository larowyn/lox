//
// Created by Paul Motte on 2019-05-05.
//

#include <stdio.h>
#include <stdlib.h>

#include "ptb_array.h"
#include "ptb_types.h"
#include "ptb_standard.h"

Array			*initArray(uint32 granularity) {
	Array *array = (Array *) calloc(1, sizeof(Array));

	if(!array) {
		return NULL;
	}

	array->length = 0;
	array->size = INITIAL_SIZE;
	array->granularity = granularity;
	array->content = (byte *)calloc(INITIAL_SIZE, granularity);

	if(!array->content) {
		free(array);
		return NULL;
	}

	return array;
}

void			freeArray(Array *array) {
	free(array->content);
	free(array);
}

// @todo @bug: Handle size * granularity > UINT32_MAX
INTERNAL Array	*resize(Array *array) {
	// @improvements: Make the growing of the array looks like a bell curve
	uint32		size = array->size < UINT32_MAX / 2 ? array->size * 2 : UINT32_MAX - 1;
	void		*previousContent = array->content;

	ASSERT((uint64)(UINT32_MAX) > (uint64)(size * array->granularity)) // @todo @bug

	array->content = (byte *)calloc(size, array->granularity);

	if(!array->content) {
		free(previousContent);
		free(array);

		return NULL;
	}

	memCopy(array->content, previousContent, (array->size * array->granularity) / sizeof(byte));

	free(previousContent);

	array->size = size;

	return array;
}

void			*getStart(Array *array) {
	return array->content;
}

void			*getNext(Array *array) {
	if (array->length == array->size && !resize(array)) {
		return NULL;
	}

	array->length++;
	return array->content + ((array->length - 1) * array->granularity);
}
