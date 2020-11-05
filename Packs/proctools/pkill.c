/*
 * pkill [-s <signal>|-<signal>] [-finvVx] [-g <pgrplist>] [-G <gidlist>]
 *       [-P <ppidlist>] [-t <termlist>] [-u <euidlist>]
 *       [-U <uidlist>] [<pattern>]
 *
 *  -s <signal> : sends specified signal to process (default SIGTERM)
 *  -<signal> : same as -S
 *  -f : match against full name, not just executable name
 *  -g <pgrplist> : matches process groups
 *  -G <gidlist> : matches group IDs
 *  -i : case-insensitive matching
 *  -n : matches only newest process that matches otherwise
 *  -P <ppidlist> : matches parent pids
 *  -t <termlist> : matches terminal
 *  -u <euidlist> : matches effective uids
 *  -U <uidlist> : matches real uids
 *  -v : invert match
 *  -V : print version identifier
 *  -x : exact match (default regex)
 *
 *  <pattern> : regex (or exact string if -x) to match
 */

#include <sys/types.h>

#include <ctype.h>
#include <err.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "proctools.h"

__dead static void nosig(char *);
__dead static void usage();

extern char *__progname;

/*
 * pkill matches processes specified by its arguments and sends a signal to them
 */
int
main(argc, argv)
	int argc;
	char *argv[];
{
	int fflag, iflag, nflag, vflag, xflag, ch, numsig;
	struct pidlist *pgroupl, *ppidl, *sidl;
	struct proctoolslist *proctoolslist, *temppl;
	struct uidlist *euidl, *uidl;
	struct baton *baton;
	struct grouplist *gidl;
	struct termlist *terml;
	char *newarg;
	char *endptr;
	size_t len;

	if (argc < 2) {
		(void)fprintf(stderr, "%s: no matching criteria specified\n", __progname);
		usage();
	}

	proctoolslist = NULL;
	gidl = NULL;
	pgroupl = ppidl = sidl = NULL;
	terml = NULL;
	euidl = uidl = NULL;
	fflag = iflag = xflag = nflag = vflag = FALSE;
	numsig = SIGTERM;
	newarg = NULL;

	if (argc > 1) {
		argv++;
		if (**argv == '-') {
			++*argv;
			if (isalpha(**argv)) {
				if ((numsig = signameToSignum(*argv)) < 0) {
					numsig = SIGTERM;
					--*argv;
				}
				else {
					len = strlen(*argv) + 3;
					if ((newarg = calloc(len, sizeof(char))) == NULL)
						err(EX_OSERR, NULL);
					snprintf(newarg, len, "-s%s", *argv);
					--*argv;
					*argv = newarg;
				}
			}
			else
			if (isdigit(**argv)) {
				numsig = strtol(*argv, &endptr, 10);
				if (*endptr != '\0') {
					numsig = SIGTERM;
					--*argv;
				}
				else {
					len = strlen(*argv) + 3;
					if ((newarg = calloc(len, sizeof(char))) == NULL)
						err(EX_OSERR, NULL);
					snprintf(newarg, len, "-s%s", *argv);
					--*argv;
					*argv = newarg;
				}
				if (numsig < 0 || numsig >= NSIG)
					nosig(*argv);
			}
		}
		argv--;
	}

	while ((ch = getopt(argc, argv, "fg:G:inP:s:t:u:U:vVx")) != -1)
		switch ((char)ch) {
		case 'f':
			fflag = TRUE;
			break;
		case 'g':
			(void)parsePidList(optarg, &pgroupl);
			break;
		case 'G':
			(void)parseGroupList(optarg, &gidl);
			break;
		case 'i':
			iflag = TRUE;
			break;
		case 'n':
			nflag = TRUE;
			break;
		case 'P':
			(void)parsePidList(optarg, &ppidl);
			break;
		case 's':
			if (*optarg == '-')
				optarg++;
			if (isalpha(*optarg)) {
				if ((numsig = signameToSignum(optarg)) < 0)
					nosig(optarg);
			}
			else
			if (isdigit(*optarg)) {
				numsig = strtol(optarg, &endptr, 10);
				if (*endptr != '\0')
					nosig(optarg);
				if (numsig < 0 || numsig >= NSIG)
					nosig(optarg);
			}
			else
				nosig(optarg);
			break;
		case 't':
			(void)parseTermList(optarg, &terml);
			break;
		case 'u':
			(void)parseUidList(optarg, &euidl);
			break;
		case 'U':
			(void)parseUidList(optarg, &uidl);
			break;
		case 'v':
			vflag = TRUE;
			break;
		case 'V':
			(void)fprintf(stderr, "%s (proctools " VERSION ") http://proctools.sourceforge.net\n", __progname);
			exit(EX_OK);
			/* NOTREACHED */
		case 'x':
			xflag = TRUE;
			break;
		case '?':
		default:
			usage();
			/* NOTREACHED */
		}

	argc -= optind;
	argv += optind;

	if (newarg != NULL)
		free(newarg);

	if (argc > 1) {
		warn("too many arguments");
		usage();
	}

	baton = getProcList (NULL, &proctoolslist, euidl, uidl, gidl, ppidl, pgroupl, terml, fflag, iflag, nflag, vflag, xflag, ((argc > 0)?argv[0]:NULL));

	temppl = proctoolslist;
	while (temppl != NULL) {
		if (kill(temppl->pid, numsig) < 0)
			warn("pid %d", temppl->pid);
		temppl = temppl->next;
	}

	freeProcList(baton);

	return proctoolslist == NULL ? 1 : EX_OK;
}

/*
 * expanded warning for bogus signal names
 */
__dead static void
nosig(name)
	char *name;
{
	warnx("unknown signal %s; valid signals:", name);
	printSignals(stderr);
	exit(EX_USAGE);
}

/*
 * prints out the usage of the program and exits
 */
__dead static void
usage()
{
	(void)fprintf(stderr, "Usage: %s [-<signal>] [-finvVx] [-g <pgrplist>] [-G <gidlist>] [-P <ppidlist>] [-s <signal>] [-t <termlist>] [-u <euidlist>] [-U <uidlist>] [<pattern>]\n", __progname);
	exit(EX_USAGE);
}


/*
 * Copyright (c) 2001 William B Faulk.  All rights reserved.
 * Copyright (c) 2003 James Devenish.  All rights reserved.
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
 *
 *
 * Portions of this code taken from the file:
 *      $OpenBSD: kill.c,v 1.3 1997/02/06 13:29:08 deraadt Exp $
 *      $NetBSD: kill.c,v 1.11 1995/09/07 06:30:27 jtc Exp $
 * which has the following license:
 *
 * Copyright (c) 1988, 1993, 1994
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * and the file:
 *      $OpenBSD: ps.c,v 1.19 2001/04/17 21:12:07 millert Exp $
 *      $NetBSD: ps.c,v 1.15 1995/05/18 20:33:25 mycroft Exp $
 * which has the following license:
 *
 * Copyright (c) 1990, 1993, 1994
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
