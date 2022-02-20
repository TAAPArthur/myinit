#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <unistd.h>

static int shuttingDown;
static pid_t pid;

void killChildren(int i) {
    if(i == SIGINT || i == SIGQUIT)
        shuttingDown = 1;
    kill(-pid, SIGINT);
    kill(0, SIGTERM);
}

int main(int argc, char*argv[]) {
    struct sigaction ign_action  = {SIG_IGN};
    // Don't die when killing process group
    sigaction(SIGTERM, &ign_action, NULL);
    struct sigaction action  = {killChildren};
    // shutdown and kill child
    sigaction(SIGINT, &action, NULL);
    // kill child; effectively restarts the process
    sigaction(SIGHUP, &action, NULL);
    const char* pidFile = argv[1];
    const char* cmd = argv[2];
    int fd = open(pidFile, O_WRONLY|O_CREAT);
    if(fd < 0) {
        perror("Could not open pidfile");
        exit(2);
    }
    if(flock(fd, LOCK_EX | LOCK_NB) == -1) {
        perror("Could not take lock");
        exit(2);
    }
    if(ftruncate(fd, 0) != 0){
        perror("Could not truncate file");
        exit(2);
    }
    dprintf(fd, "%d\n", getpid());

    while(!shuttingDown) {
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
    return 0;
}
