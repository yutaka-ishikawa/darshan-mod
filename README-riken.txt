Extended Darshan (Based on 2.3.0 release)
					      January 04, 2019
					      System Software Research Team
					      System Software Development Team
					       			   RIKEN R-CCS
					      			   Fujitsu Ltd.
Darshan is developed at Argonne National Laboratory.  We extend it to
recording file I/O operation history.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Just Try it
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
The example directory is located under the directory of this file.
For the K computer users, type the following commands to see
how the darshan works:
  $ cd example
  $ make
  $ pjsub run-k.sh

After finishing the job execution, issue the following commands:
$ cd darshan/2015/*/*
$ ls -l
You will see a file something like
    a03228_simple_id29390_2-24-29839-4689494154855117309_1.darshan.gz

Execute the darshan-parser utility as follows:
$ darshan-parser --history a03228_simple_id29390_2-24-29839-4689494154855117309_1.darshan.gz
You will see the log of read/write operations issued by the simple.c program.


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 How to Use Darshan for MPI applications
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
The darshan hooks system calls to record file I/O operations.  If your
binary program has been created with the dynamically linked option (in
default), you do not need anything for the binary.

1) Create a directory for log files
In this darshan binary distribution for Co-design of Post K, the log
directory is "../darshan/" in the working directory.  The darshan will
create a log file under the "../darshan/MM/DD/" directory where MM and
DD are month and day, respectively.  Before executing an application,
the directory must be created.  For example, if an application runs on
17th of April, 2015, the following directory must exist.
	../darshan/2015/4/17		# in K
	./darshan/2015/4/17		# in x86 Linux
Here is an example to create such a directory.
    $ mkdir -p ../darshan/`date +%Y/%m/%d | sed -e "s/\/0/\//g"`

2) You must specify the darshan library as a dynamic link library. In
order to enable recording history of read and write operations, the
environment variable DARSHAN_HISTORY_RW must be specified.  Here is an
example where the dynamic link library is located in the current
working directory.
    $ ( export LD_PRELOAD=./libdarshan.so; export DARSHAN_HISTORY_RW="rw"; \
        <your binary> )

3) After the execution, a log file appears under the directory created
in step 1). Here is an example:
	ishikawa_simple_id8732_4-16-40056-6523797983477487085_1.darshan.gz

4) There are several utilities provided by darshan.  The
darshan-parser is the basic utility to extract information of a log
file. To extract read and write operation histories, the --history
option is introduced. e.g.,
    $ darshan-parser --history ishikawa_simple_id8732_4-16-40056-6523797983477487085_1.darshan.gz

The darshan-parser is included in the K distribution.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 How to Use Darshan for Non MPI applications
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
The darshan is extended to non MPI applications.  For logging file I/O
operations, the darshan-single.so shared library is preloaded.
For example,
    $ ( export LD_PRELOAD=./libdarshan-single.so; \
	export DARSHAN_HISTORY_RW="rw"; \
	gcc -O2 test.c )
After the execution, you will get something like the following files:
   darshanlog-ishikawa-cc1-1716.gz
   darshanlog-ishikawa-collect2-1719.gz
The darshan captured two commands "cc1" and "collect2".
The format of log file name is
    darshanlog-<user name>-<command name>-<pid>.gz

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 How to see the log
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
1) darshan-job-summary.pl is used to look an overall log.
    $ darshan-job-summary.pl <log file>
   A pdf file is generated by the tool.

2) To look file I/O operations, darshan-history.pl is used
    $ darshan-history.pl <log file>
   The history_rank.png file is generated by the tool.
   If you want to get a pdf file, "--terminal" option is used as follows:
    $ darshan-history.pl --terminal "pdf" <log file>
   The history_rank.pdf file is generated.
   With --summary option, you will see summary of I/O operations.
    $ darshan-history.pl --summary <log file>

3) To extract the statistics of file I/O operations, darshan-fileio.pl is used
    $ darshan-fileio.pl <log file>

4) The darshan-parser, used by the above tools, is a basic utility to
   extract recorded information. You may directly use this tool.
    $ darshan-parser --history <log file>

   If the execution of the following sample program has been recorded:
	fp = fopen("file.txt, "w+"); fwrite(data, 1024, 10, fp);  fclose(fp);
	fp = fopen("file.txt, "w+"); fwrite(data, 1024, 100, fp); fclose(fp);
	fp = fopen("file.txt, "w+"); fwrite(data, 1024, 1, fp);   fclose(fp);
   The darshan-parser writes the following report:
     ....
     # <0> write xample/file.txt ntimes 3 fopen 0.000103 close 0.000745
     # size = 102400
     # start = 0.000105 count=3
     # time, elapsed, Kbyte, func name
     0.000000, 0.000027, 10.240000, try+0x35
     0.000438, 0.000101, 102.400002, try+0x35
     0.000605, 0.000001, 1.024000, try+0x35
     ....
   Though the sample program accesses the same file three times, i.e.,
   the file is opened three times, the darshan just records elapsed times
   of open and close of the last access.
   The maximum size of this file is reported, not the sizeof the last
   access.  That is, the size is 102400 byte, not 1024 byte.  The all
   write/read operations are recorded.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Additional Information
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Environment Variables extended by RIKENN AICS

-  DARSHAN_HISTORY_RW
	Enables recording read or/and write operations. No recording in default.
   e.g.
    export DARSHAN_HISTORY_RW="rw"  # for read write
    export DARSHAN_HISTORY_RW="r"   # for read only
    export DARSHAN_HISTORY_RW="w"   # for write only

-  DARSHAN_HISTORY_MEMSIZE
	Specifies the size of working memory area to record I/O operations.
	1MB (87,380 records) is in default.
   e.g.
    export DARSHAN_HISTORY_MEMSIZE=10485760	# 10MB
    export DARSHAN_HISTORY_MEMSIZE=104857600	# 100MB

- DARSHAN_HISTORY_FUNCALL_LEVEL
	Spefifies which level from the top of the calling stack is selected
	to record a function symbol. The dafult is 4.
- DARSHAN_WORK_MEMSIZE
	Speficies working memory area for compression. The user might not
	specify this option unless the compress ration is below 3/4.

Note:
  - The last 15 bytes of a file name is only saved. This is a darshan feature.

-------------------------------------------------------------------------------
HISTORY:
01/04/2019
- Fixing bugs for fputc, putc, etc.
- Some unlocked stream IO operations are now captured. Be sure that
  the following operations cannot be captured when the source code is
  compiled with -O or higher optimization levels.
  getc_unlocked, getchar_unlocked, putc_unlocked, putchar_unlocked,
  fputc_unlocked, fgetc _unlocked, and fread_unlocked
  Those functions are defined as macros for optimization.
  fwrite_unlocked might also cannot be captured.

10/09/2018
- The following stream functions are now captured.
  - fputc, fputs, putc, IO_putc, putchar, puts, fgetc, fgets, IO_getc,
    getchar, ungetc, gets, getline, getdelim,

- The "_exit" function was not captured. This affects that IO
  activities in processes created inside python are not reported
  because the darshan finalization function is not invoked. It is now
  fixed.
- If a process is created by the clone C library, the exit semantics
  is not the same semantics in a process invoked by fork system call.
  The former process does not call exit or _exit C function, but
  directly issues the exit system call. This affects that IO
  activities in this cloned process are not reported because the
  darshan finalization function is not invoked.  It is now fixed.
- The USE_TSC compile option is introduced in order to use the
  hardware time samp counter for elapsed time. The gettimeofday system
  call is time consumed operation against FILE stream operations
  without involving disk I/O in the K computer. This problem is not so
  critical problem in x86 and Arm architectures because the
  gettimeofday system such architectures is implemented using vSDO
  feature.


02/24/2018
-  The following bugs are fixed:
	- Though the file size field is 64 bit, htonl and ntohl were
	  used for byte order conversion.
	- The fclose operation was not taken care for recording file size.
02/08/2018
-  The actual file size at the close time is now recorded. Darshan accumulates
   bytes of read/write data. If an application moves the file position by
   the lseek system call followed by reading/writing data. The bytes of
   read/write data is not the same of the file size. You can get this
   information using darshan-history.pl:
	$ darshan-history.pl --summary  darshanlog-XXXXX.gz
-  The darshan-history.pl now generates pdf and other formats supported
   by gnuplot. The "--terminal" option changes the format.
   The witdh and hight of the terminal should be defined. The default size of
   pdf is 10cm x 10cm.
-  The following problems are fixed.
      - darshan-job-summary.pl assumed version 4.2 or later version of 4,
        not version 5 nor later.
      - darhsn-history.pl could not handle the latest log format.
      - The closing time of stdout/stderr was not recorded.
08/20/2017
-  The original darshan manages a fix amount of memory for working
   memory to compress a log.  The modified version dynamically reserves
   this working memory area, e.g., 3/4's logged data size.
   The users also specifies this amount of memory by setting the
   "DARSHAN_WORK_MEMSIZE" environment variable.
05/29/2017
 - The "-DHISTORY_CALLER" compile option is added.
   At logging read/write system call, its caller name is logged.
   The runtime libraries and tools must be compiled with
   "-DHISTORY_CALLER" option.
   Applications should be compiled with the "-g" option
   without "-O2" nor "-O3" option, and linked with the "-rdynamic" option.
   The "DARSHAN_HISTORY_FUNCALL_LEVEL" environment is introduced to select
   which symbol of function from the calling stack is recorded.
   The default is 4, showing the symbol in the 4th from the top of the calling
   stack.
05/21/2017
 - a bug fix reported by AXE is merged
 - the following bug is fixed
    if a log file is created with no "DARSHAN_HISTORY_RW" environment,
    the log file does not contain any history of file I/O.
    In this case, the darshan-parser does not read the last opened file.
 - darshan-fileio.pl is created to extract statistics of file I/O operations.
05/28/2015
 - timestamps of open/close/read/write system calls are relative
   to timestamp of MPI_Init.
 - libdarshan-single.so is availale for non MPI applications.
   timestamps of syste calls are related to timestamp of the first open
   system call.
 - Fujitsu provides the darshan-history.pl tool which shows file I/O history.
04/21/2015
 - First Release
	
-- Yutaka Ishikawa, yutaka.ishikawa@riken.jp
