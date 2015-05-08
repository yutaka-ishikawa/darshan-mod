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
#define TCOUNT	1000
#define SCOUNT	1000
#define BSIZE	1024
#define WSIZE	10

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
    int		fd1;
    ssize_t	rsz, rtot;
    int		cnd, step, times;
    char	*fname;

    if (argc < 2) {
	fprintf(stderr, "%s <file name>\n", argv[0]);
	exit(-1);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    rsz = rtot = 0;
    memset(buf, 0, BSIZE);
    fname = argv[1];
    fd1 = open_file(O_RDONLY, argv[1], myrank, "a", 0);
    if (fd1 < 0) {
	fprintf(stderr, "Cannot create file %s\n", fname);
	cnd = -1; goto ext;
    }
    for (times = 0; times < TCOUNT; times++) {
	for (step = 1; step <= SCOUNT; step++) {
	    rsz = read(fd1, buf, WSIZE);
	    if (rsz <= 0) break;
	    rtot += rsz;
	}
	usleep(1000);
    }
    close(fd1);
    printf("Reading %ld MB to %s\n", rtot/(1024*1024), fname);
    cnd = 0;
ext:
    MPI_Finalize();
    return cnd;
}
