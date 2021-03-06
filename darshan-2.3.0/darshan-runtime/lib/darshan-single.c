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
#include <signal.h>

#define CP_MAX_MEM_SEGMENTS 8 /* comes from darshan-mpi-io.c */

extern void darshan_history_construct_indices(struct darshan_job_runtime* final_job, int rank, int* inout_count, int* lengths, void** pointers);
extern void darshan_history_stdio_init();
extern void darshan_history_stdio_exit();
extern void darshan_single_exit(void);

struct darshan_history_header	*darshan_hheader;
double		(*__real_PMPI_Wtime)(void);
static int	single_init_f = 0;
static char	cmdline[CP_EXE_LEN + 1];
static char	**argv;
static int	argc;
static char	*cmdname;
static pid_t	pid, ppid;

static int (*real_open)(const char *pathname, int flags, mode_t mode);
static int (*real_close)(int fd);
static ssize_t (*real_write)(int fd, const void *buf, size_t count);
static ssize_t (*real_read)(int fd, const void *buf, size_t count);

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

#ifdef HISTORY_DEBUG
void
darshan_single_last_string_msg(char *where, char *msg)
{
    char buf[256];
    sprintf(buf, "echo \"%s %s\" >>/tmp/dlog", where, msg);
    system(buf);
}

void
darshan_single_last_int_msg(char *where, unsigned long val)
{
    char buf[256];
    sprintf(buf, "echo \"%s 0x%x %d\" >>/tmp/dlog", where, val, val);
    system(buf);
}
#endif /* HISTORY_DEBUG */

/*
 * Processes in python's multiprocessing.Pool will receive SIGTERM
 */
static  struct sigaction oldsigaction;

static void
handle_sigterm(int signum, siginfo_t *info, void *p)
{
    /*printf("handle_sigterm is invoked\n");*/
    sigaction(SIGTERM, &oldsigaction, NULL);
    darshan_single_exit();
}


void
darshan_single_init()
{
    int		fd, nr, i, j;
    char	*cp;
    char	bufpath[1024];
    struct sigaction	newsigaction;

    if (single_init_f == 1) {
	return;
    }
    single_init_f = 1;

    newsigaction.sa_flags = SA_SIGINFO;
    sigemptyset(&newsigaction.sa_mask);
    newsigaction.sa_sigaction = handle_sigterm;
    sigaction(SIGTERM, &newsigaction, &oldsigaction);

#ifdef HISTORY_DEBUG
    printf("darshan_single_init: invoked\n");
#endif /* HISTORY_DEBUG */
    atexit(darshan_single_exit);
    __real_PMPI_Wtime = darshan_single_wtime;

    real_open = (int (*)(const char*, int, mode_t)) dlsym(RTLD_NEXT, "open");
    real_close = (int (*)(int)) dlsym(RTLD_NEXT, "close");
    real_write = (ssize_t (*)(int, const void*, size_t)) dlsym(RTLD_NEXT, "write");
    real_read = (ssize_t (*)(int, const void*, size_t)) dlsym(RTLD_NEXT, "read");

    ppid = pid = getpid();
    snprintf(bufpath, 1024, "/proc/%d/cmdline", pid);
    fd = real_open(bufpath, O_RDONLY, 0);
    if (fd < 0) goto skip;
    nr = real_read(fd, cmdline, CP_EXE_LEN);
    real_close(fd);
    argc = 0;
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
 * This is for Python
 */
static char	name[CP_EXE_LEN + 1];
void
darshan_single_reset(char *procname)
{
    static int	reseted = 0;

    /*
     * reset is needed just once it is called
     */
    CP_LOCK();
    if (reseted) {
	CP_UNLOCK();
	return;
    }
    reseted = 1;
    if(!darshan_global_job)
    {
        CP_UNLOCK();
        return;
    }
    /* simply darhsan_global_job is set to NULL */
    darshan_global_job = NULL;
    /* record exe and arguments */
    if (procname) {
	strncpy(name, procname, CP_EXE_LEN);
    } else {
	name[0] = 0;
    }
    cmdname = argv[0] = name;
    darshan_initialize(argc, argv, 1, 0);
    /* stdin stdout stderr entries are created at this time */
    darshan_history_stdio_init();
    CP_UNLOCK();
}

/*
 * copied from darshan_shutdown() in darshan-mpi-io.c
 *	Be sure that debug messages cannot be written
 *	if stderr has been closed before the exit system call.
 *	An example is the cp command.
 */
void
darshan_single_exit()
{
    struct darshan_job_runtime* final_job;
    char			*logfile_name;
    char			cuser[L_cuserid] = {0};
    int				index_count;
    int				fd, ret;
    int				lengths[CP_MAX_MEM_SEGMENTS];
    void			*pointers[CP_MAX_MEM_SEGMENTS];
    char            hostname[1024];

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
//    fprintf(stderr, "YI: darshan_single_exit: 0.1\n");

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
//    fprintf(stderr, "YI: darshan_single_exit: 1.1 logfile_name=%p\n", logfile_name);
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
    /* refresh pid if this is a cloned process, pid has been changed */
    pid = getpid();
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
    printf("dashan_single_exit: final_job->file_count = %d\n", final_job->file_count);
#endif /* HISTORY_DEBUG */
    /* 
     * Iterates through counters and adjusts timestamps to be relative to
     * the first open function
     */
    {
	int	i, j;
	for(i = 0; i < final_job->file_count; i++)
	    for(j=CP_F_OPEN_TIMESTAMP; j<=CP_F_WRITE_END_TIMESTAMP; j++) {
		if(final_job->file_array[i].fcounters[j] > final_job->wtime_offset)
		    final_job->file_array[i].fcounters[j] -= final_job->wtime_offset;
	    }
    }
    darshan_history_construct_indices(final_job, 0, &index_count, lengths, 
				      pointers);
    /* compress data */
#ifdef HISTORY_DEBUG
    printf("index_count = %d\n", index_count);
#endif /* HISTORY_DEBUG */
    ret = cp_log_compress(final_job, 0, &index_count, lengths, pointers);
    if (ret != 0) {
	fprintf(stderr, "Cannot compress\n");
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
    int		ret = 0;
    z_stream	tmp_stream;
    int		total_target = 0;
    int		i;
    int		no_data_flag = 1;
    int		len;
    char	*sp;
    void	*compbuf;

    /* do we actually have anything to write? */
    for (i = 0; i < *inout_count; i++) {
        if (lengths[i]) {
            no_data_flag = 0;
            break;
        }
    }

    if(no_data_flag) {
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
    if(ret != Z_OK) {
        return(-1);
    }

#define CP_REQSIZE(sz) (((sz)*3)/4)
    sp = getenv("DARSHAN_WORK_MEMSIZE");
    if (sp && ((len = atol(sp)) > CP_COMP_BUF_SIZE)) {
	/* nothing */
    } else {
	len = 0;
	for (i = 0; i < *inout_count; i++) {
	    len += lengths[i];
	}
    }
    if (CP_REQSIZE(len) > CP_COMP_BUF_SIZE) {
	tmp_stream.next_out = (void*) malloc(CP_REQSIZE(len));
	if (tmp_stream.next_out == 0) goto usestatic;
	tmp_stream.avail_out = CP_REQSIZE(len);
    } else {
    usestatic:
        tmp_stream.next_out = (void*)final_job->comp_buf;
	tmp_stream.avail_out = CP_COMP_BUF_SIZE;
    }
    compbuf = tmp_stream.next_out;

#ifdef HISTORY_DEBUG2
    printf("cp_log_compress: len = %f Kibyte (CP_COMP_BUF_SIZE=%f Kibyte)\n", (float)len/1024.0, (float)CP_COMP_BUF_SIZE/1024.0);
#endif

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
    pointers[0] = compbuf;
    lengths[0] = tmp_stream.total_out;
    *inout_count = 1;

    return(0);
}
