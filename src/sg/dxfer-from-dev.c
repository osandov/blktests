#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <scsi/sg.h>

int main(int argc, char **argv)
{
	int fd;
	int rc;
	int rsz = 131072;
	int tout = 10800000;
	char buf[42] = { 0 };

	if (argc != 2) {
		printf("usage: %s /dev/sgX\n", argv[0]);
		return 1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	rc = ioctl(fd, SG_SET_RESERVED_SIZE, &rsz);
	if (rc < 0) {
		perror("ioctl SG_SET_RESERVED_SIZE");
		goto out_close;
	}

	rc = ioctl(fd, SG_SET_TIMEOUT, &tout);
	if (rc < 0) {
		perror("ioctl SG_SET_TIMEOUT");
		goto out_close;
	}

	buf[4] = 'H';
	rc = write(fd, &buf, sizeof(buf));
	if (rc < 0) {
		perror("write");
		if (errno == EINVAL)
			printf("FAIL\n");
		goto out_close;
	}

	printf("PASS\n");

out_close:
	close(fd);
}
