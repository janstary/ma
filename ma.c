#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include "ma.h"

void
usage()
{
	warnx("usage: ma rules.ma");
}

struct ma*
parse(const char* file)
{
	FILE* f;
	ssize_t len;
	size_t size = 0;
	char* line = NULL;
	struct ma* ma = NULL;
	if ((ma = calloc(1, sizeof(struct ma))) == NULL)
		err(1, NULL);
	if ((f = fopen(file, "r")) == NULL)
		err(1, "%s", file);
	while ((len = getline(&line, &size, f)) != -1) {
		printf("%s", line);
	}
	free(line);
	fclose(f);
	return ma;
}

int
main(int argc, char** argv)
{
	int c;
	struct ma* ma;

	while ((c = getopt(argc, argv, "")) != -1) switch (c) {
		default:
			usage();
			return 1;
	}
	argc -= optind;
	argv += optind;

	if (argc != 1) {
		usage();
		return 1;
	}

	if ((ma = parse(*argv)) == NULL)
		return 1;
	return 0;
}
