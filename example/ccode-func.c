#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int
func1(char *fname)
{
    int		fd;
    if ((fd = open(fname, O_CREAT|O_WRONLY, 0644)) < 0) {
	if ((fd = open(fname, O_WRONLY)) < 0) {
	    fprintf(stderr, "Cannot open file %s\n", fname);
	    exit(-1);
	}
    } else {
	printf("open: fd=%d\n", fd);
    }
    return fd;
}

size_t
func2(int fd, char *buf, size_t wsz)
{
    size_t	sz;
    printf("func2 is called\n");
    if ((int) (sz = write(fd, buf, wsz)) < 0) {
	fprintf(stderr, "Cannot write data\n");
	exit(-1);
    }
    return sz;
}
