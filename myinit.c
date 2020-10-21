/* See LICENSE file for copyright and license details. */
#include <sys/types.h>
#include <sys/wait.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LEN(x) (sizeof (x) / sizeof *(x))

static void spawn(char* const []);

static struct {
    int sig;
    char* const cmd[3];
} sigmap[] = {
    { SIGINT, { "/bin/rc.shutdown", "poweroff", NULL }},
    { SIGUSR1, {"/bin/rc.shutdown", "reboot", NULL }},
    { SIGUSR2, {"/bin/rc.shutdown", "soft-reboot", NULL }},
};
static char* const rcInitCmd[]     = { "/bin/rc.init", NULL };

static sigset_t set;

static void
spawn(char* const argv[]) {
    switch(fork()) {
        case 0:
            sigprocmask(SIG_UNBLOCK, &set, NULL);
            setsid();
            signal(SIGCHLD, SIG_DFL);
            execvp(argv[0], argv);
            perror("Could not execvp");
            _exit(1);
        case -1:
            perror("fork");
    }
}

int
main(void) {
    int sig;
    size_t i;
    sigfillset(&set);
    sigprocmask(SIG_BLOCK, &set, NULL);
    signal(SIGCHLD, SIG_IGN);
    spawn(rcInitCmd);
    while(1) {
        sigwait(&set, &sig);
        for(i = 0; i < LEN(sigmap); i++) {
            if(sigmap[i].sig == sig) {
                execvp(sigmap[i].cmd[0], sigmap[i].cmd);
                perror("Could not execvp");
            }
        }
    }
    /* not reachable */
    return 0;
}
