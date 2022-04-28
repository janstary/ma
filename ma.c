#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <err.h>
#include "ma.h"

void
usage()
{
	warnx("usage: ma [-v] rules.ma");
}

void
printrule(struct rule* rule)
{
	if (rule) {
		printf("%s\t%s\n", rule->l, rule->r);
	}
}

struct rule*
mkrule(char* line)
{
	char *l, *r;
	struct rule* rule = NULL;
	if ((rule = calloc(1, sizeof(struct rule))) == NULL)
		err(1, NULL);
	l = strsep(&line, " \t\n");
	while (isspace(*line))
		line++;
	r = strsep(&line, " \t\n");
	if (*l == '\0' || *r == '\0') {
		free(rule);
		return NULL;
	}
	rule->l = strdup(l);
	rule->r = strdup(r);
	return rule;
}

void
printma(struct ma* ma)
{
	struct rule* r;
	if (ma) {
		for (r = ma->r; r; r = r->n)
			printrule(r);
	}
}

int
addrule(struct ma* ma, char* line)
{
	struct rule* r;
	if (ma == NULL)
		return -1;
	if ((r = mkrule(line)) == NULL)
		return -1;
	if (ma->r == NULL) {
		ma->r = r;
		ma->l = r;
	} else {
		ma->l->n = r;
		ma->l = r;
	}
	return 0;
}

void
freema(struct ma* ma)
{
	/* FIXME */
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
		if (len == 1)
			continue;
		/*printf("%s", line);*/
		if (addrule(ma, line) == -1) {
			warnx("Error adding rule for %s", line);
			goto bad;
		}
	}
	fclose(f);
	free(line);
	return ma;
bad:
	fclose(f);
	freema(ma);
	return NULL;
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

	printma(ma);
	freema(ma);
	return 0;
}
