#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <unistd.h>

static int shuttingDown;
void killChildren(int i) {
    if(i == SIGINT)
        shuttingDown = 1;
    kill(0, SIGTERM);
}

void main(int argc, char*argv[]) {
    struct sigaction ign_action  = {SIG_IGN};
    struct sigaction action  = {killChildren};
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);
    sigaction(SIGTERM, &ign_action, NULL);
    sigaction(SIGUSR1, &action, NULL);
    const char* pidFile = argv[1];
    const char* cmd = argv[2];
    int fd = open(pidFile, O_WRONLY|O_CREAT|O_TRUNC);
    dprintf(fd, "%d\n", getpid());
    if(flock(fd, LOCK_EX | LOCK_NB) == -1) {
        perror("Could not take lock");
        exit(2);
    }

    while(!shuttingDown) {
        pid_t pid;
        if(!(pid=fork())) {
            execvp(cmd, argv+2);
            exit(1);
        }
        printf("Spawned %s (%d)\n", cmd, pid);
        int status;
        while(waitpid(pid, &status, 0) == -1){
            if(errno != EINTR)
                break;
        }
        if(WIFEXITED(status)) {
            int exitStatus = WEXITSTATUS(status);
            printf("Command %s (%d) exited with status %d\n", cmd, pid, exitStatus);
            if(exitStatus == 127)
                break;
        }
        sleep(1);
    }
    printf("Service stopped\n");
    unlink(pidFile);
    close(fd);
}
