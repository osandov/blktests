// SPDX-License-Identifier: GPL-3.0+
// Copyright (C) 2018 Josef Bacik

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/nbd.h>

int main(int argc, char **argv)
{
	unsigned long long size;
	char *end;
	int fd = -1;

	if (argc != 3) {
		fprintf(stderr, "usage: %s DEV SIZE\n", argv[0]);
		return 1;
	}

	errno = 0;
	size = strtoull(argv[2], &end, 0);
	if (errno || *end) {
		fprintf(stderr, "invalid size\n");
		return 1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd == -1) {
		perror("open");
		return 1;
	}

	if (ioctl(fd, NBD_SET_SIZE, size) == -1) {
		int status = errno == EINVAL ? 2 : 1;

		perror("NBD_SET_SIZE");
		close(fd);
		return status;
	}

	close(fd);
	return 0;
}
