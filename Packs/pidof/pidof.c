#import <sys/sysctl.h>
#import <unistd.h>
#import <stdio.h>
#import <stdlib.h>
#import <pwd.h>
#import <syslog.h>

#import <string.h>
#import <signal.h>

#define YES 1
#define NO  0
#define BOOL int

char *ProgName=NULL;

void printProcs()
{
	struct kinfo_proc *kp;
	int mib[4],nentries,i;
	size_t bufSize=0;
	uid_t user;
	struct passwd *pw;

	mib[0]=CTL_KERN;
	mib[1]=KERN_PROC;
	mib[2]=KERN_PROC_ALL;
	mib[3]=0;

	if (sysctl(mib,4,NULL,&bufSize,NULL,0) < 0) {
	openlog(ProgName, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
	syslog(LOG_ERR, "Error gettting processes!");
	closelog();
	return;
	}

	kp=(struct kinfo_proc *)malloc(bufSize);
	if (sysctl(mib,4,kp,&bufSize,NULL,0) < 0) {
	openlog(ProgName, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
	syslog(LOG_ERR, "Error getting processes!");
	closelog();
	return;
	}

	nentries=bufSize/sizeof(struct kinfo_proc);
	if (nentries == 0) {
	openlog(ProgName, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
	syslog(LOG_ERR, "Error getting processes!");
	closelog();
	return;
	}

	printf("%10.10s\t%6s\t%.16s\n","USER","PID","COMMAND");

	for (i=nentries; --i >= 0;) {
	user=(uid_t)((&(&kp[i])->kp_eproc)->e_ucred.cr_uid);
	pw=getpwuid(user);
	printf("%10.10s\t%6d\t%.16s\n",(pw != 0L) ? pw->pw_name : "UNKNOWN", ((&(&kp[i])->kp_proc)->p_pid),((&(&kp[i])->kp_proc)->p_comm));
	}
}

void pidof(BOOL terse, const char *argv)
{
	struct kinfo_proc *kp;
	int mib[4],nentries,i,output=0;
	size_t bufSize=0;

	mib[0]=CTL_KERN;
	mib[1]=KERN_PROC;
	mib[2]=KERN_PROC_ALL;
	mib[3]=0;

	if (sysctl(mib,4,NULL,&bufSize,NULL,0) < 0) {
	openlog(ProgName, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
	syslog(LOG_ERR, "Error getting processes!");
	closelog();
	return;
	}

	kp=(struct kinfo_proc *)malloc(bufSize);
	if (sysctl(mib,4,kp,&bufSize,NULL,0) < 0) {
	openlog(ProgName, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
	syslog(LOG_ERR, "Error getting processes!");
	closelog();
	return;
	}

	nentries=bufSize/sizeof(struct kinfo_proc);
	if (nentries == 0) {
	openlog(ProgName, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
	syslog(LOG_ERR, "Error getting processes!");
	closelog();
	return;
	}

	for (i=nentries; --i >= 0;) {
	char *proc=((&(&kp[i])->kp_proc)->p_comm);
	if (strcasestr(proc, argv) != NULL) {
		output=1;
		if (!terse) {
		uid_t user=(uid_t)((&(&kp[i])->kp_eproc)->e_ucred.cr_uid);
		struct passwd *pw=getpwuid(user);
		printf("PID for %s is %d (%s)\n",proc,((&(&kp[i])->kp_proc)->p_pid),(pw != 0L) ? pw->pw_name : "UNKNOWN");
	   } else
		printf("%d ",((&(&kp[i])->kp_proc)->p_pid));
	}
	}
	if (terse && output) {
	printf("\n");
	}
}

void killProc(const char *argv)
{
	struct kinfo_proc *kp;
	int mib[4],nentries,i;
	size_t bufSize=0;

	mib[0]=CTL_KERN;
	mib[1]=KERN_PROC;
	mib[2]=KERN_PROC_ALL;
	mib[3]=0;

	if (sysctl(mib,4,NULL,&bufSize,NULL,0) < 0) {
	openlog(ProgName, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
	syslog(LOG_ERR, "Error getting processes!");
	closelog();
	return;
	}

	kp=(struct kinfo_proc *)malloc(bufSize);
	if (sysctl(mib,4,kp,&bufSize,NULL,0) < 0) {
	openlog(ProgName, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
	syslog(LOG_ERR, "Error getting processes!");
	closelog();
	return;
	}

	nentries=bufSize/sizeof(struct kinfo_proc);
	if (nentries == 0) {
	openlog(ProgName, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
	syslog(LOG_ERR, "Error getting processes!");
	closelog();
	return;
	}

	for (i=nentries; --i >= 0;) {
	char *proc=((&(&kp[i])->kp_proc)->p_comm);
	if (strcasestr(proc, argv) != NULL) {
		if (kill(((&(&kp[i])->kp_proc)->p_pid),15) == 0) {
		printf("Process %d (%s) killed\n",((&(&kp[i])->kp_proc)->p_pid),proc);
		} else {
		printf("Process %d (%s) NOT killed\n",((&(&kp[i])->kp_proc)->p_pid),proc);
		}
	}
	}
}

int main(int argc, char *argv[])
{
	int c,i;
	BOOL terse=YES, kill=NO;

	ProgName=argv[0];

	if (argc == 1) {
	printProcs();
	return 0;
	}

	while ((c=getopt(argc, argv, "vklh?")) != EOF) {
	switch (c) {
		case 'v':
		printf("%s version 0.1.4\n\tCopyright 2003 - 2004 Night Productions\n\n",ProgName);
		return 0;
		break;
		case 'h':
		case '?':
		printf("Help:\n");
		printf("\t-k\tKill processes for given pid name\n");
		printf("\t  \t  (Note: You must have sufficient privileges!)\n");
		printf("\t-l\tList long output\n");
		printf("\t-h -?\tThis help screen\n");
		printf("\t-v\tPrint out the version info\n");
		return 0;
		break;
		case 'l':
		terse=NO;
		break;
		case 'k':
		kill=YES;
		break;
		default:
		printf("USAGE: %s [-klvh] [PID name]\n",ProgName);
		return 1;
		break;
	}
	}

	for (i=optind; i < argc; i++) {
	if (!kill)
		pidof(terse,argv[i]);
	else
		killProc(argv[i]);
	}

	return 0;
}
