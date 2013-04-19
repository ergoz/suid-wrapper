#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	char str[1024];
	int i;

	if (argc < 2) {
		printf("usage: %s <command>\n", argv[0]);
		return -EINVAL;
	}

	for (i = 1; i < argc; i++) {
		if (i == 1)
			strcpy(str, argv[i]);
		else
			strcat(str, argv[i]);

		if (i < argc-1)
			strcat(str, " ");
	}

	if (setuid(0) != 0) {
		perror("Can't change user to root");
		return -EPERM;
	}
	if (setgid(0) != 0) {
		perror("Can't change group to root");
		return -EPERM;
	}
	return system(str);
}

