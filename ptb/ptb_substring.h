//
// Created by Paul Motte on 2019-05-05.
//

#ifndef PTB_SUBSTRING_H
#define PTB_SUBSTRING_H

#include "ptb_types.h"

struct		SubString {
	char	*start;
	int32	length;
};

void		printSubstring(SubString *substr);

int32		substrCompare(char const *str1, SubString *substr);
int32		substrCompare(SubString *sub1, SubString *sub2);

bool32		substrEqual(char const *str1, SubString *str2);
bool32		substrEqual(SubString *str1, SubString *str2);

#endif // PTB_SUBSTRING_H
