struct rule {
	char*		l;
	char*		r;
	struct rule*	n;
};

struct ma {
	struct rule*	r;
	uint32_t	n;
};
