//
// Created by Paul Motte on 2019-05-05.
//

#include "ptb_substring.h"

void		printSubstring(Substring *substring) {
	printf("%.*s", substring->length, substring->start);
}

int32		substrCompare(char const *str1, Substring *substring) {
	char	*str2 = substring->start;

	while (
		*str1
		&& str2 - substring->start < substring->length
		&& *str1 == *str2
	) {
		str1++;
		str2++;
	}

	return !*str1 ? 0 : (*(unsigned char*)str1 - *(unsigned char*)str2);
}

bool32		substrEqual(char const *str1, Substring *str2) {
	if (str1 && str2) {
		if (substrCompare(str1, str2) == 0)
			return 1;
	}

	return 0;
}

int32		substrCompare(Substring *sub1, Substring *sub2) {
	int32	i = 0;

	while (
		i < sub1->length
		&& i < sub2->length
		&& sub1->start[i]
		&& sub1->start[i] == sub2->start[i]
	) {
		i++;
	}

	return (
		i == sub1->length && i == sub2->length
			? 0
			: (*(unsigned char*)sub1->start[i - 1] - *(unsigned char*)sub2->start[i - 1])
	);
}

bool32		substrEqual(Substring *str1, Substring *str2) {
	if (str1 && str2) {
		if (substrCompare(str1, str2) == 0)
			return 1;
	}

	return 0;
}
