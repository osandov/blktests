/*
 * Copyright (C) 2017 Omar Sandoval
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/loop.h>

#ifndef LOOP_SET_BLOCK_SIZE
#define LOOP_SET_BLOCK_SIZE 0x4C09
#endif

int main(int argc, char **argv)
{
	unsigned long long blksize;
	char *end;
	int fd = -1;

	if (argc != 3) {
		fprintf(stderr, "usage: %s DEV BLKSIZE\n", argv[0]);
		return 1;
	}

	errno = 0;
	blksize = strtoull(argv[2], &end, 0);
	if (errno || *end) {
		fprintf(stderr, "invalid block size\n");
		return 1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd == -1) {
		perror("open");
		return 1;
	}

	if (ioctl(fd, LOOP_SET_BLOCK_SIZE, blksize) == -1) {
		int status = errno == EINVAL ? 2 : 1;

		perror("LOOP_SET_BLOCK_SIZE");
		close(fd);
		return status;
	}

	close(fd);
	return 0;
}
