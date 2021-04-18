#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void die(const char*msg) {
    perror(msg);
    exit(1);
}
void usage(int exitStatus){
    printf("setgid [-h] groupname cmd\n");
    exit(exitStatus);
}
int main(int argc, char* argv[]) {
    if(argc < 3){
        usage(1);
    }
    if(argv[1][0] == '-' && argv[1][1] == 'h') {
        usage(0);
    }
    struct group * group = getgrnam(argv[1]);
    if(!group){
        die("Could not open group");
    }
    if(setgid(group->gr_gid) < 0) {
        die("Could set set gid");
    }
    execvp(argv[2], argv+2);
    die("failed exec");
}
