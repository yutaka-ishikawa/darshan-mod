#!/usr/bin/perl -w
#
#

# Set via configure
my $PREFIX="/usr/local";

#use lib "/usr/local/lib";
#use Encode;
use File::Temp qw/ tempdir /;
use File::Basename;
use Cwd;
use Getopt::Long;
use English;
use POSIX qw(strftime);

my $vflag = 0;
my $input_file = "";
my $darshan_parser = "$PREFIX/bin/darshan-parser";

init();
process_args();

my $orig_dir = getcwd;
my $tmp_dir = tempdir( CLEANUP => !$verbose_flag );
if ($verbose_flag) {
    print "verbose: $tmp_dir\n";
}

system "$darshan_parser $input_file > $tmp_dir/history.dat";

open(TRACE, "< $tmp_dir/history.dat") || die("Can't execute \"$darshan_parser $input_file\": $!\n");

while ($line = <TRACE>) {
    chomp($line);
    if ($line =~ /^\s*$/) {
        # ignore blank lines
    } elsif ($line =~ /^#/) {
	# ignore
    } else {
	@data = split(/\t/, $line);
	if (grep {$data[2]} @sysname) {
	    printf("%s, %s, %s\n", $data[4], $data[2], $data[3]);
	}
#	printf("%s\n", $data[2]);
#	    print $line . "\n";
    }
}

close(TRACE) || die "darshan-parser failure: $! $?";

sub print_help
{
    print <<EOF;
Usage: $PROGRAM_NAME <options> input_file
    --help      Prints this help message
    --verbose   verbose mode
EOF
    return;
}

sub process_args
{
    use vars qw( $opt_help $verbose);

    Getopt::Long::Configure("no_ignore_case", "bundling");
    GetOptions("help",	"verbose");
    if ($opt_help) {
	print_help();
	exit(0);
    }
    if ($verbose) {
	$vflag = 1;
    }
    if($#ARGV != 0)
    {
        print_help();
        exit(1);
    }
    $input_file = $ARGV[0];
}

sub init
{
@sysname = ('CP_POSIX_OPENS',
'CP_POSIX_SEEKS',
'CP_POSIX_READS',
'CP_POSIX_WRITES',
'CP_POSIX_SEEKS',
'CP_POSIX_MMAPS',
'CP_POSIX_FOPENS',
'CP_POSIX_FSEEKS',
'CP_POSIX_FREADS',
'CP_POSIX_FWRITES',
'CP_POSIX_FSYNCS',
'CP_POSIX_FDSYNCS',
'CP_SIZE_READ_0_100',
'CP_SIZE_READ_100_1K',
'CP_SIZE_READ_1K_10K',
'CP_SIZE_READ_10K_100K',
'CP_SIZE_READ_100K_1M',
'CP_SIZE_READ_1M_4M',
'CP_SIZE_READ_4M_10M',
'CP_SIZE_READ_10M_100M',
'CP_SIZE_READ_100M_1G',
'CP_SIZE_READ_1G_PLUS',
'CP_SIZE_WRITE_0_100',
'CP_SIZE_WRITE_100_1K',
'CP_SIZE_WRITE_1K_10K',
'CP_SIZE_WRITE_10K_100K',
'CP_SIZE_WRITE_100K_1M',
'CP_SIZE_WRITE_1M_4M',
'CP_SIZE_WRITE_4M_10M',
'CP_SIZE_WRITE_10M_100M',
'CP_SIZE_WRITE_100M_1G',
'CP_SIZE_WRITE_1G_PLUS');
}
