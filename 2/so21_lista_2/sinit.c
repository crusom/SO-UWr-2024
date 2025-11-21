/* See LICENSE file for copyright and license details. */
#include <sys/types.h>
#include <sys/wait.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LEN(x)	(sizeof (x) / sizeof *(x))
#define TIMEO	30

static void sigpoweroff(void);
static void sigreap(void);
static void sigreboot(void);
static void spawn(char *const []);

static struct {
	int sig;
	void (*handler)(void);
} sigmap[] = {
	{ SIGUSR1, sigpoweroff },
	{ SIGCHLD, sigreap     },
	{ SIGALRM, sigreap     },
	{ SIGINT,  sigreboot   },
};

#include "config.h"

static sigset_t set;

int
main(void)
{
	int sig;
	size_t i;

    // it should be init right
	if (getpid() != 1)
		return 1;
	chdir("/");
    // 'sigfillset' initializes a signal set to contain all signals.
	sigfillset(&set);
    // sigprocmask — examine and change blocked signals
    // SIG_BLOCK The set of blocked signals is the union of the current set and the set argument.
    // it blocks all signals, to then use sigwait, so that signals are handled synchronously.
	sigprocmask(SIG_BLOCK, &set, NULL);
	spawn(rcinitcmd);
	while (1) {
        // alarm() arranges for a SIGALRM signal to be delivered to the calling process in seconds seconds.
		alarm(TIMEO);
		// The  sigwait()  function  suspends  execution  of the calling thread until one of the signals specified in the signal set set becomes pending.  
        // For a signal to become pending, it must first be blocked with sigprocmask(2).
        // The function accepts the signal (removes it from the pending list of signals), and returns the signal number in sig.
        sigwait(&set, &sig);
        // itaretes over possible signals and if it is defined, call the handler
		for (i = 0; i < LEN(sigmap); i++) {
			if (sigmap[i].sig == sig) {
				sigmap[i].handler();
				break;
			}
		}
	}
	/* not reachable */
	return 0;
}

// self-explaining
static void
sigpoweroff(void)
{
	spawn(rcpoweroffcmd);
}

// well, if we got an orphan who died then we need to reap him
static void
sigreap(void)
{
	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
	alarm(TIMEO);
}
// self-explaining
static void
sigreboot(void)
{
	spawn(rcrebootcmd);
}

static void
spawn(char *const argv[])
{
	switch (fork()) {
	case 0:
        // "The signals in set are removed from the current set of blocked signals.  It is permissible to attempt to unblock a signal which is not blocked."
        // well, the program we are executing should not use the parents handlers.
		sigprocmask(SIG_UNBLOCK, &set, NULL);
        // "setsid()  creates  a  new session if the calling process is not a process group leader.
        // The calling process is the leader of the new session (i.e., its session ID is made the same as its process ID).  
        // The calling process also becomes the process group leader of a new process group in the session (i.e., its process group ID is made the same as its process ID)."
		setsid();
        // run the program
        // remember that exec shouldn't return
		execvp(argv[0], argv);
		perror("execvp");
		_exit(1);
	case -1:
		perror("fork");
	}
}
