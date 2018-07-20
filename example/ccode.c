#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern int	func1(char*);
extern size_t	func2(int, char*, size_t);
char	buf[1024];

int
main()
{
    char	*fname;
    int		fd, i, j;
    size_t	totw, wsz;

    fname = "tdata";
    printf("start\n");
    for (i = 0; i < 4; i++) {
	fd = func1(fname);
	wsz = 1024;
	for (j = 0; j < 10; j++) {
	    totw = func2(fd, buf, wsz);
	    if ((int)wsz < 0) {
		printf("Cannot write data after writing %ld MB\n", totw);
	    }
	    usleep(10000);
	}
	sleep(1);
	close(fd);
    }
    return 0;
}
