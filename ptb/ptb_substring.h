//
// Created by Paul Motte on 2019-05-05.
//

#ifndef PTB_SUBSTRING_H
#define PTB_SUBSTRING_H

#include "ptb_types.h"

struct		Substring {
	char	*start;
	int32	length;
};

void		printSubstring(Substring *substring);
int32		substrCompare(char const *str1, Substring *substring);
int32		substrCompare(Substring *sub1, Substring *sub2);
bool32		substrEqual(char const *str1, Substring *str2);
bool32		substrEqual(Substring *str1, Substring *str2);

#endif // PTB_SUBSTRING_H
