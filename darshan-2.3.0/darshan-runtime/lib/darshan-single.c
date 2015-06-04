#define _XOPEN_SOURCE 500

#include "darshan-runtime-config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <time.h>
#include <pthread.h>
#include <zlib.h>
#include "darshan.h"
#define __USE_GNU
#include <dlfcn.h>
#include <linux/limits.h>
#include <errno.h>

#define CP_MAX_MEM_SEGMENTS 8 /* comes from darshan-mpi-io.c */

extern void darshan_history_construct_indices(struct darshan_job_runtime* final_job, int rank, int* inout_count, int* lengths, void** pointers);
extern void darshan_history_stdio_init();

struct darshan_history_header	*darshan_hheader;
double		(*__real_PMPI_Wtime)(void);
static int	single_init_f = 0;
static char	cmdline[CP_EXE_LEN + 1];
static char	**argv;
static char	*cmdname;
static pid_t	pid;

static int (*real_open)(const char *pathname, int flags, mode_t mode);
static int (*real_close)(int fd);
static ssize_t (*real_write)(int fd, const void *buf, size_t count);
static ssize_t (*real_read)(int fd, const void *buf, size_t count);

static void	darhsan_single_exit(void);
static int	cp_log_compress(struct darshan_job_runtime* final_job,
		    int rank, int* inout_count, int* lengths, void** pointers);

/*
 * timestamp in seconds 
 */
static double
darshan_single_wtime()
{
    struct timespec	tp;
    double		dt;

    clock_gettime(CLOCK_REALTIME, &tp);
    dt = (double) tp.tv_sec;
    dt += ((double)tp.tv_nsec)/1000000000.0; /* second */
    return dt;
}

void
darshan_single_init()
{
    int		fd, nr, i, j;
    char	*cp;
    int		argc = 0;
    char	bufpath[1024];

    if (single_init_f == 1) {
	return;
    }
    single_init_f = 1;
#ifdef HISTORY_DEBUG
    printf("darshan_single_init: invoked\n");
#endif /* HISTORY_DEBUG */
    atexit(darhsan_single_exit);
    __real_PMPI_Wtime = darshan_single_wtime;

    real_open = (int (*)(const char*, int, mode_t)) dlsym(RTLD_NEXT, "open");
    real_close = (int (*)(int)) dlsym(RTLD_NEXT, "close");
    real_write = (ssize_t (*)(int, const void*, size_t)) dlsym(RTLD_NEXT, "write");
    real_read = (ssize_t (*)(int, const void*, size_t)) dlsym(RTLD_NEXT, "read");

    pid = getpid();
    snprintf(bufpath, 1024, "/proc/%d/cmdline", pid);
    fd = real_open(bufpath, O_RDONLY, 0);
    if (fd < 0) goto skip;
    nr = real_read(fd, cmdline, CP_EXE_LEN);
    real_close(fd);
    for (i = 0; i < nr; i++) {
	if (cmdline[i] == 0) argc++;
    }
    argv = (char**) malloc(sizeof(char*) * (argc + 1));
    if (argv == 0) goto skip;
    memset(argv, 0, sizeof(char*) * (argc + 1));
    argv[0] = cmdline;
    for (i = 0, j = 0, cp = cmdline; i < nr; i++) {
	if (cmdline[i] == 0) {
	    argv[j] = cp;
	    cp = &cmdline[i] + 1;
	    j++;
	}
    }
    /* find command name e.g., ./cmd path1/path2/cmd */
    cmdname = cp = argv[0];
    while ((cp = index(cp, '/')) != NULL) {
	cmdname = ++cp;
    }
skip:
    /* record exe and arguments */
    darshan_initialize(argc, argv, 1, 0);

    /* stdin stdout stderr entries are created at this time */
    darshan_history_stdio_init();

    return;
}

char*
darshan_get_exe_and_mounts(struct darshan_job_runtime* final_job)
{
    return 0;
}

void
darshan_mnt_id_from_path(const char* path, int64_t* device_id, int64_t* block_size)
{
    return;
}

/*
 * copied from darshan_shutdown() in darshan-mpi-io.c
 */
static void
darhsan_single_exit()
{
    struct darshan_job_runtime* final_job;
    char			*logfile_name;
    char			cuser[L_cuserid] = {0};
    int				index_count;
    int				fd, ret;
    int				lengths[CP_MAX_MEM_SEGMENTS];
    void			*pointers[CP_MAX_MEM_SEGMENTS];
    char            hostname[1024];

    /* get host name for log file */
    if (gethostname(hostname, sizeof(hostname)) < 0) {
        fprintf(stderr, "error: obtaining hostname\n");
        return;
    }

    CP_LOCK();
    if(!darshan_global_job)
    {
        CP_UNLOCK();
        return;
    }
    /* closing time is set to stdin/stdout/stderr */
    darshan_history_stdio_exit();
    /* disable further tracing while hanging onto the data so that we can
     * write it out
     */
    final_job = darshan_global_job;
    darshan_global_job = NULL;
    CP_UNLOCK();

    //
    // /* figure out which access sizes to log */
    // darshan_walk_file_accesses(final_job);
    //

    /* if the records have been condensed, then zero out fields that are no
     * longer valid for safety 
     */
    if(final_job->flags & CP_FLAG_CONDENSED && final_job->file_count)
    {
        CP_SET(&final_job->file_runtime_array[0], CP_MODE, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_CONSEC_READS, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_CONSEC_WRITES, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_SEQ_READS, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_SEQ_WRITES, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_STRIDE1_STRIDE, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_STRIDE2_STRIDE, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_STRIDE3_STRIDE, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_STRIDE4_STRIDE, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_STRIDE1_COUNT, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_STRIDE2_COUNT, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_STRIDE3_COUNT, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_STRIDE4_COUNT, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_ACCESS1_ACCESS, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_ACCESS2_ACCESS, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_ACCESS3_ACCESS, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_ACCESS4_ACCESS, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_ACCESS1_COUNT, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_ACCESS2_COUNT, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_ACCESS3_COUNT, 0);
        CP_SET(&final_job->file_runtime_array[0], CP_ACCESS4_COUNT, 0);
        
        CP_F_SET(&final_job->file_runtime_array[0], CP_F_OPEN_TIMESTAMP, 0);
        CP_F_SET(&final_job->file_runtime_array[0], CP_F_CLOSE_TIMESTAMP, 0);
        CP_F_SET(&final_job->file_runtime_array[0], CP_F_READ_START_TIMESTAMP, 0);
        CP_F_SET(&final_job->file_runtime_array[0], CP_F_READ_END_TIMESTAMP, 0);
        CP_F_SET(&final_job->file_runtime_array[0], CP_F_WRITE_START_TIMESTAMP, 0);
        CP_F_SET(&final_job->file_runtime_array[0], CP_F_WRITE_END_TIMESTAMP, 0);
    }
    logfile_name = malloc(PATH_MAX);
    if(!logfile_name) {
        darshan_finalize(final_job);
        return;
    }
    cuserid(cuser);
    if (strcmp(cuser, "") == 0) {
	uid_t uid = geteuid();
	snprintf(cuser, sizeof(cuser), "%u", uid);
    }
    /*
     * darshanlog-username-pid
     */
    snprintf(logfile_name, PATH_MAX, "%s%sdarshanlog-%s-%ld-%s-%u.gz", 
        getenv("DARSHAN_SINGLE_LOG_DIR") ? getenv("DARSHAN_SINGLE_LOG_DIR") : "",
        getenv("DARSHAN_SINGLE_LOG_DIR") ? "/" : "",
        hostname, final_job->log_job.start_time, cmdname, pid);
    final_job->log_job.end_time = time(NULL);

    /* collect data to write from local process */
    index_count = 0;
    lengths[index_count] = sizeof(final_job->log_job);
    pointers[index_count] = &final_job->log_job;
    index_count++;
    /* also string containing exe command line */
    lengths[index_count] = CP_EXE_LEN + 1; 
    pointers[index_count] = final_job->trailing_data;
    index_count++;

    if(final_job->file_count > 0)
    {
        lengths[index_count] = final_job->file_count*CP_FILE_RECORD_SIZE;
        pointers[index_count] = final_job->file_array;
	index_count++;
    }
#ifdef HISTORY_DEBUG
    printf("final_job->file_count = %d\n", final_job->file_count);
#endif /* HISTORY_DEBUG */
    darshan_history_construct_indices(final_job, 0, &index_count, lengths, 
				      pointers);
    /* compress data */
#ifdef HISTORY_DEBUG
    printf("index_count = %d\n", index_count);
#endif /* HISTORY_DEBUG */
    ret = cp_log_compress(final_job, 0, &index_count, lengths, pointers);
    if (ret != 0) {
	/* error return */
    }

    /* writting log */
#ifdef HISTORY_DEBUG
    printf("lengths=%d pointers[0] = %p\n", lengths[0], pointers[0]);
#endif /* HISTORY_DEBUG */
    if (getenv("DARSHAN_SINGLE_LOG_DIR")) {
        if (mkdir(getenv("DARSHAN_SINGLE_LOG_DIR"), 0777) < 0 && errno != EEXIST) {
            fprintf(stderr, "darshan: cannot create directory %s\n", 
                    getenv("DARSHAN_SINGLE_LOG_DIR"));
            goto err_ret;
        }
    }
    fd = real_open(logfile_name, O_CREAT|O_WRONLY, 0644);
    if (fd < 0) {
	fprintf(stderr, "darshan: cannot create %s\n", logfile_name);
	goto err_ret;
    }
    ret = real_write(fd, pointers[0], lengths[0]);
    if (ret != lengths[0]) {
	fprintf(stderr, "darshan: cannot write history data to %s\n", logfile_name);
    }
    real_close(fd);
err_ret:
    if(final_job->trailing_data)
        free(final_job->trailing_data);
    free(logfile_name);
    darshan_finalize(final_job);
    return;
}


/*
 * copied from darshan-mpi-io.c
 */
static int cp_log_compress(struct darshan_job_runtime* final_job,
    int rank, int* inout_count, int* lengths, void** pointers)
{
    int ret = 0;
    z_stream tmp_stream;
    int total_target = 0;
    int i;
    int no_data_flag = 1;

    /* do we actually have anything to write? */
    for(i=0; i<*inout_count; i++)
    {
        if(lengths[i])
        {
            no_data_flag = 0;
            break;
        }
    }

    if(no_data_flag)
    {
        /* nothing to compress */
        *inout_count = 0;
        return(0);
    }

    memset(&tmp_stream, 0, sizeof(tmp_stream));
    tmp_stream.zalloc = Z_NULL;
    tmp_stream.zfree = Z_NULL;
    tmp_stream.opaque = Z_NULL;

    ret = deflateInit2(&tmp_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
        31, 8, Z_DEFAULT_STRATEGY);
    if(ret != Z_OK)
    {
        return(-1);
    }

    tmp_stream.next_out = (void*)final_job->comp_buf;
    tmp_stream.avail_out = CP_COMP_BUF_SIZE;

    /* loop through all pointers to be compressed */
    for(i=0; i<*inout_count; i++)
    {
        total_target += lengths[i];
        tmp_stream.next_in = pointers[i];
        tmp_stream.avail_in = lengths[i];
        /* while we have not finished consuming all of the data available to
         * this point in the loop
         */
        while(tmp_stream.total_in < total_target)
        {
            if(tmp_stream.avail_out == 0)
            {
                /* We ran out of buffer space for compression.  In theory,
                 * we could start using some of the file_array buffer space
                 * without having to malloc again.  In practice, this case 
                 * is going to be practically impossible to hit.
                 */
                deflateEnd(&tmp_stream);
                return(-1);
            }

            /* compress data */
            ret = deflate(&tmp_stream, Z_NO_FLUSH);
            if(ret != Z_OK)
            {
                deflateEnd(&tmp_stream);
                return(-1);
            }
        }
    }
    
    /* flush compression and end */
    ret = deflate(&tmp_stream, Z_FINISH);
    if(ret != Z_STREAM_END)
    {
        deflateEnd(&tmp_stream);
        return(-1);
    }
    deflateEnd(&tmp_stream);

    /* substitute our new buffer */
    pointers[0] = final_job->comp_buf;
    lengths[0] = tmp_stream.total_out;
    *inout_count = 1;

    return(0);
}
