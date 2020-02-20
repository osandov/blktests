// SPDX-License-Identifier: GPL-3.0+
// Copyright (C) 2019 Sun Ke

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/fs.h>
#include <linux/nbd.h>

int main(int argc, char **argv)
{
	const char *mountpoint, *dev, *fstype;
	int loops, fd, i;

	if (argc != 5) {
		fprintf(stderr, "usage: %s DEV MOUNTPOINT FSTYPE LOOPS", argv[0]);
		return EXIT_FAILURE;
	}

	dev = argv[1];
	mountpoint = argv[2];
	fstype = argv[3];
	loops = atoi(argv[4]);

	fd = open(dev, O_RDWR);
	if (fd == -1) {
		perror("open");
		return EXIT_FAILURE;
	}

	for (i = 0; i < loops; i++) {
		pid_t mount_pid, clear_sock_pid;
		int wstatus;

		mount_pid = fork();
		if (mount_pid == -1) {
			perror("fork");
			return EXIT_FAILURE;
		}
		if (mount_pid == 0) {
			mount(dev, mountpoint, fstype,
			      MS_NOSUID | MS_SYNCHRONOUS, 0);
			umount(mountpoint);
			exit(EXIT_SUCCESS);
		}

		clear_sock_pid = fork();
		if (clear_sock_pid == -1) {
			perror("fork");
			return EXIT_FAILURE;
		}
		if (clear_sock_pid == 0) {
			if (ioctl(fd, NBD_CLEAR_SOCK, 0) == -1) {
				perror("ioctl");
				exit(EXIT_FAILURE);
			}
			exit(EXIT_SUCCESS);
		}

		if (waitpid(mount_pid, &wstatus, 0) == -1) {
			perror("waitpid");
			return EXIT_FAILURE;
		}
		if (!WIFEXITED(wstatus) ||
		    WEXITSTATUS(wstatus) != EXIT_SUCCESS) {
			fprintf(stderr, "mount process failed");
			return EXIT_FAILURE;
		}

		if (waitpid(clear_sock_pid, &wstatus, 0) == -1) {
			perror("waitpid");
			return EXIT_FAILURE;
		}
		if (!WIFEXITED(wstatus) ||
		    WEXITSTATUS(wstatus) != EXIT_SUCCESS) {
			fprintf(stderr, "NBD_CLEAR_SOCK process failed");
			return EXIT_FAILURE;
		}
	}

	close(fd);
	return EXIT_SUCCESS;
}
