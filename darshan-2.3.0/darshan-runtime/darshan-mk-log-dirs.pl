#!/usr/bin/perl -w
#
#  (C) 2009 by Argonne National Laboratory.
#      See COPYRIGHT in top-level directory.
#

use File::Basename;

# creates a hierarchy of subdirectories for darshan to place log files in
# LOGDIR/<year>/<month>/<day>/

# use log dir specified at configure time
$LOGDIR = "./darshan";


my $year = (localtime)[5] + 1900;
my $month;
my $day;
my $i;
my $j;
my $k;

umask(0);

# go through the end of next year
for ($i=$year; $i<($year+2); $i++)
{
    mkdir("$LOGDIR/$i", 0755) or die("Error: could not mkdir $LOGDIR/$i.\n");
    for ($j=1; $j<13; $j++)
    {
        mkdir("$LOGDIR/$i/$j", 0755) or die("Error: could not mkdir $LOGDIR/$i/$j.\n");
        for ($k=1; $k<32; $k++)
        {
            mkdir("$LOGDIR/$i/$j/$k", 01777) or die("Error: could not mkdir $LOGDIR/$i/$j/$k.\n");

        }
    }
}


