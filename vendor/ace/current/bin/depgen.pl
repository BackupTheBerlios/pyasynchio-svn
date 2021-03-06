eval '(exit $?0)' && eval 'exec perl -w -S $0 ${1+"$@"}'
    & eval 'exec perl -w -S $0 $argv:q'
    if 0;

# ************************************************************
# Description   : Generate dependencies for GNU Make and NMake.
# Author        : Chad Elliott
# Create Date   : 5/06/2002
#            depgen.pl,v 1.10 2003/12/12 14:56:51 elliott_c Exp
# ************************************************************

# ************************************************************
# Pragma Section
# ************************************************************

use strict;
use Cwd;
use File::Basename;

my($execPath) = getExecutePath($0);
unshift(@INC, "$execPath/DependencyGenerator");

require DependencyEditor;

# ************************************************************
# Data Section
# ************************************************************

my($version)  = '0.8';
my($os)       = ($^O eq 'MSWin32' || $^O eq 'cygwin' ? 'Windows' : 'UNIX');
my(%types)    = ('gnu'   => 1,
                 'nmake' => 1,
                );
my(%defaults) = ('UNIX'    => ['gnu'],
                 'Windows' => ['nmake'],
                );

# ************************************************************
# Subroutine Section
# ************************************************************

sub which {
  my($prog) = shift;
  my($exec) = $prog;

  if (defined $ENV{'PATH'}) {
    my($envSep) = ($^O eq 'MSWin32' ? ';' : ':');
    foreach my $part (split(/$envSep/, $ENV{'PATH'})) {
      $part .= "/$prog";
      if ( -x $part ) {
        $exec = $part;
        last;
      }
    }
  }

  return $exec;
}


sub getExecutePath {
  my($prog) = shift;
  my($loc)  = '';

  if ($prog ne basename($prog)) {
    if ($prog =~ /^[\/\\]/ ||
        $prog =~ /^[A-Za-z]:[\/\\]?/) {
      $loc = dirname($prog);
    }
    else {
      $loc = getcwd() . '/' . dirname($prog);
    }
  }
  else {
    $loc = dirname(which($prog));
  }

  if ($loc eq '.') {
    $loc = getcwd();
  }

  if ($loc ne '') {
    $loc .= '/';
  }

  return $loc;
}


sub usageAndExit {
  my($base) = shift;
  my($opt)  = shift;

  if (defined $opt) {
    print "$opt.\n";
  }

  print "$base v$version\n" .
        "Usage: $base [-D<MACRO>[=VALUE]] [-I<include dir>] [-A] " .
        "[-R <VARNAME>]\n" .
        "       " . (" " x length($base)) .
        " [-f <output file>] [-t <type>] [-n] <files...>\n" .
        "\n" .
        "-D   This option sets a macro to an optional value.\n" .
        "-I   The -I option adds an include directory.\n" .
        "-A   Replace \$ACE_ROOT and \$TAO_ROOT paths with \$(ACE_ROOT) " .
        "and \$(TAO_ROOT)\n     respectively.\n" .
        "-R   Replace \$VARNAME paths with \$(VARNAME).\n" .
        "-f   Specifies the output file.  This file will be edited if it " .
        "already\n     exists.\n" .
        "-n   Do not include inline files (ending in .i or .inl) in the " .
        "dependencies.\n" .
        "-t   Use specified type (";
  my(@keys) = sort keys %types;
  for(my $i = 0; $i <= $#keys; ++$i) {
    print "$keys[$i]" .
          ($i != $#keys ? $i == $#keys - 1 ? ' or ' : ', ' : '');;
  }
  print ") instead of the default.\n" .
        "     The default is ";
  @keys = sort keys %defaults;
  for(my $i = 0; $i <= $#keys; ++$i) {
    my($def) = $keys[$i];
    print $defaults{$def}->[0] . " on $def" .
          ($i != $#keys ? $i == $#keys - 1 ? ' and ' : ', ' : '');
  }
  print ".\n";
  exit(0);
}


sub setReplace {
  my($replace) = shift;
  my($name)    = shift;
  my($value)   = shift;

  ## The key will be used in a regular expression.
  ## So, we need to escape some special characters.
  $name =~ s/([\+\-\\\$\[\]\(\)\.])/\\$1/g;
  $$replace{$name} = $value;
}


# ************************************************************
# Main Section
# ************************************************************

my($base)     = basename($0);
my($type)     = $defaults{$os}->[0];
my($noinline) = undef;
my(@files)    = ();
my(%macros)   = ();
my(@ipaths)   = ();
my(%replace)  = ();
my($output)   = '-';


if (defined $ENV{ACE_ROOT} && !defined $ENV{TAO_ROOT}) {
  $ENV{TAO_ROOT} = "$ENV{ACE_ROOT}/TAO";
}

for(my $i = 0; $i <= $#ARGV; ++$i) {
  my($arg) = $ARGV[$i];
  if ($arg =~ /^\-D(\w+)(=(.*))?/) {
    $macros{$1} = $3;
  }
  elsif ($arg =~ /^\-I(.*)/) {
    push(@ipaths, $1);
  }
  elsif ($arg eq '-A') {
    setReplace(\%replace, $ENV{ACE_ROOT}, '$(ACE_ROOT)');
    setReplace(\%replace, $ENV{TAO_ROOT}, '$(TAO_ROOT)');
  }
  elsif ($arg eq '-R') {
    ++$i;
    $arg = $ARGV[$i];
    if (defined $arg) {
      my($val) = $ENV{$arg};
      if (defined $val) {
        setReplace(\%replace, $val, "\$($arg)");
      }
    }
    else {
      usageAndExit($base, 'Invalid use of -R');
    }
  }
  elsif ($arg eq '-f') {
    ++$i;
    $arg = $ARGV[$i];
    if (defined $arg) {
      $output = $arg;
    }
    else {
      usageAndExit($base, 'Invalid use of -f');
    }
  }
  elsif ($arg eq '-n') {
    $noinline = 1;
  }
  elsif ($arg eq '-h') {
    usageAndExit($base);
  }
  elsif ($arg eq '-t') {
    ++$i;
    $arg = $ARGV[$i];
    if (defined $arg && defined $types{$arg}) {
      $type = $arg;
    }
    else {
      usageAndExit($base, 'Invalid use of -t');
    }
  }
  elsif ($arg =~ /^[\-+]/) {
    ## We will ignore unknown options
    ## Some options for aCC start with +
  }
  else {
    push(@files, $arg);
  }
}

if (!defined $files[0]) {
  usageAndExit($base, 'No files specified');
}

my($editor) = new DependencyEditor();
exit($editor->process($output, $type, $noinline,
                      \%macros, \@ipaths, \%replace, \@files));
