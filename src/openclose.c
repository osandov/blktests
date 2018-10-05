// SPDX-License-Identifier: GPL-3.0+
// Copyright (C) 2017 Omar Sandoval

/* Open and close a file repeatedly. */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
	int i, n;

	if (argc != 3) {
		fprintf(stderr, "usage: %s PATH REPEAT\n", argv[0]);
		return EXIT_FAILURE;
	}

	n = atoi(argv[2]);

	for (i = 0; i < n; i++) {
		int fd;

		fd = open(argv[1], O_RDWR);
		if (fd == -1) {
			perror("open");
			return EXIT_FAILURE;
		}

		if (close(fd) == -1) {
			perror("open");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
