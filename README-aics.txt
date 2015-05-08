Extended Darshan (Based on 2.3.0 release)
					      April 21, 2015
					      System Software Development Team
					       			    RIKEN AICS
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
 How to Use Darshan
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
    $ ( export LD_LIBRARY=./libdarshan.so; export DARSHAN_HISTORY_RW="rw"; \
        <your binary> )

3) After the execution, a log file appears under the directory created
in step 1). Here is an example:
	ishikawa_simple_id8732_4-16-40056-6523797983477487085_1.darshan.gz

4) There are several utilities provided by darshan.  The
darshan-parser is the basic utility to extract information of a log
file. To extract read and write operation histories, the --history
option is introduced. e.g.,
    $ darshan-parser --history ishikawa_simple_id8732_4-16-40056-6523797983477487085_1.darshan.gz

The darshan-parser is included in the K distribution, but the darshan-parser only works with --history option.

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

Note:
  - The last 15 bytes of a file name is only saved. This is a darshan feature.
  - We will follow the latest distribuion, 2.3.1, soon.

-------------------------------------------------------------------------------
			Written by Yutaka Ishikawa, yutaka.ishikawa@riken.jp
