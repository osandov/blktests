/*
 * Open and close a file repeatedly.
 *
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
