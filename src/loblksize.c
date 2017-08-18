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

/* LO_FLAGS_BLOCKSIZE is an enum so we can't ifdef. */
#define LO_FLAGS_BLOCKSIZE_ 32

#ifndef LO_INFO_BLOCKSIZE
#define LO_INFO_BLOCKSIZE(l) (l)->lo_init[0]
#endif

int main(int argc, char **argv)
{
	struct loop_info64 lo;
	unsigned long long blksize;
	char *end;
	int fd = -1;
	bool set = argc >= 3;
	int status = EXIT_FAILURE;

	if (argc != 2 && argc != 3) {
		fprintf(stderr, "usage: %s DEV [BLKSIZE]\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (set) {
		errno = 0;
		blksize = strtoull(argv[2], &end, 0);
		if (errno || *end) {
			fprintf(stderr, "invalid block size\n");
			return EXIT_FAILURE;
		}
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("open");
		return EXIT_FAILURE;
	}

	if (ioctl(fd, LOOP_GET_STATUS64, &lo) == -1) {
		perror("LOOP_GET_STATUS64");
		goto out;
	}

	if (!(lo.lo_flags & LO_FLAGS_BLOCKSIZE_)) {
		fprintf(stderr, "LO_FLAGS_BLOCKSIZE not supported");
		goto out;
	}

	if (set) {
		lo.lo_flags |= LO_FLAGS_BLOCKSIZE_;
		LO_INFO_BLOCKSIZE(&lo) = blksize;
		if (ioctl(fd, LOOP_SET_STATUS64, &lo) == -1) {
			perror("LOOP_SET_STATUS64");
			goto out;
		}
	} else {
		printf("%llu\n", LO_INFO_BLOCKSIZE(&lo));
	}

	status = EXIT_SUCCESS;
out:
	close(fd);
	return status;
}
