#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

void stracat(char **target, char *source)
{
	char *tmp;

	tmp = realloc(*target, (strlen(*target)+strlen(source)+1));
	if (tmp == NULL) {
		perror("Failure allocating memory for command");
		exit(ENOMEM);
	}
	strcat(tmp, source);
	*target = tmp;
}

int main(int argc, char *argv[])
{
	char *str;
	int i, result = 0;

	if (argc < 2) {
		printf("usage: %s <command>\n", argv[0]);
		return -EINVAL;
	}

	str = malloc(strlen(argv[1])+1);
	if (str == NULL) {
		perror("Failure allocating memory for command");
		return -ENOMEM;
	}
	strcpy(str, argv[1]);

	for (i = 2; i < argc; i++) {
		stracat(&str, " ");
		stracat(&str, argv[i]);
	}

	if (setuid(0) != 0) {
		perror("Can't change user to root");
		result = -EPERM;
	}

	if (result == 0) {
		if (setgid(0) != 0) {
			perror("Can't change group to root");
			result = -EPERM;
		}
	}

	if (result == 0)
		result = system(str);
	free(str);
	return result;
}

