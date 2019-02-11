// SPDX-License-Identifier: GPL-3.0+
// Copyright (C) 2018 Western Digital Corporation or its affiliates.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#ifdef HAVE_LINUX_BLKZONED_H
#include <linux/blkzoned.h>
#endif
#include <linux/types.h>

#ifndef BLKGETZONESZ
#define BLKGETZONESZ	_IOR(0x12, 132, __u32)
#endif
#ifndef BLKGETNRZONES
#define BLKGETNRZONES	_IOR(0x12, 133, __u32)
#endif

struct request {
	const char *name;
	unsigned long code;
} requests[] = {
	{ "-s", BLKGETZONESZ},
	{ "-n", BLKGETNRZONES},
	{ NULL, 0},
};

void usage(const char *progname)
{
	int i = 0;

	fprintf(stderr, "usage: %s <request> <device file>\n", progname);
	fprintf(stderr, "<request> can be:\n");
	while (requests[i].name) {
		fprintf(stderr, "\t%s\n", requests[i].name);
		i++;
	}
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	int i = 0, fd, ret;
	unsigned int val;
	unsigned long code = 0;

	if (argc != 3)
		usage(argv[0]);

	while (requests[i].name) {
		if (strcmp(argv[1], requests[i].name) == 0) {
			code = requests[i].code;
			break;
		}
		i++;
	}
	if (code == 0)
		usage(argv[0]);

	fd = open(argv[2], O_RDWR);
	if (fd < 0) {
		perror("open");
		return EXIT_FAILURE;
	}

	ret = ioctl(fd, code, &val);
	if (ret < 0) {
		perror("ioctl");
		close(fd);
		return EXIT_FAILURE;
	}

	printf("%u\n", val);

	close(fd);

	return EXIT_SUCCESS;
}
