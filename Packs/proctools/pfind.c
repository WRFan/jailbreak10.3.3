/**
 * pfind.c
 * Copyright (c) 2003 James Devenish. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of William B Faulk nor the names of his contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/ptrace.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/user.h>
#include <sys/wait.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "./pfind.h"

static void usage();
static void nosig(char *);

extern char *__progname;
static char *delim;
static int iflag, xflag, firstflag;
static int alarmflag, escapeflag;
static struct itimerval alarmtimer;
static pid_t child;

__dead static void nosig(char *);
__dead static void usage();

__dead static void
usage()
{
	(void)fprintf(stderr, "Usage: %s [-V] [-S] [-d <delim>] [-f <file> | -M <core>] [-i] [-x] [<expression>]\n", __progname);
	exit(EX_USAGE);
}

#define PLAIN_EXPR() \
				expression->children = NULL; \
				expression->next = strategy->next = expression + 1; \
				expression->next->eval = NULL; \
				expression->next->invert_after = 0; \
				expression->next->previous = expression; \
				expression->next->parent = expression->parent; \
				expression->next->depth = expression->depth;

static struct _pfind_keymap keywords[] = {
	{"!", PFIND_OP_NOT},
	{"(", PFIND_OP_OPAREN},
	{")", PFIND_OP_CPAREN},
	{"-and", PFIND_OP_AND},
	{"-any", PFIND_PRI_ANY},
	{"-args", PFIND_PRI_ARGS},
	{"-ask", PFIND_PRI_ASK},
	{"-details", PFIND_ACT_PS},
	{"-do", PFIND_OP_DO},
	{"-dump", PFIND_ACT_DUMP},
	{"-else", PFIND_OP_ELSE},
	{"-endif", PFIND_OP_ENDIF},
	{"-euid", PFIND_PRI_EUID},
	{"-full", PFIND_PRI_ARGS},
	{"-gid", PFIND_PRI_GID},
	{"-group", PFIND_PRI_GID},
	{"-id", PFIND_ACT_ID},
	{"-if", PFIND_OP_IF},
	{"-kill", PFIND_ACT_KILL},
	{"-name", PFIND_PRI_NAME},
	{"-not", PFIND_OP_NOT},
	{"-or", PFIND_OP_OR},
	{"-parent", PFIND_PRI_PPID},
	{"-pgrp", PFIND_PRI_PGRP},
	{"-ppid", PFIND_PRI_PPID},
	{"-print", PFIND_ACT_PRINT},
	{"-ps", PFIND_ACT_PS},
	{"-result", PFIND_ACT_RESULT},
	{"-s", PFIND_ACT_KILL},
	{"-term", PFIND_PRI_TERM},
	{"-then", PFIND_ACT_KILL},
	{"-timeout", PFIND_ACT_TIMEOUT},
	{"-uid", PFIND_PRI_UID},
	{"-user", PFIND_PRI_EUID},
	{"-wait", PFIND_ACT_WAIT},
};

void
dump(expression_t strategy)
{
	printf("->op: %d\n", strategy->op);
	printf("->invert_after: %d\n", strategy->invert_after);
	/*printf("->optarg: %s\n", strategy->optarg);*/
	printf("->eval: %p\n", strategy->eval);
	printf("->previous: %p\n", strategy->previous);
	printf("->children: %p\n", strategy->children);
	printf("->next: %p\n", strategy->next);
	printf("->parent: %p\n", strategy->parent);
	printf("->depth: %d\n", strategy->depth);
	printf("->result: %d\n", strategy->result);
}

static void
signal_alarm(signal)
	int signal;
{
	alarmflag = signal;
	if (child)
		(void)kill(child, SIGSTOP);
}

static void
ptrace_cleanup(signal)
	int signal;
{
	escapeflag = signal;
	if (child)
		(void)kill(child, SIGSTOP);
	child = 0;
}

static void
atexit_cleanup(void)
{
	if (!firstflag)
		printf("\n");
}

#define PFIND_EVAL(X) \
	int \
	X(expression, proctoolslist, baton, displayed) \
		expression_t *expression; \
		const struct proctoolslist *proctoolslist; \
		struct baton *baton; \
		int *displayed;

PFIND_EVAL(pfind_act_id)
{
	(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 1;
	printf("%s%d", firstflag ? (firstflag = FALSE, "") : delim, proctoolslist->pid);
	(*displayed) = TRUE;
	return 0;
}

PFIND_EVAL(pfind_act_kill)
{
	if (proctoolslist->pid < 1)
		errx(EX_SOFTWARE, "invalid pid stored internally");
	if (kill(proctoolslist->pid, (*expression)->optarg.i)) {
		(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 0;
		warn("could not send signal %d to process ID %d", (*expression)->optarg.i, proctoolslist->pid);
	}
	else
		(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 1;
	return 0;
}

PFIND_EVAL(pfind_act_signalling_core)
{
	int result, status;
	pid_t p;

	if (proctoolslist->pid < 1)
		errx(EX_SOFTWARE, "invalid pid stored internally");


	//result = ptrace(PT_ATTACH, proctoolslist->pid, NULL, 0);
	result = ptrace(PT_ATTACHEXC, proctoolslist->pid, NULL, 0);

	if (result && errno) {
		(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 0;
		warn("could not attach to process %d", proctoolslist->pid);
		return 0;
	}

	status = 0;
	p = wait(&status);
	if (p != proctoolslist->pid || !WIFSTOPPED(status)) {
		(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 0;
		warnx("unexpected state 0x%x (0%o) from process %d", status, status, proctoolslist->pid);
	}
	else
	if (!alarmflag) {
		result = ptrace(PT_CONTINUE, proctoolslist->pid, (caddr_t)1, 0);
		if (result && errno) {
			(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 0;
			warn("error restarting process %d", proctoolslist->pid);
		}
		else {
			/*
			printf("result:%d, errno:%d, status: %d / %x\n", result, errno, status, status);
			printf("WIFEXITED:%d\n", WIFEXITED(status)); printf("WIFSIGNALED:%d\n", WIFSIGNALED(status)); printf("WIFSTOPPED:%d\n", WIFSTOPPED(status)); printf("WEXITSTATUS:%d\n", WEXITSTATUS(status)); printf("WTERMSIG:%d\n", WTERMSIG(status)); printf("WCOREDUMP:%d\n", WCOREDUMP(status)); printf("WSTOPSIG:%d\n", WSTOPSIG(status));
			*/
			do {
				status = 0;
				child = proctoolslist->pid; /* for signal_alarm */
				if (!alarmflag && waitpid(proctoolslist->pid, &status, 0) == -1) {
					child = 0;
					(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 0;
					warn("error waiting for process %d", proctoolslist->pid);
					break;
				}
				else
				if (escapeflag) {
					child = 0;
					(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 0;
					break;
				}
				else
				if (WIFEXITED(status)) {
					child = 0;
					(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 1;
					if ((*expression)->op == PFIND_ACT_RESULT) {
						printf("%s%d %d", firstflag ? (firstflag = FALSE, "") : delim, proctoolslist->pid, WEXITSTATUS(status));
						(*displayed) = TRUE;
					}
					return 0; /* don't try PT_DETACH */
				}
				else
				if (alarmflag) {
					child = 0;
					warnx("process %d timed out", proctoolslist->pid);
					(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 0;
					break;
				}
				else
				if (WIFSTOPPED(status)) {
					child = 0;
					warnx("process %d stopped: skipping", proctoolslist->pid);
					(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 0;
					break;
				}
				else {
					child = 0;
					warnx("unexpected state 0x%x (0%o) from process %d", status, status, proctoolslist->pid);
					(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 0;
					break;
				}
			}
			while (!status || !WIFEXITED(status));
		}
	}

	result = ptrace(PT_DETACH, proctoolslist->pid, 0, 0);
	if (result && errno) {
		warn("possible error detaching from process %d", proctoolslist->pid);
	}

	child = 0;

	return 0;
}

PFIND_EVAL(pfind_act_wait)
{
	sig_t hupsave, intsave, quitsave, termsave;
	int result;

	result = pfind_act_kill(expression, proctoolslist, baton, displayed);
	if (!(*expression)->result) {
		return result;
	}

	hupsave = signal(SIGHUP, ptrace_cleanup);
	intsave = signal(SIGINT, ptrace_cleanup);
	quitsave = signal(SIGQUIT, ptrace_cleanup);
	termsave = signal(SIGTERM, ptrace_cleanup);

	result = pfind_act_signalling_core(expression, proctoolslist, baton, displayed);
	if (escapeflag)
		exit(EX_UNAVAILABLE);

	(void)signal(SIGHUP, hupsave);
	(void)signal(SIGINT, intsave);
	(void)signal(SIGQUIT, quitsave);
	(void)signal(SIGTERM, termsave);

	return result;
}

PFIND_EVAL(pfind_act_timeout)
{
	int result, duration = (*expression)->optarg.i;
	sig_t hupsave, intsave, quitsave, termsave;

	if ((*expression)->op == PFIND_ACT_TIMEOUT) {
		/* send SIGSTOP to child after duration elapses */
		alarmtimer.it_value.tv_sec = duration;
		if (setitimer(ITIMER_REAL, &alarmtimer, NULL) == -1)
			err(EX_OSERR, "could not set up timer");
	}

	hupsave = signal(SIGHUP, ptrace_cleanup);
	intsave = signal(SIGINT, ptrace_cleanup);
	quitsave = signal(SIGQUIT, ptrace_cleanup);
	termsave = signal(SIGTERM, ptrace_cleanup);

	result = pfind_act_signalling_core(expression, proctoolslist, baton, displayed);
	alarmflag = 0;

	(void)signal(SIGHUP, hupsave);
	(void)signal(SIGINT, intsave);
	(void)signal(SIGQUIT, quitsave);
	(void)signal(SIGTERM, termsave);

	return result;
}

PFIND_EVAL(pfind_act_print)
{
	(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 1;
	printf("%s%d %s", firstflag ? (firstflag = FALSE, "") : delim, proctoolslist->pid, proctoolslist->name);
	(*displayed) = TRUE;
	return 0;
}

PFIND_EVAL(pfind_act_ps)
{
	(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 1;
	printf("%s%d ", firstflag ? (firstflag = FALSE, "") : delim, proctoolslist->pid);
	printProcInfo(baton, proctoolslist);
	(*displayed) = TRUE;
	return 0;
}

PFIND_EVAL(pfind_act_dump)
{
	int fd = (*expression)->optarg.dump.fd;
	if (fd < 0)
		errx(EX_SOFTWARE, "invalid fd stored internally");
	(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 1;
	if (dumpProc(baton, fd, proctoolslist) == -1)
		err(EX_OSERR, "could not dump");
	return 0;
}

PFIND_EVAL(pfind_op_oparen)
{
	(*expression)->result = 1; /* match by default */
	if ((*expression)->parent)
		(*expression)->parent->result = (*expression)->result;
	return 0;
}

PFIND_EVAL(pfind_op_and)
{
	/* this is redundant */
	return 0;
}

PFIND_EVAL(pfind_op_or)
{
	if ((*expression)->parent->result) {
		/* condition is satisfied */
		*expression = (*expression)->parent->next;
		return 1;
	}
	else {
		/* carry on */
		(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ 1;
		return 0;
	}
}

PFIND_EVAL(pfind_op_do)
{
	(*expression)->parent->result = 1;
	(*expression)->result = (*expression)->invert_after ^ (*expression)->previous->result;
	if ((*expression)->result) {
		/* carry on to children */
		return 0;
	}
	else {
		/* skip our children and jump to next */
		*expression = (*expression)->next;
		return 1;
	}
}

PFIND_EVAL(pfind_op_else)
{
	(*expression)->parent->result = (*expression)->result = 1;
	if ((*expression)->previous->result) {
		/* skip our children and jump to next */
		*expression = (*expression)->next;
		return 1;
	}
	else {
		/* carry on to children */
		return 0;
	}
}

PFIND_EVAL(pfind_op_cparen)
{
	(*expression)->result = (*expression)->invert_after ^ (*expression)->previous->invert_after ^ (*expression)->previous->result;
	if ((*expression)->parent)
		(*expression)->parent->result = (*expression)->result;
	return 0;
}

PFIND_EVAL(pfind_op_endif)
{
	(*expression)->parent->result = (*expression)->result = 1; /* match by default */
	return 0;
}

PFIND_EVAL(pfind_pri_any)
{
	int result;

	//result = !regexec(&(*expression)->optarg.re.r, proctoolslist->name, 0, NULL, NULL);
	result = !regexec(&(*expression)->optarg.re.r, proctoolslist->name, 0, NULL, 0);

	if (!result) {
		const char *args;
		args = getProcArgs(baton, proctoolslist);

		//result = args && !regexec(&(*expression)->optarg.re.r, args, 0, NULL, NULL);
		result = args && !regexec(&(*expression)->optarg.re.r, args, 0, NULL, 0);

		/* do not free(args) */
	}
	(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ result;
	return 0;
}

PFIND_EVAL(pfind_pri_args)
{
	const char *args;
	int result;
	args = getProcArgs(baton, proctoolslist);

	//result = args && !regexec(&(*expression)->optarg.re.r, args, 0, NULL, NULL);
	result = args && !regexec(&(*expression)->optarg.re.r, args, 0, NULL, 0);

	(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ result;
	/* do not free(args) */
	return 0;
}

PFIND_EVAL(pfind_pri_ask)
{
	char *c;
	size_t len;

	fflush(stdout);
	if (!firstflag) {
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "Proceed with process %d? [Y/n] ", proctoolslist->pid);
	fflush(stderr);

	c = fgetln(stdin, &len);

	if (c == NULL || len < 1) {
		if (ferror(stdin))
			err(EX_OSERR, "could not -ask");
		else
			errx(EX_OSERR, "input ended before -ask finished");
	}

	(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ (*c == 'Y' || *c == 'y' || *c == '\n'); /* match by default */
	return 0;
}

PFIND_EVAL(pfind_pri_name)
{
	if (xflag) {
		if (iflag)
			(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ !strcasecmp(proctoolslist->name, (*expression)->optarg.str);
		else
			(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ !strcmp(proctoolslist->name, (*expression)->optarg.str);
	}
	else
	{
		//(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ !regexec(&(*expression)->optarg.re.r, proctoolslist->name, 0, NULL, NULL);

		(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ !regexec(&(*expression)->optarg.re.r, proctoolslist->name, 0, NULL, 0);
	}
	return 0;
}

PFIND_EVAL(pfind_pri_pgrp)
{
	int result =
		(*expression)->optarg.pidlist != NULL &&
		matchPidList((*expression)->optarg.pidlist,
				proctoolslist->kp->kp_eproc.e_pgid);
	(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ result;
	return 0;
}

PFIND_EVAL(pfind_pri_gid)
{
	int result =
		(*expression)->optarg.grouplist != NULL &&
		matchGroupList((*expression)->optarg.grouplist,
				proctoolslist->kp->kp_eproc.e_ucred.cr_gid);
	(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ result;
	return 0;
}

PFIND_EVAL(pfind_pri_ppid)
{
	int result =
		(*expression)->optarg.pidlist != NULL &&
		matchPidList((*expression)->optarg.pidlist,
				proctoolslist->kp->kp_eproc.e_ppid);
	(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ result;
	return 0;
}

PFIND_EVAL(pfind_pri_term)
{
	int result =
		(*expression)->optarg.termlist != NULL &&
		matchTermList((*expression)->optarg.termlist,
				proctoolslist->kp->kp_eproc.e_tdev);
	(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ result;
	return 0;
}

PFIND_EVAL(pfind_pri_euid)
{
	int result =
		(*expression)->optarg.uidlist != NULL &&
		matchUidList((*expression)->optarg.uidlist,
				proctoolslist->kp->kp_eproc.e_pcred.p_svuid);
	(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ result;
	return 0;
}

PFIND_EVAL(pfind_pri_ruid)
{
	int result =
		(*expression)->optarg.uidlist != NULL &&
		matchUidList((*expression)->optarg.uidlist,
				proctoolslist->kp->kp_eproc.e_pcred.p_ruid);
	(*expression)->parent->result = (*expression)->result = (*expression)->invert_after ^ result;
	return 0;
}

expression_t
/* strategy contains space for at least two elements */
open_strategy(expression_t strategy)
{
	expression_t next = strategy + 1;
	/* open parenthesis */
	strategy->op = PFIND_OP_OPAREN;
	strategy->invert_after = 0;
	strategy->eval = &pfind_op_oparen;
	strategy->previous = NULL;
	strategy->children = next;
	strategy->next = next; /* because it's the end -- close_strategy will convert it from being a child */
	strategy->parent = NULL;
	strategy->depth = 0;

	next->invert_after = 0;
	next->eval = NULL;
	next->previous = NULL;
	next->parent = strategy;
	next->depth = 1;

	return strategy;
}

/* build needs to allocate pointer to 'next'*/
void
close_strategy(expression_t strategy)
{
	/* close parenthesis */
	expression_t expression = strategy->next; /* synonym for "the end" */
	if (expression->depth > 1) {
		expression_t other, extended = malloc((expression->depth - 1) * sizeof(struct _pfind_expression));

		if (extended == NULL)
			err(EX_OSERR, "could not fix missing brackets");
		warnx("inserting missing brackets");

		other = extended;

		while (expression->depth > 1) {
			expression->op = PFIND_OP_CPAREN;
			expression->eval = &pfind_op_cparen;
			expression->previous = expression->parent; /* replaced */
			expression->previous->next = expression;
			expression->next = /* strategy->next = */ other;
			expression->parent = expression->previous->parent; /* replaced */
			expression->children = NULL;
			expression->depth = expression->previous->depth; /* replaced */

			other->invert_after = 0;
			other->eval = NULL;
			other->previous = expression;
			other->parent = expression->parent;
			other->depth = expression->depth;

			expression = other;
			other++;
		}
	}
	expression->op = PFIND_OP_CPAREN;
	/*expression->invert_after;*/
	expression->eval = &pfind_op_cparen;
	expression->previous = expression->parent; /* replaced */
	expression->previous->next = expression;
	expression->next = NULL;
	expression->parent = expression->previous->parent; /* replaced */
	expression->children = NULL;
	expression->depth = expression->previous->depth; /* replaced */
}

int
keyword_comparison(a, b)
	const void *a, *b;
{
	return (strcmp(((struct _pfind_keymap *)a)->keyword, ((struct _pfind_keymap *)b)->keyword));
	/* some might suggest strcasecmp */
}

/*
 * pfind matches processes specified by its arguments and prints them out
 */
int
main(argc, argv)
	int argc;
	char *argv[];
{
	struct proctoolslist *proctoolslist, *temppl;
	char **arg, *infilename, *endptr, *corefile;
	int Sflag;
	expression_t strategy, expression, other;
	int count;
	struct _pfind_keymap *keymap;
	struct _pfind_keymap key;
	int ch, i;
	struct baton *baton;

	if (argc < 2)
	{
		(void)fprintf(stderr, "%s: no arguments supplied\n", __progname);
		usage();
	}
	else
	{
		count = argc;
		arg = argv;
	}

	proctoolslist = temppl = NULL;
	delim = infilename = endptr = corefile = NULL;
	iflag = Sflag = xflag = FALSE;
	strategy = expression = NULL;
	keymap = NULL;
	baton = NULL;

	(void)signal(SIGALRM, signal_alarm);

	while (++arg, --count) {
		if (strategy == NULL) {
			if (**arg == '-' && (ch = *(1+*arg)) != '\0') {
				if (*(2+*arg) == '\0') {
					/* single-letter option */
					switch (ch) {
						case 'd':
							if (count > 1 /* i.e. there are further arguments */) {
								if (delim != NULL)
									free(delim);
								if ((delim = strdup((--count, *++arg))) == NULL)
									err(EX_OSERR, NULL);
								if (Sflag)
									fprintf(stderr, "Delimiter is now '%s'.\n", delim);
								continue;
							}
							break;
						case 'f':
							if (count > 1 /* i.e. there are further arguments */) {
								if (corefile != NULL) {
									if (Sflag)
										fprintf(stderr, "Ignoring core file \"%s\".\n", corefile);
									free(corefile);
								}
								if (infilename != NULL)
									free(infilename);
								if ((infilename = strdup((--count, *++arg))) == NULL)
									err(EX_OSERR, NULL);
								if (Sflag)
									fprintf(stderr, "Input file is now \"%s\".\n", infilename);
								continue;
							}
							break;
						case 'i':
							iflag = TRUE;
							if (Sflag)
								fprintf(stderr, "Case-insensitive matching.\n");
							continue;
							/* NOTREACHED */
						case 'M':
							if (count > 1 /* i.e. there are further arguments */) {
								if (infilename != NULL) {
									if (Sflag)
										fprintf(stderr, "Ignoring input file \"%s\".\n", infilename);
									free(infilename);
								}
								if (corefile != NULL)
									free(corefile);
								if ((corefile = strdup((--count, *++arg))) == NULL)
									err(EX_OSERR, NULL);
								if (Sflag)
									fprintf(stderr, "Core file is now \"%s\".\n", corefile);
								continue;
							}
							break;
						case 'S':
							Sflag = TRUE;
							continue;
							/* NOTREACHED */
						case 'V':
							(void)fprintf(stderr, "%s (proctools " VERSION ") http://proctools.sourceforge.net\n", __progname);
							exit(EX_OK);
							/* NOTREACHED */
						case 'x':
							xflag = TRUE;
							continue;
							/* NOTREACHED */
						default:
							break;
					}
				}
				/* fall through */
			}
			/* fall through */
			if ((strategy = malloc((argc + 1) * sizeof(struct _pfind_expression))) == NULL)
				err(EX_OSERR, NULL);

			/* 'strategy' has sufficient capacity to accommodate a user who has
			 * given us argc number of keywords, plus capacity for two extra
			 * slots for the implicit 'brackets' that surround the command
			 * line.
			 */

			open_strategy(strategy);
		}

		if (strategy == NULL)
			err(EX_OSERR, NULL);

		expression = strategy->next; /* end */

		key.keyword = *arg;
		key.op = PFIND_PATTERN;

		keymap = (struct _pfind_keymap *)bsearch(&key, keywords,
				sizeof(keywords)/sizeof(struct _pfind_keymap),
				sizeof(struct _pfind_keymap), keyword_comparison);

		if (keymap == NULL) {
			keymap = &key;
			if (Sflag)
				fprintf(stderr, "Next pattern is \"%s\".\n", *arg);
			if (**arg == '-')
				warnx("pattern starts with a hyphen: %s", *arg);
		}
		else {
			if (Sflag)
				fprintf(stderr, "Next keyword is \"%s\".\n", *arg);
		}

		/* hmmm. monolithic */
		expression->op = keymap->op;

		switch (keymap->op) {
			case PFIND_PATTERN:
				if (strlen(*arg) == 0) {
					warnx("skipped empty pattern");
					continue;
				}

				expression->op = PFIND_PRI_NAME;

				if (xflag) {
					if ((expression->optarg.str = strdup(*arg)) == NULL)
						err(EX_OSERR, NULL);
				}
				else {
					if (regcomp(&expression->optarg.re.r, *arg, REG_EXTENDED | REG_NOSUB | (iflag ? REG_ICASE : 0)) != 0)
						errx(EX_UNAVAILABLE, "unable to compile regular expression");
					if ((expression->optarg.re.pattern = strdup(*arg)) == NULL)
						err(EX_OSERR, NULL);
				}
				expression->eval = &pfind_pri_name;
				expression->children = NULL;
				expression->next = strategy->next = expression + 1;

				expression->next->invert_after = 0;
				expression->next->previous = expression;
				expression->next->parent = expression->parent;
				expression->next->depth = expression->depth;
				break;
			case PFIND_PRI_NAME:
			case PFIND_PRI_ARGS:
			case PFIND_PRI_ANY:
				if (count < 2)
					errx(EX_USAGE, "pattern required");

				(--count, ++arg);

				if (strlen(*arg) == 0) {
					warnx("skipped empty pattern");
					continue;
				}

				if (xflag && keymap->op == PFIND_PRI_NAME) {
					if ((expression->optarg.str = strdup(*arg)) == NULL)
						err(EX_OSERR, NULL);
				}
				else {
					if (regcomp(&expression->optarg.re.r, *arg,
						(xflag ? REG_NOSPEC : REG_EXTENDED) | REG_NOSUB | (iflag ? REG_ICASE : 0)) != 0)
						errx(EX_UNAVAILABLE, "unable to compile regular expression");
					if ((expression->optarg.re.pattern = strdup(*arg)) == NULL)
						err(EX_OSERR, NULL);
				}

				if (keymap->op == PFIND_PRI_ANY)
					expression->eval = &pfind_pri_any;
				else
				if (keymap->op == PFIND_PRI_ARGS)
					expression->eval = &pfind_pri_args;
				else
					expression->eval = &pfind_pri_name;

				PLAIN_EXPR();
				break;
			case PFIND_PRI_ASK:
				expression->eval = &pfind_pri_ask;
				PLAIN_EXPR();
				break;
			case PFIND_PRI_EUID:
			case PFIND_PRI_UID:
				if (count < 2)
					errx(EX_USAGE, "uid list required");

				(void)parseUidList(*(--count, ++arg), &expression->optarg.uidlist);
				expression->eval = &pfind_pri_euid;

				PLAIN_EXPR();
				break;
			case PFIND_PRI_GID:
				if (count < 2)
					errx(EX_USAGE, "gid list required");

				(void)parseGroupList(*(--count, ++arg), &expression->optarg.grouplist);
				expression->eval = &pfind_pri_gid;

				PLAIN_EXPR();
				break;
			case PFIND_PRI_PGRP:
				if (count < 2)
					errx(EX_USAGE, "pgrp list required");

				(void)parsePidList(*(--count, ++arg), &expression->optarg.pidlist);
				expression->eval = &pfind_pri_pgrp;

				PLAIN_EXPR();
				break;
			case PFIND_PRI_PPID:
				if (count < 2)
					errx(EX_USAGE, "pid list required");

				(void)parsePidList(*(--count, ++arg), &expression->optarg.pidlist);
				expression->eval = &pfind_pri_ppid;

				PLAIN_EXPR();
				break;
			case PFIND_PRI_TERM:
				if (count < 2)
					errx(EX_USAGE, "terminal list required");

				(void)parseTermList(*(--count, ++arg), &expression->optarg.termlist);
				expression->eval = &pfind_pri_term;

				PLAIN_EXPR();
				break;
			case PFIND_ACT_DUMP:
				if (count < 2)
					errx(EX_USAGE, "filename required");

				(--count, ++arg);

				if (strlen(*arg) == 0) {
					warnx("skipped dump with empty filename");
					continue;
				}

				{
					int fd = open(*arg, O_WRONLY | O_CREAT | O_TRUNC, 0666);
					ssize_t result;
					if (fd == -1)
						err(EX_OSERR, "could not open file \"%s\" for writing", *arg);
					expression->optarg.dump.fd = fd;
					if ((expression->optarg.dump.dumpfile = strdup(*arg)) == NULL)
						err(EX_OSERR, NULL);
					result = dumpProcHeader(fd);
					if (result == -1 || result == 0)
						err(EX_OSERR, "could not write to \"%s\"", *arg);
				}

				expression->eval = &pfind_act_dump;
				PLAIN_EXPR();
				break;
			case PFIND_ACT_ID:
				expression->eval = &pfind_act_id;
				PLAIN_EXPR();
				break;
			case PFIND_ACT_KILL:
			case PFIND_ACT_RESULT:
			case PFIND_ACT_WAIT:
				if (count < 2)
					errx(EX_USAGE, "signal required");

				(--count, ++arg);

				if (*(*arg) == '-')
					(*arg)++;

				if (strlen(*arg) == 0) {
					warnx("skipped empty signal name");
					continue;
				}

				if (isalpha(**arg)) {
					int sig = signameToSignum(*arg);
					if (sig < 0 || sig > NSIG)
						nosig(*arg);
					expression->optarg.i = sig;
				}
				else
				if (isdigit(**arg)) {
					int sig = strtol(*arg, &endptr, 10);
					if (*endptr || sig < 0 || sig > NSIG)
						nosig(*arg);
					expression->optarg.i = sig;
				}
				else
					nosig(*arg);

				if (infilename == NULL && corefile == NULL) {
					if (expression->op == PFIND_ACT_KILL)
						expression->eval = &pfind_act_kill;
					else
						expression->eval = &pfind_act_wait;
				}
				PLAIN_EXPR();
				break;
			case PFIND_ACT_PRINT:
				expression->eval = &pfind_act_print;
				PLAIN_EXPR();
				break;
			case PFIND_ACT_PS:
				expression->eval = &pfind_act_ps;
				PLAIN_EXPR();
				break;
			case PFIND_ACT_TIMEOUT:
				if (count < 2)
					errx(EX_USAGE, "interval required");

				expression->optarg.i = strtol(*(--count, ++arg), &endptr, 10);
				if (*endptr)
					errx(EX_USAGE, "not a number: %s", *arg);
				if (expression->optarg.i < 0)
					errx(EX_USAGE, "not a positive non-zero interval: %ds", expression->optarg.i);

				if (infilename == NULL && corefile == NULL)
					expression->eval = &pfind_act_timeout;
				PLAIN_EXPR();
				break;
			case PFIND_OP_OPAREN:
			case PFIND_OP_IF:
				other = expression + 1;
				/* open parenthesis */
				expression->eval = &pfind_op_oparen;
				expression->children = strategy->next = other;
				expression->next = NULL; /* PFIND_OP_CPAREN needs to fix this up */

				other->invert_after = 0;
				other->previous = NULL;
				other->parent = expression;
				other->depth = expression->depth + 1;
				break;
			case PFIND_OP_CPAREN:
			case PFIND_OP_ENDIF:
				if (expression->depth < 2) {
					warnx("cannot have %s here: skipped", *arg);
					continue;
				}
				other = expression + 1;

				if (keymap->op == PFIND_OP_ENDIF)
					expression->eval = &pfind_op_oparen;
				else
					expression->eval = &pfind_op_cparen;
				expression->previous = expression->parent; /* replaced */
				expression->previous->next = expression;
				expression->next = strategy->next = other;
				expression->parent = expression->previous->parent; /* replaced */
				expression->children = NULL;
				expression->depth = expression->previous->depth; /* replaced */

				other->invert_after = 0;
				other->previous = expression;
				other->parent = expression->parent;
				other->depth = expression->depth;
				break;
			case PFIND_OP_AND:
				expression->eval = &pfind_op_and;
				PLAIN_EXPR();
				break;
			case PFIND_OP_OR:
				expression->eval = &pfind_op_or;
				PLAIN_EXPR();
				break;
			case PFIND_OP_NOT:
				expression->invert_after = !expression->invert_after;
				break;
			case PFIND_OP_DO:
			case PFIND_OP_ELSE:
				if (expression->depth < 2) {
					warnx("cannot have %s here: skipped", *arg);
					continue;
				}
				other = expression + 1;
				/* open parenthesis */
				if (keymap->op == PFIND_OP_ELSE)
					expression->eval = &pfind_op_else;
				else
					expression->eval = &pfind_op_do;
				expression->previous = expression->parent; /* replaced */
				expression->previous->next = expression; /* replaced */
				expression->children = strategy->next = other;
				expression->next = NULL; /* PFIND_OP_CPAREN needs to fix this up */
				expression->parent = expression->previous->parent; /* replaced */
				expression->depth = expression->previous->depth; /* replaced */
				expression->result = 1; /* match by default */

				other->invert_after = 0;
				other->previous = NULL;
				other->parent = expression;
				other->depth = expression->depth + 1;
				break;
			default:
				errx(EX_SOFTWARE, "unable to interpret expression: support of a feature was missing");
		}
	}
	if (strategy) {
		close_strategy(strategy);
		if (Sflag) {
			fprintf(stderr, "Strategy is:\n");
			expression = strategy;
			while (expression) {
				for (i = 0; i < expression->depth; i++)
					fprintf(stderr, "\t");
				if (expression->invert_after)
					fprintf(stderr, "! ");
				switch (expression->op) {
					case PFIND_PRI_ANY:
					case PFIND_PRI_ARGS:
						if (xflag)
							fprintf(stderr, "args match \"%s\" literally\n", expression->optarg.re.pattern);
						else
							fprintf(stderr, "args match \"%s\"\n", expression->optarg.re.pattern);
						break;
					case PFIND_PRI_ASK:
						fprintf(stderr, "ask whether to proceed with this process\n");
						break;
					case PFIND_PRI_EUID:
						fprintf(stderr, "effective uid is ");
						printUidList(stderr, expression->optarg.uidlist, " or ");
						fprintf(stderr, "\n");
						break;
					case PFIND_PRI_GID:
						fprintf(stderr, "effective gid is ");
						printGroupList(stderr, expression->optarg.grouplist, " or ");
						fprintf(stderr, "\n");
						break;
					case PFIND_PRI_NAME:
						if (xflag)
							fprintf(stderr, "name matches \"%s\" exactly\n", expression->optarg.str);
						else
							fprintf(stderr, "name matches \"%s\"\n", expression->optarg.re.pattern);
						break;
					case PFIND_PRI_PGRP:
						fprintf(stderr, "process group is ");
						printPidList(stderr, expression->optarg.pidlist, " or ");
						fprintf(stderr, "\n");
						break;
					case PFIND_PRI_PPID:
						fprintf(stderr, "parent pid is ");
						printPidList(stderr, expression->optarg.pidlist, " or ");
						fprintf(stderr, "\n");
						break;
					case PFIND_PRI_TERM:
						fprintf(stderr, "terminal is ");
						printTermList(stderr, expression->optarg.termlist, " or ");
						fprintf(stderr, "\n");
						break;
					case PFIND_PRI_UID:
						fprintf(stderr, "real uid is ");
						printUidList(stderr, expression->optarg.uidlist, " or ");
						fprintf(stderr, "\n");
						break;
					case PFIND_ACT_DUMP:
						fprintf(stderr, "dump process info to file \"%s\"\n", expression->optarg.dump.dumpfile);
						break;
					case PFIND_ACT_ID:
						fprintf(stderr, "print pid\n");
						break;
					case PFIND_ACT_KILL:
						fprintf(stderr, "send signal %d (%s)\n", expression->optarg.i, sys_signame[expression->optarg.i]);
						break;
					case PFIND_ACT_PRINT:
						fprintf(stderr, "print pid and name\n");
						break;
					case PFIND_ACT_PS:
						fprintf(stderr, "print pid and argument string\n");
						break;
					case PFIND_ACT_RESULT:
						fprintf(stderr, "send signal %d (%s) and wait for result\n", expression->optarg.i, sys_signame[expression->optarg.i]);
						break;
					case PFIND_ACT_TIMEOUT:
						fprintf(stderr, "wait %ds for termination\n", expression->optarg.i);
						break;
					case PFIND_ACT_WAIT:
						fprintf(stderr, "send signal %d (%s), wait for termination\n", expression->optarg.i, sys_signame[expression->optarg.i]);
						break;
					case PFIND_OP_OPAREN:
						fprintf(stderr, "(\n");
						break;
					case PFIND_OP_CPAREN:
						fprintf(stderr, ")\n");
						break;
					case PFIND_OP_AND:
						fprintf(stderr, "&&\n");
						break;
					case PFIND_OP_OR:
						fprintf(stderr, "||\n");
						break;
					case PFIND_OP_IF:
						fprintf(stderr, "if (\n");
						break;
					case PFIND_OP_DO:
						fprintf(stderr, ") do (\n");
						break;
					case PFIND_OP_ELSE:
						fprintf(stderr, ") else (\n");
						break;
					case PFIND_OP_ENDIF:
						fprintf(stderr, ")\n");
						break;
					default:
						fprintf(stderr, "<indescribable expression>\n");
				}
				if (expression->children != NULL) {
					expression = expression->children;
				}
				else
				if (expression->next != NULL) {
					expression = expression->next;
				}
				else
				if (expression->parent != NULL) {
					if (expression->parent->next != NULL) {
						expression = expression->parent->next;
					}
					else {
						if (expression->depth)
							errx(EX_SOFTWARE, "premature end-of-strategy!");
						expression = NULL;
					}
				}
				else {
					if (expression->depth)
						errx(EX_SOFTWARE, "premature end of strategy");
					expression = NULL;
				}
			}
		}
	}

	if (delim == NULL)
		delim = "\n";

	if (infilename)
		baton = undumpProcList(infilename, &proctoolslist);
	else
		baton = getProcList(corefile, &proctoolslist, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, TRUE, FALSE, FALSE, FALSE, NULL);

	temppl = proctoolslist;

	firstflag = TRUE;
	atexit(&atexit_cleanup);

	if (strategy) {

		while (temppl != NULL) {
			int outputdone = 0;

			expression = strategy;
			while (expression) {
				if (expression->eval) {
					if (!(expression->eval)(&expression, temppl, baton, &outputdone)) {
						/* the evaluator didn't perform a jump, so we have to work it out ourselves */
						if (!expression->result && expression->parent != NULL) {
							if (expression->next && expression->next->op == PFIND_OP_OR) {
								expression = expression->next;
							}
							else
							/* skip to end of bracket-group */
							if (expression->parent->next) {
								expression = expression->parent->next;
							}
							else {
								if (expression->depth)
									errx(EX_SOFTWARE, "premature end-of-strategy!");
								expression = NULL;
							}
						}
						else
						if (expression->children != NULL) {
							expression = expression->children;
						}
						else
						if (expression->next != NULL) {
							expression = expression->next;
						}
						else
						if (expression->parent != NULL) {
							if (expression->parent->next != NULL) {
								expression = expression->parent->next;
							}
							else {
								if (expression->depth)
									errx(EX_SOFTWARE, "premature end-of-strategy!");
								expression = NULL;
							}
						}
						else {
							if (expression->depth)
								errx(EX_SOFTWARE, "premature end of strategy");
							expression = NULL;
						}
					}
				}
				else
					errx(EX_SOFTWARE, "unable to execute strategy: support of a feature was missing");
			}

			if (strategy->next->result && !outputdone) {
				printf("%s%d", firstflag ? (firstflag = FALSE, "") : delim, temppl->pid);
			}

			temppl = temppl->next;
		}
	}
	else {
		while (temppl != NULL) {
			printf("%s%d", firstflag ? (firstflag = FALSE, "") : delim, temppl->pid);
			temppl = temppl->next;
		}
	}

	if (baton)
		freeProcList(baton);

	return proctoolslist == NULL ? 1 : EX_OK;
}

__dead static void
nosig(name)
	char *name;
{
	warnx("not a signal: %s", name);
	warnx("valid signals:");
	printSignals(stderr);
	exit(EX_USAGE);
}

