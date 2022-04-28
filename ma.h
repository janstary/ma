struct rule {
	char*		l;
	char*		r;
	struct rule*	n;
};

struct ma {
	struct rule*	r;
	struct rule*	l;
};
