#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/loop.h>

void usage(const char *progname)
{
	fprintf(stderr, "usage: %s LOOPDEV PATH\n", progname);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	int ret;
	int fd, filefd;

	if (argc != 3)
		usage(argv[0]);

	fd = open(argv[1], O_RDWR);
	if (fd == -1) {
		perror("open");
		return EXIT_FAILURE;
	}

	filefd = open(argv[2], O_RDWR);
	if (filefd == -1) {
		perror("open");
		return EXIT_FAILURE;
	}

	ret = ioctl(fd, LOOP_CHANGE_FD, filefd);
	if (ret == -1) {
		perror("ioctl");
		close(fd);
		close(filefd);
		return EXIT_FAILURE;
	}
	close(fd);
	close(filefd);
	return EXIT_SUCCESS;
}
