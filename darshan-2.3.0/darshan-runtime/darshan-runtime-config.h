/* darshan-runtime-config.h.  Generated from darshan-runtime-config.h.in by configure.  */
/* darshan-runtime-config.h.in.  Generated from configure.in by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Name of the environment variable that stores the jobid */
#define CP_JOBID "PBS_JOBID"

/* Define if cuserid() should be disabled */
/* #undef DARSHAN_DISABLE_CUSERID */

/* Define if struct aiocb64 type is defined */
#define HAVE_AIOCB64 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `z' library (-lz). */
/* #undef HAVE_LIBZ */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <mntent.h> header file. */
#define HAVE_MNTENT_H 1

/* Define if MPI-IO prototypes use const qualifier */
#define HAVE_MPIIO_CONST 1

/* Define if off64_t type is defined */
#define HAVE_OFF64_T 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/mount.h> header file. */
#define HAVE_SYS_MOUNT_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "darshan-runtime"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "darshan-runtime 2.3.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "darshan-runtime"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.3.0"

/* Define if <inttypes.h> exists and defines unusable PRI* macros. */
/* #undef PRI_MACROS_BROKEN */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define if Darshan should set log files to be group readable */
/* #undef __CP_GROUP_READABLE_LOGS */

/* Comma separated list of env. variables to use for log path */
/* #undef __CP_LOG_ENV */

/* Comma-separated list of MPI-IO hints for log file write */
#define __CP_LOG_HINTS "romio_no_indep_rw=true;cb_nodes=4"

/* Location to store log files at run time */
#define __CP_LOG_PATH "./darshan"

/* Memory alignment in bytes */
#define __CP_MEM_ALIGNMENT 8

/* Define if Darshan should stat files at open time to collect extra
   statistics */
/* #undef __CP_STAT_AT_OPEN */

/* Generalized request type for MPI-IO */
#define __D_MPI_REQUEST MPIO_Request
