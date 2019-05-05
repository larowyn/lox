//
// Created by Paul Motte on 2019-05-04.
//

#ifndef PTB_STRING_H
#define PTB_STRING_H

#include "ptb_types.h"

bool32	isDigit(char c);
bool32	isAlpha(char c);
int32	strCompare(char const *str1, char const *str2);
bool32	strEqual(char const *str1, char const *str2);
void	*memCopy(void *dest, void const *source, uint32 len);

#endif // PTB_STRING_H
