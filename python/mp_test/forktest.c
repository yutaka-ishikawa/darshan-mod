#define _GNU_SOURCE
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE);	\
    } while (0)

char	buf[128];

int
main(int argc, char *argv[])
{
    int	i;
    pid_t pid;

    pid = fork();
    if (pid == 0) {
	/* child */
        printf("child PID: %d\n", getpid());
	printf("> "); fflush(stdout);
	gets(buf);
	for (i = 0; i < 3; i++) {
	    printf("%d: Hello from child process\n", i);
	}
	return 0;
    } else if (pid == -1) {
	errExit("Cannot fork\n");
    }
    printf("fork() returned %ld\n", (long) pid); fflush(stdout);
    if (waitpid(pid, NULL, 0) == -1) {
	errExit("waitpid");
    }
    printf("child has terminated\n"); fflush(stdout);

    exit(0);
}

