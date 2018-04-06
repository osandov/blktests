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
	fprintf(stderr, "usage: %s PATH [64]\n", progname);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	unsigned long cmd;
	int ret;
	int fd;

	if (argc != 2 && argc != 3)
		usage(argv[0]);

	if (argc == 3) {
		if (strcmp(argv[2], "64") == 0)
			cmd = LOOP_GET_STATUS64;
		else
			usage(argv[0]);
	} else {
		cmd = LOOP_GET_STATUS;
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("open");
		return EXIT_FAILURE;
	}

	ret = ioctl(fd, cmd, NULL);
	if (ret == -1) {
		if (errno == EINVAL) {
			printf("Got EINVAL\n");
		} else {
			perror("ioctl");
			close(fd);
			return EXIT_FAILURE;
		}
	} else {
		printf("Did not get EINVAL\n");
	}

	close(fd);
	return EXIT_SUCCESS;
}
