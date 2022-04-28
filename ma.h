struct rule {
	char*		left;
	char*		right;
	size_t		llen;
	size_t		rlen;
	int		stop;
	struct rule*	next;
};

struct ma {
	struct rule*	rules;
	struct rule*	last;
};
