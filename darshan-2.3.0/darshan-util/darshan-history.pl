#!/usr/bin/perl -w

# Set via configure
my $PREFIX="/usr/local";

use lib "/usr/local/lib";
use Encode;
use File::Temp qw/ tempdir /;
use File::Basename;
use Cwd;
use Getopt::Long;
use English;
use POSIX qw(strftime);


# system commands used

my $darshan_parser = "$PREFIX/bin/darshan-parser";

my $pdflatex       = "pdflatex";
my $mv             = "mv";
my $gnuplot;

# Prefer gnuplot installed with darshan, if present.
if(-x "$PREFIX/bin/gnuplot")
{
    $gnuplot       = "$PREFIX/bin/gnuplot";
}
else
{
    $gnuplot       = "gnuplot";
}

my $plot_interval = 0.01;
my $th_time = $plot_interval;
my $verbose_flag = 0;
my $plot_from = -0.1;
my $plot_to = 0;
my $input_file = "";
my $output_file = "history_rank.png";
my $width = 640;
my $hight = 480;
my $plot_start = "*";
my $plot_end = "*";
my $plot_data = "\"file-access-rank_read.dat\" using 1:2 with lines lw 1 lt 1 title \"read\", \"file-access-rank_write.dat\" using 1:2 with lines lw 1 lt 2  title \"write\", \"file-access-rank_open.dat\" using 1:2 with points pt 1  title \"open\", \"file-access-rank_close.dat\" using 1:2 with points pt 2  title \"close\"";
my $plot_caption = "Timespan for read/write access on files";
my $plot_ytics = "";
my $print_summary = 0;

process_args();

check_prereqs();

my $orig_dir = getcwd;
my $tmp_dir = tempdir( CLEANUP => !$verbose_flag );
if ($verbose_flag) {
    print "verbose: $tmp_dir\n";
}

system "$darshan_parser --history $input_file > $tmp_dir/history.dat";

my $minimum_start_time = get_minimum_start_time();

#open(TRACE, "$darshan_parser --history $input_file |") || die("Can't execute \"$darshan_parser $input_file\": $!\n");
open(TRACE, "< $tmp_dir/history.dat") || die("Can't execute \"$darshan_parser $input_file\": $!\n");
open(TMP_RANK_R, ">$tmp_dir/file-access-rank_read.dat") || die("error opening output file: $!\n");
open(TMP_RANK_W, ">$tmp_dir/file-access-rank_write.dat") || die("error opening output file: $!\n");
open(TMP_RANK_O, ">$tmp_dir/file-access-rank_open.dat") || die("error opening output file: $!\n");
open(TMP_RANK_C, ">$tmp_dir/file-access-rank_close.dat") || die("error opening output file: $!\n");

my $current_rank = 0;
my $current_vec = "";
my $current_file = "";
my $io_sum = 0;
my $open_time = 0;
my $close_time = 0;
my $notfirst = 0;
my $is_stream = 0;

if ($print_summary) {
    print "#rank,filename,\"read|write\",size,\"stream|fd\",open,close,start\n";
}
while ($line = <TRACE>) {
    chomp($line);

    if ($line =~ /^\s*$/) {
        # ignore blank lines
    }
    elsif ($line =~ /^#/) {
	if ($line =~ /^# start_time: ([\d\.]+)/) {
##	    $jobstart_time = $1;
	} elsif ($line =~ /^# nprocs: /) {
	    $nprocs = (split(':', $line, 2))[1];
	    if($plot_to == 0) {
		$plot_to = int($nprocs)-0.5;
	    }
	} elsif ($line =~ /^# start = ([\d\.]+)/) {
#YI	    $diff_since_first_io = $1 - $minimum_start_time;
	    $diff_since_first_io = $1;
	} elsif ($line =~ /^# <([0-9]+)>/) {
	    if ($notfirst == 1) {
		print_iosummary();
	    }
	    $io_sum = 0;
	    $notfirst = 1;
	    # <rank> read/write file ntimes <n> open/fopen <n> close <n>
	    $current_rank = $1;
	    ($current_vec, $current_file) = (split(/[\t ]+/, $line))[2..3];
	    $current_file =~ s/\//-/g; # the last 15 bytes of a file name only
 	    # not used in this release $ntimes = (split(/[\t ]+/, $line))[5];
	    if ((split(/[\t ]+/, $line))[6] =~ "fopen") {
		$is_stream = 1;
	    }
 	    $open_time = (split(/[\t ]+/, $line))[7];
##	    $open_time -= $jobstart_time;
 	    $close_time = (split(/[\t ]+/, $line))[9];
##	    $close_time -= $jobstart_time;
	    print TMP_RANK_O "$open_time\t$current_rank\t0\n";
	    print TMP_RANK_C "$close_time\t$current_rank\t0\n";
	    #close(TMP);
	    #open(TMP, ">$tmp_dir/file-access-io-${current_rank}_${current_vec}_${current_file}.dat") || die("error opening output file: $!\n");
	    $th_time = $plot_interval;
	}
    }
    else{
	# time, elapsed, Kbyte
#	($start_time, $elapsed, $io_size) = (split(/[\t ]+/, $line))[0..2];
	($start_time, $elapsed, $io_size) = (split(/[\t ]+/, $line))[0..2];
	$io_sum += $io_size;
	chop($start_time);
	chop($elapsed);
	$start_time += $diff_since_first_io;
	$end_time = $start_time + $elapsed;

	if($current_vec eq "read") {
	    print TMP_RANK_R "$start_time\t$current_rank\t0\n$end_time\t$current_rank\t0\n\n\n";
	} else {
	    print TMP_RANK_W "$start_time\t$current_rank\t0\n$end_time\t$current_rank\t0\n\n\n";
	}
	### $start_time += $elapsed;

	#if($end_time > $th_time) {
	    #print TMP "$th_time, $io_sum\n";
	#    $th_time += $plot_interval;
	#    while($th_time < $end_time) {
		#print TMP "$th_time, 0\n";
	#	$th_time += $plot_interval;
	#    }
	#    $io_sum = 0;
	#} else {
	#   $io_sum += $io_size;
	#}
    }
}
if ($notfirst == 1) {
    print_iosummary();
}
close(TMP_RANK_R);
close(TMP_RANK_W);
close(TMP_RANK_O);
close(TMP_RANK_C);
#close(TMP);
close(TRACE) || die "darshan-parser failure: $! $?";

make_conf();

chdir $tmp_dir;
system "$gnuplot rank.conf";
chdir $orig_dir;
system "$mv $tmp_dir/history_rank.png $output_file";
###system "cp $tmp_dir/*.dat .";

exit 0;

sub print_iosummary
{
    if ($print_summary == 0) { return; }
    if ($is_stream) {
	$otype = "stream";
    } else {
	$otype = "fd";
    }
    print "$current_rank,$current_file,$current_vec,$io_sum,$otype,$open_time,$close_time,$diff_since_first_io\n";
#    print "rank: $current_rank file: $current_file op: $current_vec $io_sum KB, $otype $open_time, $close_time\n";
}

sub make_conf
{
if(abs($plot_to - $plot_from) < 5)
{
    $plot_ytics = 1;
}

    open(RANKCONF, ">$tmp_dir/rank.conf") || die("error tmp file: $!\n");
    print RANKCONF "#!/usr/bin/gnuplot -persist
set terminal png
set terminal png size $width, $hight
set output \"history_rank.png\"
set ylabel \"rank\"
set xlabel \"seconds\"
set timefmt \"%s\"
set xrange [$plot_start:$plot_end]
set yrange [$plot_from:$plot_to]
set title \"$plot_caption\"

set ytics $plot_ytics
plot $plot_data
"
;
    close(RANKCONF);
}

sub process_args
{
    use vars qw( $opt_help $opt_output $opt_verbose $opt_from_rank $opt_to_rank $opt_start_time $opt_end_time $opt_caption $opt_nocaption $opt_summary);

    Getopt::Long::Configure("no_ignore_case", "bundling");
    GetOptions( "help",
		"output=s",
		"verbose",
		"from_rank=i",
		"to_rank=i",
                "width=i",
                "hight=i",
		"start_time=s",
		"end_time=s",
		"plot=s",
		"caption=s",
		"nocaption",
		"summary"
	);

    if($opt_help)
    {
	print_help();
	exit(0);
    }

    if($opt_plot)
    {
	my @data_ary = ();
	if( $opt_plot =~ /r/)
	{
	    push(@data_ary, "\"file-access-rank_read.dat\" using 1:2 with lines lw 1 lt 1 title \"read\"");
	}
	if( $opt_plot =~ /w/)
	{
	    push(@data_ary, "\"file-access-rank_write.dat\" using 1:2 with lines lw 1 lt 2  title \"write\"");
	}
	if(!@data_ary)
	{
	    print STDERR "invalid value $opt_plot for access option (expected rw)\n";
	    exit(1);
	}
	$plot_data = join("," ,@data_ary);
    }


    if($opt_output)
    {
	$output_file = $opt_output;
    }

    if($opt_summary)
    {
	$print_summary = 1;
    }

    if($opt_verbose)
    {
	$verbose_flag = $opt_verbose;
    }

    if($opt_from_rank)
    {
	$plot_from = $opt_from_rank-0.1;
    }

    if($opt_to_rank)
    {
	$plot_to = $opt_to_rank+0.5;
    }

    if($opt_width)
    {
        $width = $opt_width;
    }

    if($opt_hight)
    {
        $hight = $opt_hight;
    }

    if($opt_start_time)
    {
	if(is_decimal($opt_start_time))
	{
	    $plot_start = $opt_start_time;
	} else
	{
	    print STDERR "invalid value $opt_start_time for start option (decimal expected)\n";
	    exit(1);
	}
    }

    if($opt_end_time)
    {
	if(is_decimal($opt_end_time))
	{
	    $plot_end = $opt_end_time;
	} else
	{
	    print STDERR "invalid value $opt_end_time for end option (decimal expected)\n";
	    exit(1);
	}
    }


    if($opt_caption)
    {
	$plot_caption = $opt_caption;
	$plot_caption =~ s/\"/\\\"/g;
    }


    if($opt_nocaption)
    {
	$plot_caption = "";
    }

    if($#ARGV != 0)
    {
        print "Error: invalid arguments.\n";
        print_help();
        exit(1);
    }

    $input_file = $ARGV[0];
}



sub check_prereqs
{
    my $rc;
    my $output;
    my @bins = ($darshan_parser,$gnuplot, $mv);
    foreach my $bin (@bins)
    {
        $rc = checkbin($bin);
        if ($rc)
        {
            print("error: $bin not found in PATH\n");
            exit(1);
        }
    }

    # check  gnuplot version
    $output = `$gnuplot --version`;
    if($? != 0)
    {
        print("error: failed to execute $gnuplot.\n");
        exit(1);
    }

    $output =~ /gnuplot (\d+)\.(\d+)/;
    if($1 < 4 || $2 < 2)
    {
        print("error: detected $gnuplot version $1.$2, but darshan-history requires at least 4.2.\n");
        exit(1);
    }

    return;
}

sub is_decimal
{
 $_[0] =~ /^([1-9]\d*|0)(\.\d+)?$/
}

sub checkbin($)
{
    my $binname = shift;
    my $rc;

    # save stdout/err
    open(SAVEOUT, ">&STDOUT");
    open(SAVEERR, ">&STDERR");

    # redirect stdout/error
    open(STDERR, '>/dev/null');
    open(STDOUT, '>/dev/null');
    $rc = system("which $binname");
    if ($rc)
    {
        $rc = 1;
    }
    close(STDOUT);
    close(STDERR);

    # suppress perl warning
    select(SAVEERR);
    select(SAVEOUT);

    # restore stdout/err
    open(STDOUT, ">&SAVEOUT");
    open(STDERR, ">&SAVEERR");

    return $rc;
}


sub get_minimum_start_time
{
    my $minimum_start_time = 0^-1; # MAX value
    open(PRETRACE, "< $tmp_dir/history.dat") || die("error opening input file: $!\n");

    while ($line = <PRETRACE>) {
        chomp($line);
        if ($line =~ /^# start = ([\d\.]+)/) {
             $start_time = $1;
             if ($start_time < $minimum_start_time) {
                 $minimum_start_time = $start_time;
             }
        }
    }
    close(PRETRACE);
    return $minimum_start_time;
}


sub print_help
{
    print <<EOF;

Usage: $PROGRAM_NAME <options> input_file

    --help                       Prints this help message
    --from_rank <rank_number>    Specifies a minimum rank number to display
    --to_rank <rank_number>      Specifies a maximal rank number to display
    --start_time <time>          Specifies start time(second) to display
    --end_time <time>            Specifies end time(second) to display
    --width <pixel>              Specifies xrange pixel
    --hight <pixel>              Specifies yrange pixel
    --plot <type>                Specifies I/O type to display
                                 (rw for read write, r for read only, w for write only)
    --caption <caption>          Specifies caption
    --nocaption                  Not print caption
    --output <output_file>       Specifies a file to write png output to
                                 (defaults to ./history_rank.png)
    --summary                    print summary in the csv format
    --verbose                    Prints and retains tmpdir used for output

Purpose:

    This script reads a Darshan output file generated by a job and
    generates a png file summarizing job behavior.

EOF
    return;
}
