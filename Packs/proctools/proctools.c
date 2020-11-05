/**
 * $Id: proctools.c,v 1.39 2003/12/06 11:41:26 devenish Exp $
 */

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <sys/user.h>

#include <err.h>
#include <fcntl.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <regex.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "proctools.h"

extern char *cmdpart(char *);
extern void  fmt_puts(char *, int *);
extern void  fmt_argv(kvm_t *kd, struct kinfo_proc *ki);

static char  dbuf[DUMPBUFSZ];

static void	 printProcInfoNative(struct baton *, const struct proctoolslist *proctoolslist);
static int	 pushProcList(struct proctoolslist **, struct kinfo_proc *kpi);
static int	 pushProcListDetails(struct proctoolslist **, struct kinfo_proc *kpi, pid_t, char *);
static int	 popProcList(struct proctoolslist **, pid_t);
static const char * procArgs(struct baton *baton, struct kinfo_proc *kp);

#if defined(USE_KVM)
static char *kvmArgs(struct baton *baton, struct kinfo_proc *kp);
#endif

#if defined(KERN_PROCARGS2)
static char *procArgs2(struct baton *baton, pid_t pid);
#endif


/*
 * Takes a comma/whitespace separated list of pids and places
 * them in a linked list in an unspecified order.
 */
int
parsePidList(pidstring, pidlist)
	char *pidstring;
	struct pidlist **pidlist;
{
	struct pidlist *head, *pl;
	int bad, invalid;
	char *stringp;
	char *endptr;
	pid_t pid;

	*pidlist = head = NULL;
	invalid = 0;

	while ((stringp = strsep(&pidstring, ", \t")) != NULL) {
		if (*stringp != '\0') {
			bad = 0;
			pid = (pid_t)strtol(stringp, &endptr, 10);
			if (*endptr != '\0') {
				warnx("unable to parse pid: %s", stringp);
				bad++;
			}
			if (bad == 0) {
				if ((pl = calloc(1, sizeof(struct pidlist))) == NULL)
					err(EX_OSERR, NULL);
				pl->pid = pid;
				pl->next = NULL;
				if (*pidlist == NULL) {
					*pidlist = pl;
					head = *pidlist;
				}
				else {
					(*pidlist)->next = pl;
					*pidlist = (*pidlist)->next;
				}
			} else
				invalid++;
		}
	}

	*pidlist = head;
	return (invalid);
}

/*
 * Takes a comma/whitespace separated list of usernames and uids
 * and places them in a linked list in an unspecified order.
 */
int
parseUidList(uidstring, uidlist)
	char *uidstring;
	struct uidlist **uidlist;
{
	struct uidlist *head, *ul;
	struct passwd *tempu;
	int bad, invalid;
	char *stringp;
	char *endptr;
	uid_t uid;

	*uidlist = head = NULL;
	invalid = 0;

	while ((stringp = strsep(&uidstring, ", \t")) != NULL) {
		if (*stringp != '\0') {
			bad = 0;
			tempu = getpwnam(stringp);
			if (tempu == NULL) {
				uid = (uid_t)strtol(stringp, &endptr, 10);
				if (*endptr != '\0') {
					warnx("unable to parse uid: %s", stringp);
					bad++;
				}
			}
			else
				uid = tempu->pw_uid;
			if (bad == 0) {
				if ((ul = calloc(1, sizeof(struct uidlist))) == NULL)
					err(EX_OSERR, NULL);
				ul->uid = uid;
				ul->next = NULL;
				if (*uidlist == NULL) {
					*uidlist = ul;
					head = *uidlist;
				}
				else {
					(*uidlist)->next = ul;
					*uidlist = (*uidlist)->next;
				}
			} else
				invalid++;
		}
	}

	*uidlist = head;
	return (invalid);
}

/*
 * Takes a comma/whitespace separated list of groups and gids
 * and places them in a linked list in an unspecified order.
 */
int
parseGroupList(groupstring, grouplist)
	char *groupstring;
	struct grouplist **grouplist;
{
	struct grouplist *gl, *head;
	struct group *tempg;
	int bad, invalid;
	char *stringp;
	char *endptr;
	gid_t group;

	*grouplist = head = NULL;
	invalid = 0;

	while ((stringp = strsep(&groupstring, ", \t")) != NULL) {
		if (*stringp != '\0') {
			bad = 0;
			tempg = getgrnam(stringp);
			if (tempg == NULL) {
				group = (gid_t)strtol(stringp, &endptr, 10);
				if (*endptr != '\0') {
					warnx("unable to parse group: %s", stringp);
					bad++;
				}
			} else
				group = tempg->gr_gid;
			if (bad == 0) {
				if ((gl = calloc(1, sizeof(struct grouplist))) == NULL)
					err(EX_OSERR, NULL);
				gl->group = group;
				gl->next = NULL;
				if (*grouplist == NULL) {
					*grouplist = gl;
					head = *grouplist;
				}
				else {
					(*grouplist)->next = gl;
					*grouplist = (*grouplist)->next;
				}
			}
			else
				invalid++;
		}
	}

	*grouplist = head;
	return (invalid);
}

/*
 * Takes a comma/whitespace separated list of filenames, translates
 * them to device numbers if possible and places them in a linked list
 * in an unspecified order.  If the filename does not begin with a '/',
 * it assumes that the file lies within the /dev directory.
 */
int
parseTermList(termstring, termlist)
	char *termstring;
	struct termlist **termlist;
{
	struct termlist *head, *tl;
	struct stat statbuf;
	int bad, invalid;
	char *stringp;
	char *temps;
	dev_t term;
	size_t len;

	*termlist = head = NULL;
	invalid = 0;

	while ((stringp = strsep(&termstring, ", \t")) != NULL) {
		if (*stringp != '\0') {
			bad = 0;
			if (*stringp != '/') {
				len = strlen(stringp);
				len += 6; /* for "/dev/" */
				if ((temps = calloc(len, sizeof(char))) == NULL)
					err(EX_OSERR, NULL);
				snprintf(temps, len, "/dev/%s", stringp);
				stringp = temps;
			}
			if (stat(stringp, &statbuf) == 0)
				if (statbuf.st_mode | S_IFCHR)
					term = statbuf.st_rdev;
				else {
					bad++;
					warnx("not a character device: %s", stringp);
				}
			else {
				bad++;
				warn("can't stat: %s", stringp);
			}
			if (bad == 0) {
				if ((tl = calloc(1, sizeof(struct termlist))) == NULL)
					err(EX_OSERR, NULL);
				tl->term = term;
				tl->next = NULL;
				if (*termlist == NULL) {
					*termlist = tl;
					head = *termlist;
				}
				else {
					(*termlist)->next = tl;
					*termlist = (*termlist)->next;
				}
			}
			else
				invalid++;
		}
	}

	*termlist = head;
	return (invalid);
}

void
printPidList(f, pidlist, sepstring)
	FILE *f;
	struct pidlist *pidlist;
	char *sepstring;
{
	while (pidlist) {
		fprintf(f, "%d", pidlist->pid);
		pidlist = pidlist->next;
		if (pidlist)
			fprintf(f, "%s", sepstring);
	}
}

void
printUidList(f, uidlist, sepstring)
	FILE *f;
	struct uidlist *uidlist;
	char *sepstring;
{
	while (uidlist) {
		fprintf(f, "%d", uidlist->uid);
		uidlist = uidlist->next;
		if (uidlist)
			fprintf(f, "%s", sepstring);
	}
}

void
printGroupList(f, grouplist, sepstring)
	FILE *f;
	struct grouplist *grouplist;
	char *sepstring;
{
	while (grouplist) {
		fprintf(f, "%d", grouplist->group);
		grouplist = grouplist->next;
		if (grouplist)
			fprintf(f, "%s", sepstring);
	}
}

void
printTermList(f, termlist, sepstring)
	FILE *f;
	struct termlist *termlist;
	char *sepstring;
{
	while (termlist) {
		fprintf(f, "%d", termlist->term);
		termlist = termlist->next;
		if (termlist)
			fprintf(f, "%s", sepstring);
	}
}

/*
 * Takes a uid and a linked list of uids and checks to see
 * if the uid exists within the linked list.
 */
int
matchUidList(uidlist, uid)
	struct uidlist *uidlist;
	uid_t uid;
{
	struct uidlist *tempul;

	if (uidlist == NULL)
		return (TRUE);

	for (tempul = uidlist; tempul != NULL; tempul = tempul->next)
		if (tempul->uid == uid)
			return (TRUE);
	return (FALSE);
}

/*
 * Takes a gid and a linked list of gids and checks to see
 * if the gid exists within the linked list.
 */
int
matchGroupList(grouplist, gid)
	struct grouplist *grouplist;
	gid_t gid;
{
	struct grouplist *tempgl;

	if (grouplist == NULL)
		return (TRUE);

	for (tempgl = grouplist; tempgl != NULL; tempgl = tempgl->next)
		if (tempgl->group == gid)
			return (TRUE);
	return (FALSE);
}

/*
 * Takes a pid and a linked list of pids and checks to see
 * if the pid exists within the linked list.
 */
int
matchPidList(pidlist, pid)
	struct pidlist *pidlist;
	pid_t pid;
{
	struct pidlist *temppl;

	if (pidlist == NULL)
		return (TRUE);

	for (temppl = pidlist; temppl != NULL; temppl = temppl->next)
		if (temppl->pid == pid)
			return (TRUE);
	return (FALSE);
}

/*
 * Takes a device number and a linked list of device numbers and
 * checks to see if the device number exists within the linked list.
 */
int
matchTermList(termlist, term)
	struct termlist *termlist;
	dev_t term;
{
	struct termlist *temptl;

	if (termlist == NULL)
		return (TRUE);

	for (temptl = termlist; temptl != NULL; temptl = temptl->next)
		if (temptl->term == term)
			return (TRUE);
	return (FALSE);
}

/*
 * Removes the identified process from an existing linked list.
 * Stops after finding the first match.
 * Returns the number of processes removed.
 * The name 'pop' is a misnomer.
 */
static int
popProcList(proctoolslist, pid)
	struct proctoolslist **proctoolslist;
	pid_t pid;
{
	struct proctoolslist *temppl, *lastpl = NULL;

	if (proctoolslist == NULL)
		return (0);

	temppl = *proctoolslist;

	while (temppl) {
		if (temppl->pid == pid) {
			struct proctoolslist *oldpl = temppl;
			if (lastpl)
				lastpl->next = oldpl->next;
			if (oldpl == *proctoolslist)
				*proctoolslist = oldpl->next;
			free(oldpl->name);
			free(oldpl);
			return (1);
		}
		lastpl = temppl;
		temppl = temppl->next;
	}

	return (0);
}

static int
pushProcList(proctoolslist, kp)
	struct proctoolslist **proctoolslist;
	struct kinfo_proc *kp;
{
	return pushProcListDetails(proctoolslist, kp, kp->kp_proc.p_pid, kp->kp_proc.p_comm);
}

/*
 * Pushes a new process and its process name into a linked
 * list of processes, creating the linked list if necessary.
 * Ignores the current process.
 * Returns the number of processes added to the list.
 * Allocates memory and duplicates the name.
 * Note: allows duplicates.
 */
static int
pushProcListDetails(proctoolslist, kp, pid, name)
	struct proctoolslist **proctoolslist;
	struct kinfo_proc *kp;
	pid_t pid;
	char *name;
{
	struct proctoolslist *temppl;

	if ((temppl = calloc(1, sizeof(struct proctoolslist))) == NULL)
		err(EX_OSERR, NULL);
	if ((temppl->name = strdup(name)) == NULL)
		err(EX_OSERR, NULL);
	temppl->kp = kp;
	temppl->pid = pid;
	temppl->next = *proctoolslist;

	*proctoolslist = temppl;
	return (1);
}

#if defined(USE_KVM)
static char *
kvmArgs(struct baton *baton, struct kinfo_proc *kp)
{
	char *c = baton->args, *p;
	char *end = c + baton->argmax - 1;
	char **argv = kvm_getargv(baton->kd, kp, 0);
	if (argv) {
		while (*argv) {
			p = *argv++;
			while (*p && c < end)
				*c++ = *p++;
			*c++ = ' ';
		}
	}
	if (c == baton->args) {
		return NULL;
	}
	else {
		*--c = '\0';
		return baton->args;
	}
}
#endif

#if defined(KERN_PROCARGS2)
static char *
procArgs2(struct baton *baton, pid_t pid)
{
	size_t len = baton->argmax;

	*baton->pid = (int)pid;
	if (sysctl(baton->mib, baton->miblen, baton->args, &len, NULL, 0) == -1) {
		return NULL;
	}
	else {
		int argc = *(int*)baton->args;
		char *p = baton->args + sizeof(int); /* ick */
		char *end = baton->args + len;
		char *c = baton->args;

		p += strlen(p); /* skip exec path */
		for (; p<end; p++) {
			if (*p == '\0') { /* skip NULs */
				if ((p + 1) < end && *(p + 1)) {
					break;
				}
			}
		}
		for (; p < end; p++) {
			if (*p == '\0') {
				p++;
				break;
			}
		}

		/* overwrite our own buffer */
		/* c always less than baton->args + baton->argmax */
		for (; p < end && argc; p++) {
			if (*p != '\0') {
				*c++ = *p;
			}
			else {
				*c++ = ' ';
				if ((p + 1) < end) {
					argc--;
				}
			}
		}

		if (c == baton->args) {
			return NULL;
		}
		else {
			*c = '\0';
			return baton->args;
		}
	}
}
#endif

static const char *
procArgs(baton, kp)
	struct baton *baton;
	struct kinfo_proc *kp;
{
	char *name;
#if defined(USE_KVM)
	name = kvmArgs(baton, kp);
#elif defined(KERN_PROCARGS2)
	name = procArgs2(baton, kp->kp_proc.p_pid);
#else
	errx(EX_SOFTWARE, "no recognised sysctl interface for obtaining process arguments");
#endif
	if (name == NULL)
		name = kp->kp_proc.p_comm;
	return name;
}

/**
 * The string may contain 'unprintable' characters. See printProcInfo, too.
 */
const char *
getProcArgs(baton, proctoolslist)
	struct baton *baton;
	const struct proctoolslist *proctoolslist;
{
	const char *name;
	switch (baton->type) {
		case PROCTOOLS_BATON_UNDUMPED:
			name = proctoolslist->kp->kp_proc.p_wmesg; /* we hijacked it!! */
			break;
		case PROCTOOLS_BATON_NATIVE:
#if defined(USE_KVM)
			name = kvmArgs(baton, proctoolslist->kp);
#elif defined(KERN_PROCARGS2)
			name = procArgs2(baton, proctoolslist->pid);
#else
			errx(EX_SOFTWARE, "no recognised sysctl interface for obtaining process arguments");
#endif
			break;
		default:
			errx(EX_SOFTWARE, "unrecognised baton type");
			break;
	}
	if (name == NULL)
		name = proctoolslist->kp->kp_proc.p_comm;
	return name;
}

#define UNDUMP(X, T) do {                                       \
	result = read(fd, dbuf, DUMPBUFSZ);                         \
	if (result == 0)                                            \
		goto undump_cleanup;                                    \
	if (result == -1 || result != DUMPBUFSZ) {                  \
		warn("could not read entire dumpfile");                 \
		goto undump_cleanup;                                    \
	}                                                           \
	val = 0;                                                    \
	for (j = 0, i = (DUMPBUFSZ-1)<<3; j < DUMPBUFSZ; j++, i-=8) \
		val |= (dbuf[j]&0xFF)<<i;                               \
	(X) = (T)val;                                               \
} while (0)

struct baton *
undumpProcList(filename, proctoolslist)
	const char *filename;
	struct proctoolslist **proctoolslist;
{
	int fd;
	size_t s;
	int i, j;
	u_int64_t val;
	ssize_t result;
	struct baton *baton;
	struct proctoolslist *temp, *last;
	char sbuf[4096];

	baton = calloc(1,sizeof(struct baton));
	if (baton == NULL)
		err(EX_OSERR, NULL);
	baton->type = PROCTOOLS_BATON_UNDUMPED;
	baton->args = malloc(4096);
	if (baton->args == NULL)
		err(EX_OSERR, NULL);

	*proctoolslist = NULL;

	fd = open(filename, O_RDONLY, 0);
	if (fd == -1)
		err(EX_OSERR, "could not open file \"%s\" for reading", filename);

	/* check signature */
	s = strlen(DUMPSIGNATURE)+1;
	result = read(fd, sbuf, s);
	if (result == -1 || result != s)
		errx(EX_OSERR, "%s: not a compatible proctools dumpfile", filename);

	do {
		temp = malloc(sizeof(struct proctoolslist));
		if (temp == NULL)
			err(EX_OSERR, NULL);
		temp->kp = malloc(sizeof(struct kinfo_proc));
		if (temp->kp == NULL)
			err(EX_OSERR, NULL);

		/* for this dump version */
		UNDUMP(temp->pid, pid_t);
		UNDUMP(s, size_t);
		result = read(fd, sbuf, s);
		if (result == -1 || result != s) {
			warn("could not read entire dumpfile"); \
			goto undump_cleanup; \
		}
		sbuf[s] = '\0';
		if ((temp->name = strdup(sbuf)) == NULL)
			err(EX_OSERR, NULL);
		snprintf(temp->kp->kp_proc.p_comm, MAXCOMLEN+1, "%s", temp->name);
		UNDUMP(temp->kp->kp_eproc.e_ppid, pid_t);
		UNDUMP(temp->kp->kp_eproc.e_pgid, pid_t);
		UNDUMP(temp->kp->kp_eproc.e_ucred.cr_gid, gid_t);
		UNDUMP(temp->kp->kp_eproc.e_pcred.p_ruid, uid_t);
		UNDUMP(temp->kp->kp_eproc.e_pcred.p_svuid, uid_t);
		UNDUMP(temp->kp->kp_eproc.e_tdev, dev_t);
		UNDUMP(s, size_t);
		result = read(fd, sbuf, s);
		if (result == -1 || result != s) {
			warn("could not read entire dumpfile"); \
			goto undump_cleanup; \
		}
		sbuf[s] = '\0';
		temp->kp->kp_proc.p_wmesg = strdup(sbuf);  /* we're hijacking it!! */
		if (temp->kp->kp_proc.p_wmesg == NULL)
			err(EX_OSERR, NULL);

		if (*proctoolslist) {
			last->next = temp;
			last = temp;
		}
		else
			*proctoolslist = last = temp;

		temp = NULL;
	}
	while (1);
undump_cleanup:
	if (temp)
		free(temp);
	(void)close(fd);
	return(baton);
}

/*
 * Parses the kernel structures containing process information searching
 * for processes that match the information stored in the linked lists
 * and flags supplied. Will not match the current process.
 *
 * Returns an opaque type that can be used for subsequent
 * printProcInfo calls.
 */
struct baton *
getProcList(corefile, proctoolslist, euidlist, uidlist, gidlist, ppidlist, pgrouplist, termlist, fullmatch, ignorecase, lastonly, invert, exact, pattern)
	const char *corefile;
	struct proctoolslist **proctoolslist;
	struct uidlist *euidlist;
	struct uidlist *uidlist;
	struct grouplist *gidlist;
	struct pidlist *ppidlist;
	struct pidlist *pgrouplist;
	struct termlist *termlist;
	int fullmatch;
	int ignorecase;
	int lastonly;
	int invert;
	int exact;
	char *pattern;
{
	struct baton *baton;
	struct kinfo_proc *kp, kpi;
	struct {
		struct	timeval time;
		struct  kinfo_proc *kp;
		char	*name;
		pid_t	pid;
		int	valid;
	} latest;
	regex_t regex;
	int i, nproc;
	int extmatch, match;
	char *name;
#if defined(USE_KVM)
	kvm_t *kd;
	char errbuf[_POSIX2_LINE_MAX];
#elif defined(USE_SYSCTL)
#define MAX_NPROC 16383
	int maxproc;
#endif
	int mib[5];
	size_t len;

	baton = calloc(1,sizeof(struct baton));
	if (baton == NULL)
		err(EX_OSERR, NULL);
	baton->type = PROCTOOLS_BATON_NATIVE;

#if defined(USE_KVM)
	if (corefile)
		kd = kvm_openfiles(NULL, corefile, NULL, O_RDONLY, errbuf);
	else
#if defined(USE_KVM_NO_FILES)
		kd = kvm_openfiles(NULL, NULL, NULL, KVM_NO_FILES, errbuf);
#else
		kd = kvm_openfiles(NULL, "/dev/null", NULL, O_RDONLY, errbuf);
#endif
	if (kd == 0)
		errx(EX_UNAVAILABLE, "%s", errbuf);
#else
#if defined(KERN_PROCARGS2)
	baton->mib[0] = CTL_KERN;
	baton->mib[1] = KERN_PROCARGS2;
	baton->mib[2] = 0;
	baton->mib[3] = 0;

	baton->miblen = 3;

	baton->pid = &baton->mib[2];
#endif

	mib[0] = CTL_KERN;
	mib[1] = KERN_MAXPROC;
	mib[2] = 0;
	len = sizeof(maxproc);
	if (sysctl(mib, 2, &maxproc, &len, NULL, 0) == -1)
		err(EX_UNAVAILABLE, "could not use sysctl to read KERN_MAXPROC");
	if (len != sizeof(maxproc))
		errx(EX_SOFTWARE, "size mismatch while reading KERN_MAXPROC");

	/**
	 * Minimise the number of procs by choosing the most restrictive
	 * criteria available.
	 */
#if defined(KERN_NPROCS)
	mib[0] = CTL_KERN;
	mib[1] = KERN_NPROCS;
	mib[2] = 0;
	len = sizeof(nproc);
	if (sysctl(mib, 2, &nproc, &len, NULL, 0) == -1)
		err(EX_UNAVAILABLE, "could not use sysctl to read KERN_NPROCS");
	if (len != sizeof(nproc))
		errx(EX_SOFTWARE, "size mismatch while reading KERN_NPROCS");

	if (nproc > maxproc)
		errx(EX_OSERR, "sanity failed: KERN_NPROCS greater than KERN_MAXPROC");
	if (nproc > MAX_NPROC)
		errx(EX_UNAVAILABLE, "proctools recognises at most %d processes (found %d)", MAX_NPROC, nproc);
#else
	nproc = MAX_NPROC;
	if (maxproc < nproc)
		nproc = maxproc;
#endif
#endif

	mib[0] = CTL_KERN;
	mib[1] = KERN_ARGMAX;
	mib[2] = 0;
	len = sizeof(baton->argmax);
	if (sysctl(mib, 2, &baton->argmax, &len, NULL, 0) == -1)
		err(EX_UNAVAILABLE, "could not use sysctl to read KERN_ARGMAX");
	if (len != sizeof(baton->argmax))
		errx(EX_SOFTWARE, "size mismatch while reading KERN_ARGMAX");
	baton->args = malloc(baton->argmax);
	if (baton->args == NULL)
		err(EX_OSERR, NULL);

	/*
	setegid(getgid());
	setgid(getgid());
	*/

#if defined(USE_KVM)
	if ((kp = kvm_getprocs(kd, KERN_PROC_ALL, 0, &nproc)) == 0)
		errx(EX_UNAVAILABLE, "%s", kvm_geterr(kd));
#else
	len = nproc * sizeof(struct kinfo_proc);
	kp = malloc(len);
	if (kp == NULL)
		err(EX_OSERR, NULL);

	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_ALL;
	mib[3] = 0;

	if (sysctl(mib, 3, kp, &len, NULL, 0) == -1)
		err(EX_UNAVAILABLE, "could not use sysctl for KERN_PROC");
	if (len % sizeof(struct kinfo_proc) != 0)
		errx(EX_SOFTWARE, "struct kinfo_proc size mismatch");
	nproc = len / sizeof(struct kinfo_proc);
	/**
	 * note: number of returned procs might be larger than the allocated
	 * space. should we compensate indefinitely?
	 **/
#endif

	if (!exact && pattern != NULL)
		if (regcomp(&regex, pattern, REG_EXTENDED | REG_NOSUB | (ignorecase ? REG_ICASE : 0)) != 0)
			err(EX_UNAVAILABLE, "unable to compile regular expression");

	if (lastonly)
		latest.valid = FALSE;

#if defined(USE_KVM)
	baton->kd = kd;
#else
	baton->kp = kp;
#endif

	if (pattern == NULL && !lastonly) {
		/* special case: return all processes other than this one */
		for (i = nproc; --i >= 0; ++kp) {
			kpi = *kp;
			if (kpi.kp_proc.p_pid == getpid())
				continue;
			pushProcList(proctoolslist, kp);
		}
	}
	else {
		for (i = nproc; --i >= 0; ++kp) {
			kpi = *kp;
			if (kpi.kp_proc.p_pid == getpid())
				continue;
			match = (matchPidList(pgrouplist, kpi.kp_eproc.e_pgid) &&
				matchGroupList(gidlist, kpi.kp_eproc.e_ucred.cr_gid) &&
				matchPidList(ppidlist, kpi.kp_eproc.e_ppid) &&
				matchTermList(termlist, kpi.kp_eproc.e_tdev) &&
				matchUidList(euidlist, kpi.kp_eproc.e_pcred.p_svuid) &&
				matchUidList(uidlist, kpi.kp_eproc.e_pcred.p_ruid));
			extmatch = pgrouplist || gidlist || ppidlist || termlist || euidlist || uidlist;
			if (match && !invert) {
				if (fullmatch) {
					if ((name = strdup(procArgs(baton, kp))) == NULL)
						err(EX_OSERR, NULL);
				}
				else {
					if ((name = strdup(kpi.kp_proc.p_comm)) == NULL)
						err(EX_OSERR, NULL);
				}
				if (lastonly) {
					struct timeval *start;
					int matched;
#if defined(USE_KVM)
					start = &kpi.kp_eproc.e_pstats.p_start;
#elif defined(p_starttime)
					start = &kpi.kp_proc.p_starttime;
#else
					errx(EX_SOFTWARE, "lastonly not supported for sysctl");
#endif
					if (pattern != NULL)
					{
						if (exact)
						{
							if (ignorecase)
								matched = !strcasecmp(name, pattern);
							else
								matched = !strcmp(name, pattern);
						}
						else
						{
							//matched = regexec(&regex, name, 0, NULL, NULL) == 0;
							matched = regexec(&regex, name, 0, NULL, 0) == 0;
						}
					}
					else
					{
						matched = TRUE;
					}

					if (matched) {
						if (!latest.valid) {
							latest.valid = TRUE;
							latest.kp = kp;
							latest.time.tv_sec = start->tv_sec;
							latest.time.tv_usec = start->tv_usec;
							if ((latest.name = strdup(kpi.kp_proc.p_comm)) == NULL)
								err(EX_OSERR, NULL);
							latest.pid = kpi.kp_proc.p_pid;
						}
						else
						if ((start->tv_sec > latest.time.tv_sec) || ((start->tv_sec == latest.time.tv_sec) && (start->tv_usec > latest.time.tv_usec))) {
							latest.time.tv_sec = start->tv_sec;
							latest.time.tv_usec = start->tv_usec;
							free(latest.name);
							if ((latest.name = strdup(kpi.kp_proc.p_comm)) == NULL)
								err(EX_OSERR, NULL);
							latest.pid = kpi.kp_proc.p_pid;
						}
					}
				}
				else
				if (pattern != NULL) {
					if (exact) {
						if (ignorecase) {
							if (strcasecmp(name, pattern) == 0)
								pushProcList(proctoolslist, kp);
						}
						else {
							if (strcmp(name, pattern) == 0)
								pushProcList(proctoolslist, kp);
						}
					}
					else
						//if (regexec(&regex, name, 0, NULL, NULL) == 0)
						if (regexec(&regex, name, 0, NULL, 0) == 0)
							pushProcList(proctoolslist, kp);
				}
				else
					pushProcList(proctoolslist, kp);
				free(name);
			}
			else
			if ((!match && invert) || (match && !extmatch && invert)) {
				if (fullmatch) {
					if ((name = strdup(procArgs(baton, kp))) == NULL)
						err(EX_OSERR, NULL);
				}
				else {
					if ((name = strdup(kpi.kp_proc.p_comm)) == NULL)
						err(EX_OSERR, NULL);
				}
				if (lastonly) {
					struct timeval *start;
					int matched;
#if defined(USE_KVM)
					start = &kpi.kp_eproc.e_pstats.p_start;
#elif defined(p_starttime)
					start = &kpi.kp_proc.p_starttime;
#else
					errx(EX_SOFTWARE, "lastonly not supported for sysctl");
#endif
					if (pattern != NULL) {
						if (exact) {
							if (ignorecase)
								matched = !strcasecmp(name, pattern);
							else
								matched = !strcmp(name, pattern);
						}
						else {
							//matched = regexec(&regex, name, 0, NULL, NULL) == 0;
							matched = regexec(&regex, name, 0, NULL, 0) == 0;
						}
					}
					else {
						matched = TRUE;
					}

					if (matched) {
						if (!latest.valid) {
							latest.valid = TRUE;
							latest.kp = kp;
							latest.time.tv_sec = start->tv_sec;
							latest.time.tv_usec = start->tv_usec;
							if ((latest.name = strdup(kpi.kp_proc.p_comm)) == NULL)
								err(EX_OSERR, NULL);
							latest.pid = kpi.kp_proc.p_pid;
						}
						else
						if ((start->tv_sec > latest.time.tv_sec) || ((start->tv_sec == latest.time.tv_sec) && (start->tv_usec > latest.time.tv_usec))) {
							latest.time.tv_sec = start->tv_sec;
							latest.time.tv_usec = start->tv_usec;
							free(latest.name);
							if ((latest.name = strdup(kpi.kp_proc.p_comm)) == NULL)
								err(EX_OSERR, NULL);
							latest.pid = kpi.kp_proc.p_pid;
						}
						pushProcList(proctoolslist, kp);
					}
				}
				else
				if (pattern != NULL) {
					if (exact) {
						if (ignorecase) {
							if (strcasecmp(name, pattern) == 0)
								pushProcList(proctoolslist, kp);
						}
						else {
							if (strcmp(name, pattern) == 0)
								pushProcList(proctoolslist, kp);
						}
					}
					else
						//if (regexec(&regex, name, 0, NULL, NULL) == REG_NOMATCH)
						if (regexec(&regex, name, 0, NULL, 0) == REG_NOMATCH)
							pushProcList(proctoolslist, kp);
				}
				else
					pushProcList(proctoolslist, kp);
				free(name);
			}
		}

		if (lastonly && latest.valid) {
			if (invert) {
				popProcList(proctoolslist, latest.pid);
			}
			else {
				pushProcListDetails(proctoolslist, latest.kp, latest.pid, latest.name);
			}
		}
	}

	return(baton);
}

void
printProcInfo(struct baton *baton, const struct proctoolslist *proctoolslist)
{
	int termwidth = -1;
	switch (baton->type) {
		case PROCTOOLS_BATON_UNDUMPED:
			fmt_puts((char *)proctoolslist->kp->kp_proc.p_wmesg, &termwidth); /* we hijacked it!! */
			if (strcmp(cmdpart((char *)proctoolslist->kp->kp_proc.p_wmesg), proctoolslist->kp->kp_proc.p_comm)) {
				putchar(' ');
				putchar('(');
				fmt_puts(proctoolslist->kp->kp_proc.p_comm, &termwidth);
				putchar(')');
			}
			break;
		case PROCTOOLS_BATON_NATIVE:
			printProcInfoNative(baton, proctoolslist);
			break;
		default:
			errx(EX_SOFTWARE, "unrecognised baton type");
			break;
	}
}

/*
 * name is only used if other methods fail
 */
static void
printProcInfoNative(struct baton *baton, const struct proctoolslist *proctoolslist)
{
#if defined(USE_KVM)
	kvm_t *kd = baton->kd;
	struct kinfo_proc *ki;
	int nproc;
	if (kd == NULL) {
		printf("(null)");
	}
	ki = kvm_getprocs(kd, KERN_PROC_PID, proctoolslist->pid, &nproc);
	if (nproc == 1 && ki != NULL) {
		fmt_argv(kd, ki);
	}
#else
#if defined(KERN_PROCARGS2)
	size_t len = baton->argmax;

	*baton->pid = (int)proctoolslist->pid;
	if (sysctl(baton->mib, baton->miblen, baton->args, &len, NULL, 0) == -1) {
		printf("? (%s)", proctoolslist->name);
		return;
	}
	else {
		int argc = *(int*)baton->args;
		char *p = baton->args + sizeof(int); /* ick */
		char *end = baton->args + len;
		int termwidth = -1;
		char *save = p;

		p += strlen(p); /* skip exec path */
		for (; p < end && argc; p++) {
			if (*p == '\0') { /* skip NULs */
				if ((p + 1) < end && *(p + 1) != '\0') {
					break;
				}
			}
		}

		if (*(p + 1) != '\0' && ((p + 1) < end) && !strcmp(cmdpart(save), cmdpart(p + 1))) {
			save = NULL;
		}

		for (; p < end && argc; p++) {
			if (*p == '\0') {
				if ((p+1) < end) {
					argc--;
					fmt_puts(p + 1, &termwidth);
					if (argc)
						putchar(' ');
				}
			}
		}

		if (save) {
			putchar(' ');
			putchar('(');
			fmt_puts(save, &termwidth);
			putchar(')');
		}
	}
#else
	#warning no KVM or sysctl known for proc arguments on this platform
	printf("?");
#endif
#endif
}

void
freeProcList(struct baton *baton)
{
	if (baton->type == PROCTOOLS_BATON_NATIVE) {
#if defined(USE_KVM)
		(void)kvm_close(baton->kd);
#else
		free(baton->kp);
#endif
	}
	free(baton->args);
	free(baton);
}

/*
 * takes a string and returns the signal number associated with it
 */
int
signameToSignum(sig)
	char *sig;
{
	int n;

	if (!strncasecmp(sig, "sig", 3))
		sig += 3;
	for (n = 1; n < NSIG; n++) {
		if (!strcasecmp(sys_signame[n], sig))
			return (n);
	}
	return (-1);
}

/*
 * usage-style warning for signal names
 */
void
printSignals(fp)
	FILE *fp;
{
	int n;

	for (n = 1; n < NSIG; n++) {
		(void)fprintf(fp, "%s", sys_signame[n]);
		if (n == (NSIG / 2) || n == (NSIG - 1))
			(void)fprintf(fp, "\n");
		else
			(void)fprintf(fp, " ");
	}
}

ssize_t dumpProcHeader(fd)
	int fd;
{
	return write(fd, DUMPSIGNATURE, strlen(DUMPSIGNATURE)+1);
}

#define DUMPOUT(X) do {                                         \
	for (j = 0, i = (DUMPBUFSZ-1)<<3; j < DUMPBUFSZ; j++, i-=8) \
		dbuf[j] = ((X)>>i)&0xFF;                                \
	result = write(fd, dbuf, DUMPBUFSZ);                        \
	if (result == -1 || result != DUMPBUFSZ)                    \
		return result;                                          \
} while (0)

ssize_t
dumpProc(baton, fd, proctoolslist)
	struct baton *baton;
	int fd;
	const struct proctoolslist *proctoolslist;
{
	size_t s;
	int i, j;
	ssize_t result;
	const char *args;
	/* for this dump version */
	DUMPOUT(proctoolslist->pid);
	s = strlen(proctoolslist->name);
	DUMPOUT(s);
	result = write(fd, proctoolslist->name, s);
	if (result == -1 || result != s)
		return result;
	DUMPOUT(proctoolslist->kp->kp_eproc.e_ppid);
	DUMPOUT(proctoolslist->kp->kp_eproc.e_pgid);
	DUMPOUT(proctoolslist->kp->kp_eproc.e_ucred.cr_gid);
	DUMPOUT(proctoolslist->kp->kp_eproc.e_pcred.p_ruid);
	DUMPOUT(proctoolslist->kp->kp_eproc.e_pcred.p_svuid);
	DUMPOUT(proctoolslist->kp->kp_eproc.e_tdev);
	args = getProcArgs(baton, proctoolslist);
	s = strlen(args);
	DUMPOUT(s);
	result = write(fd, args, s);
	return result;
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
 * Portions of this code taken from:
 *   $OpenBSD: ps.c,v 1.19 2001/04/17 21:12:07 millert Exp $
 *   $NetBSD: ps.c,v 1.15 1995/05/18 20:33:25 mycroft Exp $
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
