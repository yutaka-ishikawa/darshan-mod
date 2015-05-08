#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//#define TCOUNT	3
//#define SCOUNT	5
//#define TCOUNT	2
//#define SCOUNT	10
#define TCOUNT	3
#define SCOUNT	10
#define BSIZE	1024*1024
#define WSIZE	100

int	nprocs, myrank;
char	buf[BSIZE];

int
open_file(int flags, char *fname, int rank, char *sfx, int times)
{
    int		fd;
    char	nbuff[1024];
    
    sprintf(nbuff, "%s%d-%s-%d", fname, myrank, sfx, times);
    fd = open(nbuff, flags, 0644);
    return fd;
}

int
main(int argc, char **argv)
{
    int		fd1, fd2;
    ssize_t	wsz, wtot;
    int		cnd, i, step, times;
    char	*fname;

    if (argc < 2) {
	fprintf(stderr, "%s <file name>\n", argv[0]);
	exit(-1);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    printf("MPI_Wtime = %f\n", MPI_Wtime());
    wsz = wtot = 0;
    memset(buf, 0, BSIZE);
    fname = argv[1];
    fd1 = open_file(O_CREAT|O_WRONLY, argv[1], myrank, "a", 0);
    if (fd1 < 0) {
	fprintf(stderr, "Cannot create file %s\n", fname);
	cnd = -1; goto ext;
    }
    close(fd1);
    for (times = 0; times < TCOUNT; times++) {
	fd1 = open_file(O_APPEND|O_WRONLY, fname, myrank, "a", 0);
	fd2 = open_file(O_CREAT|O_WRONLY, fname, myrank, "b", times);
	if (fd2 < 0) {
	    fprintf(stderr, "Cannot create file %s\n", fname);
	    cnd = -1; goto ext;
	}
	for (step = 1; step <= SCOUNT; step++) {
	    for (i = 0; i < 3; i++) {
		wsz = write(fd1, buf, WSIZE*step);
		if (wsz < 0) {
		    printf("Cannot write data after writing %ld MB\n", wtot);
		    close(fd1); close(fd2);
		    cnd = -1; goto ext;
		}
		wsz = write(fd2, buf, WSIZE*step);
		if (wsz < 0) {
		    printf("Cannot write data after writing %ld MB\n", wtot);
		    close(fd1); close(fd2);
		    cnd = -1; goto ext;
		}
		wtot += wsz;
	    }
	    usleep(10000);
	}
	close(fd1); close(fd2);
	sleep(2);
    }
    printf("Writing %ld MB to %s\n", wtot, fname);
    cnd = 0;
ext:
    MPI_Finalize();
    return cnd;
}
