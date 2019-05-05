//
// Created by Paul Motte on 2019-05-04.
//

#include <sys/stat.h>
#include <cstdlib>
#include <sys/file.h>
#include <unistd.h>

#include "ptb_io.h"
#include "ptb_types.h"

// It will allocate the memory needed et return -1 on error, you are
// responsible for freeing the memory returned but note that buffer won't
// be allocated if there was an error
int		DEBUG_readEntireFile(char *path, char **buffer) {
	struct stat info = {};
	stat(path, &info);

	ASSERT(info.st_size > 0)
	ASSERT(info.st_size < 0xFFFFFFFF - 1)

	*buffer = (char *)malloc((size_t)info.st_size + 1);
	if (!*buffer) return -1;

	int32 fd = open(path, O_RDONLY);
	if (fd == -1) {
		free(buffer);
		return -1;
	}

	ssize_t size = 0;
	size = read(fd, *buffer, (size_t)info.st_size); // @todo @speed: figure out the optimal read size
	*(*buffer + info.st_size) = '\0';

	close(fd);

	if (size == -1) {
		free(buffer);
		return -1;
	}

	return (int32)size;
}

void	DEBUG_freeEntireFile(char *fileMemory) {
	free((void *)fileMemory);
}

