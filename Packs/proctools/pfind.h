#ifndef _PFIND_H_
#define _PFIND_H_

#include <regex.h>
#include "proctools.h"

typedef enum pfind_operation {
	PFIND_NULL, PFIND_PATTERN,

	PFIND_PRI_ANY, PFIND_PRI_ARGS, PFIND_PRI_ASK, PFIND_PRI_EUID,
	PFIND_PRI_GID, PFIND_PRI_NAME, PFIND_PRI_PGRP, PFIND_PRI_PPID,
	PFIND_PRI_TERM, PFIND_PRI_UID,

	PFIND_ACT_DUMP, PFIND_ACT_ID, PFIND_ACT_KILL, PFIND_ACT_PRINT,
	PFIND_ACT_PS, PFIND_ACT_RESULT, PFIND_ACT_TIMEOUT, PFIND_ACT_WAIT,

	PFIND_OP_OPAREN, PFIND_OP_CPAREN, PFIND_OP_AND, PFIND_OP_OR,
	PFIND_OP_NOT, PFIND_OP_IF, PFIND_OP_DO, PFIND_OP_ELSE,
	PFIND_OP_ENDIF,
} _pfind_operation;

struct _pfind_dumpinfo {
	char *dumpfile;
	int fd;
};

struct _pfind_regex {
	regex_t r;
	char *pattern;
};

struct _pfind_keymap {
	char *keyword;
	_pfind_operation op;
};

typedef struct _pfind_keymap *keymap;

struct _pfind_optarg {
	int i;
	char *str;
	struct _pfind_regex re;
	struct _pfind_dumpinfo dump;
	struct uidlist *uidlist;
	struct pidlist *pidlist;
	struct termlist *termlist;
	struct grouplist *grouplist;
};

struct _pfind_expression {
	_pfind_operation op;
	int invert_after; /* parent initialises this */
	struct _pfind_optarg optarg;
	int (*eval)(struct _pfind_expression **, const struct proctoolslist *, struct baton *baton, int *displayed);
	struct _pfind_expression *previous; /* parent initialises this */
	struct _pfind_expression *next;
	struct _pfind_expression *parent; /* parent initialises this */
	struct _pfind_expression *children;
	int depth; /* parent initialises this */
	int result;
};

typedef struct _pfind_expression *expression_t;

#endif /* _PFIND_H_ */

