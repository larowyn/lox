//
// Created by Paul Motte on 2019-05-05.
//

#include <stdio.h>
#include <stdlib.h>

#include "ptb_array.h"
#include "ptb_types.h"
#include "ptb_standard.h"

Array			*initArray(uint32 dataSize) {
	Array *array = (Array *) calloc(1, sizeof(Array));

	if(!array) {
		return NULL;
	}

	array->length = 0;
	array->size = INITIAL_ARRAY_SIZE;
	array->dataSize = dataSize;
	array->data = (byte *)calloc(INITIAL_ARRAY_SIZE, dataSize);

	if(!array->data) {
		free(array);
		return NULL;
	}

	return array;
}

void			freeArray(Array *array) {
	free(array->data);
	free(array);
}

// @todo @bug: Handle size * dataSize > UINT32_MAX
INTERNAL Array	*resize(Array *array) {
	// @improvements: Make the growing of the array looks like a bell curve
	uint32		size = array->size < UINT32_MAX / 2 ? array->size * 2 : UINT32_MAX - 1;
	void		*previousContent = array->data;

	ASSERT((uint64)(UINT32_MAX) > (uint64)(size * array->dataSize)) // @todo @bug

	array->data = (byte *)calloc(size, array->dataSize);

	if(!array->data) {
		free(previousContent);
		free(array);

		return NULL;
	}

	memCopy(array->data, previousContent, (array->size * array->dataSize) / sizeof(byte));

	free(previousContent);

	array->size = size;

	return array;
}

void			*getStart(Array *array) {
	return array->data;
}

void			*getNext(Array *array) {
	if (array->length == array->size && !resize(array)) {
		return NULL;
	}

	array->length++;
	return array->data + ((array->length - 1) * array->dataSize);
}
