#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#ifndef FALLBACK_USER_ALLOWED
/* Defines whether the user has access to use this
 * tool or not when the configuration file is not
 * present or readable.
 * 0 means allowed, non-zero means not-allowed
 */
#define FALLBACK_USER_ALLOWED 0
#endif

#ifndef CONFIG_FILE
#define CONFIG_FILE "/etc/suidusers"
#endif

#ifndef ROOT_UID
#define ROOT_UID 0
#endif

#ifndef ROOT_GID
#define ROOT_GID 0
#endif

int user_allowed(char *filename, const char *username) {
	FILE *fp;
	char buf[100], *p;
	size_t readsize;

	if (getuid() == ROOT_UID)
		return 0;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		return FALLBACK_USER_ALLOWED;
	}

	while (!feof(fp)) {
		readsize = fread(buf, sizeof(char), 100, fp);
		if (readsize > 1) {
			p = strtok(buf, " \t\n\r,;");
			while (p != NULL) {
				if (strcmp(p, username) == 0) {
					fclose(fp);
					return 0;
				}
				p = strtok(NULL, " \t\n\r,;");
			}
		}
	}

	fclose(fp);
	return -1;
}

int stracat(char **target, char *source)
{
	char *tmp;
	if (target == NULL) {
		errno = -EINVAL;
		return -1;
	}

	if (*target == NULL)
		tmp = calloc(strlen(source) + 1, sizeof(char));
	else
		tmp = realloc(*target, ((strlen(*target)+strlen(source) + 1) * sizeof(char)));

	if (tmp == NULL) {
		perror("Failure allocating memory for command");
		errno = -ENOMEM;
		return -1;
	}

	strcat(tmp, source);
	*target = tmp;
	return 0;
}

int main(int argc, char *argv[])
{
	char *str = NULL;
	int i, result = 0;
	uid_t uid;
	struct passwd *pw;

	if (argc < 2) {
		printf("usage: %s <command>\n", argv[0]);
		return -EINVAL;
	}

	for (i = 1; i < argc; i++) {
		if (stracat(&str, argv[i]) != 0) {
			perror("stracat failed");
			result = errno;
			break;
		}
		if (stracat(&str, " ") != 0) {
			perror("stracat failed");
			result = errno;
			break;
		}
	}

	if (result == 0) {
		uid = getuid();
		pw = getpwuid(uid);
		if (user_allowed(CONFIG_FILE, pw->pw_name) != 0) {
			printf("User not allowed to execute '%s' as root\n", str);
			result = -EPERM;
		}
	}

	if (result == 0) {
		if (setuid(ROOT_UID) != 0) {
			perror("Can't change user to root");
			result = -EPERM;
		}
	}

	if (result == 0) {
		if (setgid(ROOT_GID) != 0) {
			perror("Can't change group to root");
			result = -EPERM;
		}
	}

	if (result == 0)
		result = system(str);

	free(str);

	return result;
}

