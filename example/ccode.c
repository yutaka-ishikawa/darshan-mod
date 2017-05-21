#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char	buf[1024];

int
main()
{
    char	*fname;
    int		fd, i;
    size_t	totw, wsz;

    fname = "tdata";
    printf("start\n");
    for (i = 0; i < 4; i++) {
	if ((fd = open(fname, O_CREAT|O_WRONLY, 0644)) < 0) {
	    if ((fd = open(fname, O_WRONLY)) < 0) {
		fprintf(stderr, "Cannot open file %s\n", fname);
		exit(-1);
	    }
	} else {
	    printf("open: fd=%d\n", fd);
	}
	wsz = 1024;
	for (i = 0; i < 100; i++) {
	    totw = write(fd, buf, wsz);
	    if (wsz < 0) {
		printf("Cannot write data after writing %ld MB\n", totw);
	    }
	    usleep(10000);
	}
	sleep(1);
	close(fd);
    }
    return 0;
}
