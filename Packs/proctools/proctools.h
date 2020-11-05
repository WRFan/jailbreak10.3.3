#ifndef _PROCTOOLS_H_
#define _PROCTOOLS_H_

#define VERSION "0.4pre1"

#include <sys/cdefs.h>
#include <stdio.h>
#include <kvm.h>

#if !defined(USE_KVM) && !defined(USE_SYSCTL)
#	if defined(KVM_NO_FILES)
#		define USE_KVM
#		define USE_KVM_NO_FILES
#	elif defined(KERN_PROCARGS2)
#		define USE_SYSCTL
#	else
#		define USE_KVM
/* without USE_KVM_NO_FILES */
#	endif
#endif

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef __dead
#ifdef __dead2
#define __dead __dead2
#else
#define __dead
#endif
#endif

struct proctoolslist {
		struct	proctoolslist *next;
		struct  kinfo_proc *kp;
		char	*name;
		pid_t	pid;
};

struct uidlist {
	struct	uidlist *next;
	uid_t	uid;
};

struct pidlist {
	struct	pidlist *next;
	pid_t	pid;
};

struct grouplist {
	struct	grouplist *next;
	gid_t	group;
};

struct termlist {
	struct	termlist *next;
	dev_t	term;
};

typedef enum _baton_type {
	PROCTOOLS_BATON_NATIVE,
	PROCTOOLS_BATON_UNDUMPED
} baton_type;

struct baton {
	baton_type		 type;
	int				 argmax;
	char			*args;
#if defined(USE_KVM)
	kvm_t			*kd;
#else
	struct kinfo_proc	*kp;
#	if defined(KERN_PROCARGS2)
	int				 mib[4];
	int				 miblen;
	int				*pid;
#	endif
#endif
};

#define DUMPBUFSZ 8

#define DUMPSIGNATURE "\366proctools\001"

__BEGIN_DECLS
struct baton	*undumpProcList(const char *, struct proctoolslist **);
struct baton	*getProcList(const char *, struct proctoolslist **,
					struct uidlist *, struct uidlist *, struct grouplist *, struct pidlist *,
					struct pidlist *, struct termlist *, int, int, int, int, int, char *);
void			 freeProcList(struct baton *);

void			 printProcInfo(struct baton *, const struct proctoolslist *proctoolslist);
const char		*getProcArgs(struct baton *, const struct proctoolslist *);

int				 signameToSignum(char *);
void			 printSignals(FILE *);

int				 parsePidList(char *, struct pidlist **);
int				 parseUidList(char *, struct uidlist **);
int				 parseGroupList(char *, struct grouplist **);
int				 parseTermList(char *, struct termlist **);
void			 printPidList(FILE *f, struct pidlist *, char *);
void			 printUidList(FILE *f, struct uidlist *, char *);
void			 printGroupList(FILE *f, struct grouplist *, char *);
void			 printTermList(FILE *f, struct termlist *, char *);
int				 matchUidList(struct uidlist *, uid_t);
int				 matchGroupList(struct grouplist *, gid_t);
int				 matchPidList(struct pidlist *, pid_t);
int				 matchTermList(struct termlist *, dev_t);

ssize_t			 dumpProcHeader(int);
ssize_t			 dumpProc(struct baton *, int, const struct proctoolslist *);
__END_DECLS

#endif /* !_PROCTOOLS_H_ */
