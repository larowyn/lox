//
// Created by Paul Motte on 2019-05-04.
//

#ifndef PTB_IO_H
#define PTB_IO_H

#include "ptb_types.h"

int32	DEBUG_readEntireFile(char *path, char **buffer);
void	DEBUG_freeEntireFile(char *fileMemory);

#endif // PTB_IO_H
