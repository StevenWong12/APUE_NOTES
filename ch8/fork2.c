#include "apue.h"
#include <sys/wait.h>

int main(void){
    pid_t pid;
    if((pid = fork()) < 0)
        err_sys("fork error");
    else if (pid == 0){
        if( (pid = fork()) < 0)
            err_sys("fork error");
        else if (pid > 0){
            printf("first child, pid = %ld\n", (long)getpid());
            exit(0);
        }

        sleep(5);
        printf("second child, parent pid = %ld\n", (long)getppid());   
        exit(0);
    }
    if(waitpid(pid, NULL, 0)!= pid)
        err_sys("waitpid error");
    printf("parent, pid = %ld\n", (long)getpid());
    exit(0);
}