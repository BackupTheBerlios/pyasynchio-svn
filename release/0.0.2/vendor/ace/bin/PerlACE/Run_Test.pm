# Run_Test.pm,v 1.10 2004/11/19 00:05:33 gmaxey Exp

# This module contains a few miscellanous functions and some
# startup ARGV processing that is used by all tests.

use PerlACE::Process;
use PerlACE::ConfigList;

package PerlACE;
use Cwd;

# Figure out the svc.conf extension
$svcconf_ext = $ENV{"ACE_RUNTEST_SVCCONF_EXT"};
if (!defined $svcconf_ext) {
    $svcconf_ext = ".conf";
}

# Default timeout.  NSCORBA needs more time for process start up.
$wait_interval_for_process_creation = ($^O eq "nonstop_kernel") ? 10 : 5;

# Turn on autoflush
$| = 1;

sub LocalFile ($)
{
    my $file = shift;

    my $newfile = getcwd () . '/' . $file;

    if ($^O eq "MSWin32") {
        $newfile =~ s/\//\\/g;
    }
    elsif ($^O eq 'cygwin') {
        chop($newfile = `/usr/bin/cygpath -w $newfile`);
        $newfile =~ s/\\/\\\\/g;
    }

    return $newfile;
}


# Returns a unique id, uid for unix, last digit of IP for NT
sub uniqueid
{
  if ($^O eq "MSWin32")
  {
    my $uid = 1;

    open (IPNUM, "ipconfig|") || die "Can't run ipconfig: $!\n";

    while (<IPNUM>)
    {
      if (/Address/)
      {
        $uid = (split (/: (\d+)\.(\d+)\.(\d+)\.(\d+)/))[4];
      }
    }

    close IPNUM;

    return $uid;
  }
  else
  {
    return $>;
  }
}

# Waits until a file exists
sub waitforfile
{
  local($file) = @_;
  sleep 1 while (!(-e $file && -s $file));
}

sub waitforfile_timed
{
  my $file = shift;
  my $maxtime = shift;
  $maxtime *= $PerlACE::Process::WAIT_DELAY_FACTOR;

  while ($maxtime-- != 0) {
    if (-e $file && -s $file) {
      return 0;
    }
    sleep 1;
  }
  return -1;
}

sub check_n_cleanup_files
{
  my $file = shift;
  my @flist = glob ($file);

  my $cntr = 0;
  my $nfile = scalar(@flist);

  if ($nfile != 0) {
    for (; $cntr < $nfile; $cntr++) {
      print STDERR "File <$flist[$cntr]> exists but should be cleaned up\n";
    }
    unlink @flist;
  }
}

sub generate_test_file
{
  my $file = shift;
  my $size = shift;

  while ( -e $file ) {
    $file = $file."X";
  }

  my $data = "abcdefghijklmnopqrstuvwxyz";
  $data = $data.uc($data)."0123456789";

  open( INPUT, "> $file" ) || die( "can't create input file: $file" );
  for($i=62; $i < $size ; $i += 62 ) {
    print INPUT $data;
  }
  $i -= 62;
  if ($i < $size) {
    print INPUT substr($data, 0, $size-$i);
  }
  close(INPUT);

  return $file;
}

$sleeptime = 5;

1;
