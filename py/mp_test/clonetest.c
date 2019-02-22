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

void myfunc()
{
    printf("myfunc\n");
}

static int              /* Start function for cloned child */
childFunc(void *arg)
{
    int	i;

    printf("child PID: %d\n", getpid());
    for (i = 0; i < 3; i++) {
	printf("%d: Hello from child process\n", i);
    }
    printf("> "); fflush(stdout);
    gets(buf);
    if (buf[0] == 'e') {
	printf("calling exit function explicitly\n");
	exit(0);
    } else if (buf[0] == 'h') {
	printf("issuing hlt instruction\n");
	asm("hlt");
    } else if (buf[0] == 'a') {
	printf("aborting\n");
	*((char*)0) = 0;
    }
    myfunc();
    return 0;
}

#define STACK_SIZE (1024 * 1024)    /* Stack size for cloned child */

int
main(int argc, char *argv[])
{
    char *stack;                    /* Start of stack buffer */
    char *stackTop;                 /* End of stack buffer */
    pid_t pid;

    /* Allocate stack for child */
    stack = malloc(STACK_SIZE);
    if (stack == NULL)
	errExit("malloc");
    stackTop = stack + STACK_SIZE;  /* Assume stack grows downward */

    /* Create child that has its own UTS namespace;
       child commences execution in childFunc() */

    pid = clone(childFunc, stackTop, CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, 0);
    if (pid == -1)
	errExit("clone");

    printf("my pid(%d) clone() returned %ld\n", getpid(), (long) pid);
    if (waitpid(pid, NULL, 0) == -1)    /* Wait for child */
	errExit("waitpid");
    printf("child has terminated\n");

    // exit(0);
    return 0;
}

