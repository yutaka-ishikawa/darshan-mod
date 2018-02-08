/*
 *  (C) 2015 by RIKEN AICS
 */
#include "darshan-runtime-config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <zlib.h>
#include <assert.h>
#include <search.h>
#include <arpa/inet.h>
#include "darshan.h"
#include "darshan-dynamic.h"

struct darshan_history_header *darshan_hheader;
#ifdef HISTORY_DEBUG
static void print_history(int rank, int count, struct darshan_history_data *hdata);
#endif /* HISTORY DEBUG */

#ifdef WORDS_BIGENDIAN
#define swap32(fd) (fd)
#define swap64(dd) (dd)
#else
/* little endian */
static float swap32(float fd)
{
    union uu {
	float		fd;
	uint32_t	ud;
    } uu;
    uu.fd = fd;
    uu.ud = ((uu.ud << 24) & 0xff000000) | ((uu.ud <<  8) & 0x00ff0000)
	  | ((uu.ud >>  8) & 0x0000ff00) | ((uu.ud >> 24) & 0x000000ff);
    return uu.fd;
}
static double swap64(double dd)
{
    union uu {
	double		dd;
	uint64_t	ud;
    } uu;
    uu.dd = dd;
    uu.ud = ((uu.ud << 56) & 0xff00000000000000)
	  | ((uu.ud << 40) & 0x00ff000000000000)
	  | ((uu.ud << 24) & 0x0000ff0000000000)
	  | ((uu.ud <<  8) & 0x000000ff00000000)
          | ((uu.ud >>  8) & 0x00000000ff000000)
	  | ((uu.ud >> 24) & 0x0000000000ff0000)
	  | ((uu.ud >> 40) & 0x000000000000ff00)
	  | ((uu.ud >> 56) & 0x00000000000000ff);
    return uu.dd;
}
#endif /* WORDS_BIGENDIAN */


static unsigned long long d2ull(double dd)
{
    union uu {
	double		dd;
	uint64_t	ud;
    } uu;
    uu.dd = dd;
    return uu.ud;
}

void histcopy(struct darshan_history_data **dhpp, int *nentp, int total_ent,
	      struct histent *hep)
{
    struct darshan_history_data *dhp = *dhpp;
    int		nent = *nentp;

    if (hep == 0) return;
    while (hep) {
	if (nent > total_ent) break;
	memcpy(dhp, hep->hist_ent,
	       sizeof(struct darshan_history_data)*hep->hist_offset);
	nent += hep->hist_offset;
	dhp += hep->hist_offset;
	hep = hep->hist_next;
    }
    *nentp = nent;
    *dhpp = dhp;
}

static double
darshan_history_normalize_wtime(double offset, double start)
{
    double	dt;
    dt = (start > offset) ? (start - offset) : start;
    // printf("start(%f) offset(%f), return(%f)\n", start, offset, dt);
    return swap64(dt);
}

#ifdef HISTORY_DEBUG
static void print_history(int rank, int count, struct darshan_history_data *hdata)
{
    int		i;
    float	timesec = 0.0;

    for (i = 0; i < count; i++) {
	timesec += swap32(hdata[i].diff_sec);
	printf("<%d> %f, %f, %f\n", rank, timesec,
	       swap32(hdata[i].time_sec),  swap32(hdata[i].size));
    }
}
#endif /* HISTORY_DEBUG */

void
darshan_history_construct_indices(struct darshan_job_runtime* final_job, 
		  int rank, int* inout_count, int* lengths, void** pointers)
{
    int		i;
    uint32_t	szw, szr;
    int		nfiles;
    int		nent, total_ent;
    size_t		dhdsz, dhfsz, dhhsz;
    struct darshan_history_header *dhhp;
    struct darshan_history_file *dhfp;
    struct darshan_history_data *dhdp;

    if(final_job->file_count == 0) return;
    nfiles = 0; nent = 0; total_ent = 0;
#ifdef HISTORY_DEBUG
    printf("<%d> darhsan_history_construct_indcies: final_job->file_count(%d)\n", rank, final_job->file_count);
    for (i = 0; i < CP_MAX_FILES; i++) {
	int j;
	for (j = 0; j < final_job->file_count; j++) {
	    if (final_job->file_runtime_array[i].log_file == &final_job->file_array[j]) {
		printf("<%d> file_array[%d] = file_runtime_array[%d]\n",
		       rank, j, i);
	    }
	}
    }
#endif /* HISTORY_DEBUG */
    /* max size of history file */
//  dhfsz = sizeof(struct darshan_history_file) * final_job->file_count;
    dhfsz = sizeof(struct darshan_history_file) * CP_MAX_FILES;
    dhfp = (struct darshan_history_file*) malloc(dhfsz);
    if (dhfp == NULL) {
	printf("<%d>darshan: cannot allocate memory\n", rank);
	goto exit_history;
    }
    memset(dhfp, 0, dhfsz);
    for (i = 0; i < final_job->file_count; i++) {
	szw = final_job->file_runtime_array[i].hist_w.hist_total;
	szr = final_job->file_runtime_array[i].hist_r.hist_total;
	if (szw == 0 && szr == 0) continue;
	/* file name */
	strcpy(dhfp[nfiles].hfile_name,
	       final_job->file_array[i].name_suffix);
	/* open time stamp relative to MPI_Init */
	dhfp[nfiles].hfile_open
	    = swap64(CP_F_VALUE(&final_job->file_runtime_array[i],
				CP_F_OPEN_TIMESTAMP));
	/* close time stamp relative to MPI_Init */
	dhfp[nfiles].hfile_close
	    = swap64(CP_F_VALUE(&final_job->file_runtime_array[i],
				CP_F_CLOSE_TIMESTAMP));
	/* read start time stamp relative to MPI_Init */
	dhfp[nfiles].hfile_rstart
	    = darshan_history_normalize_wtime(final_job->wtime_offset,
					      final_job->file_runtime_array[i].hist_r.hist_start);
	/* write start time stamp relative to MPI_Init */
	dhfp[nfiles].hfile_wstart
	    = darshan_history_normalize_wtime(final_job->wtime_offset, 
					      final_job->file_runtime_array[i].hist_w.hist_start);
	dhfp[nfiles].hfile_read = htonl(szr);
	dhfp[nfiles].hfile_write = htonl(szw);
	dhfp[nfiles].hfile_nopen = 0;
	/* number of times in open and fopen */
	dhfp[nfiles].hfile_nopen
		= htonl(CP_VALUE(&final_job->file_runtime_array[i],
				 CP_POSIX_OPENS));
	dhfp[nfiles].hfile_nfopen
		= htonl(CP_VALUE(&final_job->file_runtime_array[i],
				 CP_POSIX_FOPENS));
	/* file size at close time */
	dhfp[nfiles].hfile_size
		= htonl(final_job->file_runtime_array[i].fsize);
#ifdef HISTORY_DEBUG
	printf(" i(%d) open(%f) close(%f)\n", i,
	       swap64(dhfp[nfiles].hfile_open),
	       swap64(dhfp[nfiles].hfile_close));
#endif /* HISTORY_DEBUG */
	total_ent += szr + szw;
	nfiles++;
    }
#ifdef HISTORY_DEBUG
    printf("<%d> total_ent = %d nfiles = %d\n", rank, total_ent, nfiles);
#endif /* HISTORY_DEBUG */
    if (nfiles == 0) {
	free(dhfp);
	goto exit_history;
    }
    /* real size of history file */
    dhfsz = SIZE_HISTORY_FILE * nfiles;
    dhdsz = SIZE_HISTORY_DATA * total_ent;
    dhhsz = SIZE_HISTORY_HEADER + dhfsz + dhdsz;
    dhhp = darshan_hheader = (struct darshan_history_header*) malloc(dhhsz);
    if (dhhp == NULL) {
	free(dhfp);
	printf("<%d> darshan: cannot allocate memory\n", rank);
	goto exit_history;
    }
    /* history header */
    strncpy(dhhp->magic, HISTMAGIC_STRING, HISTMAGIC_SIZE);
    dhhp->nfiles = htonl(nfiles);
    dhhp->rank = htonl(rank);
    /* copy history file */
    memcpy((char*) dhhp + SIZE_HISTORY_HEADER, dhfp, dhfsz);
    free(dhfp);
    /* the start address of history data */
    dhdp = (struct darshan_history_data*)
	((char*) dhhp + SIZE_HISTORY_HEADER + dhfsz);
    memset(dhdp, 0, dhdsz);
    nent = 0;
    for (i = 0; i < final_job->file_count; i++) {
	histcopy(&dhdp, &nent, total_ent,
		 final_job->file_runtime_array[i].hist_r.hist_head);
	histcopy(&dhdp, &nent, total_ent,
		 final_job->file_runtime_array[i].hist_w.hist_head);
    }
#ifdef HISTORY_DEBUG
    print_history(rank, total_ent, (struct darshan_history_data*)
		  ((char*) dhhp + SIZE_HISTORY_HEADER + dhfsz));
#endif /* HISTORY_DEBUG */
    if (total_ent != nent) {
	printf("<%d> Internal error in darshan while writing a log total_ent(%d) nent(%d)\n", rank, total_ent, nent);
	goto exit_history;
    }
    lengths[*inout_count] = dhhsz;
    pointers[*inout_count] = darshan_hheader;
    (*inout_count)++;
#ifdef HISTORY_DEBUG
    printf("*inout_count = %d\n", *inout_count);
#endif /* HISTORY_DEBUG */
exit_history:
    return;
}


