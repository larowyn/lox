//
// Created by Paul Motte on 2019-05-04.
//

#include "ptb_standard.h"
#include "ptb_types.h"

bool32			isDigit(char c) {
	if (c >= '0' && c <= '9') {
		return 1;
	}

	return 0;
}

bool32			isAlpha(char c) {
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
		return 1;
	}

	return 0;
}

int32			strCompare(char const *str1, char const *str2) {
	while (*str1 && *str1 == *str2) {
		str1++;
		str2++;
	}

	return (*(unsigned char*)str1 - *(unsigned char*)str2);
}

bool32			strEqual(char const *str1, char const *str2) {
	if (str1 && str2) {
		if (strCompare(str1, str2) == 0)
			return 1;
	}

	return 0;
}

void			*memCopy(void *dest, void const *source, uint32 len) {
	byte		*dst;
	byte const	*src;

	dst = (byte *)(dest);
	src = (byte *)(source);

	if (len == 0 || dst == src) {
		return (dest);
	}

	while (len) {
		*dst++ = *src++;
		len--;
	}

	return (dest);
}
