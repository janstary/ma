#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <err.h>
#include "ma.h"

int verbose = 0;

void
usage()
{
	warnx("usage: ma [-v] rules.ma");
}

void
frule(struct rule* rule)
{
	if (rule) {
		free(rule->left);
		free(rule->right);
		free(rule);
	}
}

struct rule*
mkrule(char* line)
{
	char *l, *r, *s = NULL;
	struct rule* rule = NULL;
	l = strsep(&line, " \t\n");
	if (*l == '\0') {
		warnx("empty left side");
		return NULL;
	}
	if (strchr(l, '.')) {
		warnx("dot on the left");
		return NULL;
	}
	if (strchr(l, '_')) {
		if (l[0] == '_' && l[1] == '\0') {
			/* blank */
			*l = '\0';
		} else {
			warnx("blank may only appear alone");
			return NULL;
		}
	}
	while (isspace(*line))
		line++;
	r = strsep(&line, " \t\n");
	if (*r == '\0') {
		warnx("empty right side");
		return NULL;
	}
	if ((s = strrchr(r, '.'))) {
		if (*(s+1) != '\0') {
			warnx("dot may only appear last");
			return NULL;
		}
		*s = '\0';
	}
	if (strchr(r, '_')) {
		if (r[0] == '_' && r[1] == '\0') {
			/* blank */
			*r = '\0';
		} else {
			warnx("blank may only appear alone");
			return NULL;
		}
	}
	if ((rule = calloc(1, sizeof(struct rule))) == NULL)
		err(1, NULL);
	rule->left  = strdup(l);
	rule->right = strdup(r);
	rule->llen  = strlen(l);
	rule->rlen  = strlen(r);
	rule->stop = (s != NULL);
	return rule;
}

void
prule(struct rule* rule)
{
	if (rule) {
		printf("%s\t", strlen(rule->left)  ? rule->left  : "_");
		printf("%s",   strlen(rule->right) ? rule->right : "_");
		if (rule->stop)
			putchar('.');
		putchar('\n');
	}
}

int
addrule(struct ma* ma, char* line)
{
	struct rule* rule;
	if (ma == NULL)
		return -1;
	if ((rule = mkrule(line)) == NULL)
		return -1;
	if (ma->rules == NULL) {
		ma->rules = rule;
		ma->last = rule;
	} else {
		ma->last->next = rule;
		ma->last = rule;
	}
	return 0;
}

void
printma(struct ma* ma)
{
	struct rule* rule;
	if (ma) {
		for (rule = ma->rules; rule; rule = rule->next)
			prule(rule);
	}
}

void
freema(struct ma* ma)
{
	struct rule* r;
	struct rule* n;
	if (ma) {
		r = ma->rules;
		while (r) {
			n = r->next;
			frule(r);
			r = n;
		}
		free(ma);
	}
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

char*
replace(char* expr, char* here, struct rule* rule)
{
	char* c;
	char* head = NULL;
	char* tail = NULL;
	char* copy = NULL;
	if (expr == NULL
	||  here == NULL
	||  rule == NULL)
		return NULL;
	head = copy = calloc(1, strlen(expr) + 1 + rule->rlen - rule->llen);
	for (c = expr; *c && c < here ; )
		*copy++ = *c++;
	tail = c + rule->llen;
	for (c = rule->right; *c ; )
		*copy++ = *c++;
	for (c = tail; *c ; )
		*copy++ = *c++;
	*copy = '\0';
	/*printf("%s\n", head);*/
	return head;
}

void
debug(char* expr, char* here, struct rule* rule)
{
	char* c;
	char* tail;
	if (expr == NULL)
		return;
	if (here && rule) {
		for (c = expr; *c && c < here ; )
			putchar(*c++);
		tail = c + rule->llen;
		if (isatty(1)) {
			printf(
				"\033[4m%s\033[24m",
				*rule->left ? rule->left : "_");
		} else {
			if (*rule->left) {
				for (c = rule->left; *c ; c++)
					/*printf("%c%c%c", *c, 0x08, *c);*/
					printf("%c%c%c", *c, 0x08, '_');
			} else {
				printf("%c%c%c", '_', 0x08, '_');
			}
		}
		for (c = tail; *c; )
			putchar(*c++);
		printf("\t%s", *rule->right ? rule->right : "_");
	} else {
		printf("%s", expr);
	}
	putchar('\n');
}

char*
rewrite(char* expr, struct ma* ma)
{
	struct rule* rule;
	char* here = NULL;
	char* prev = NULL;
	char* this = strdup(expr);
	if (expr == NULL || ma == NULL)
		return NULL;
	do {
		free(prev);
		prev = this;
		here = NULL;
		for (rule = ma->rules; rule; rule = rule->next) {
			if ((here = strstr(prev, rule->left))) {
				if (verbose > 1)
					debug(prev, here, rule);
				this = replace(prev, here, rule);
				if (verbose == 1)
					debug(this, NULL, NULL);
				break;
			}
		}
		if (rule && rule->stop) {
			free(prev);
			return this;
		}
	} while (here);
	return this;
}

int
main(int argc, char** argv)
{
	int c;
	ssize_t len;
	size_t size = 0;
	char* line = NULL;
	struct ma* ma;

	while ((c = getopt(argc, argv, "v")) != -1) switch (c) {
		case 'v':
			verbose++;
			break;
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
	/*printma(ma);*/

	while ((len = getline(&line, &size, stdin)) != -1) {
		line[--len] = '\0'; /* newline */
		printf("%s\n", rewrite(line, ma));
	}

	freema(ma);
	free(line);
	return 0;
}
