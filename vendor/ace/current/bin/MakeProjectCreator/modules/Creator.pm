package Creator;

# ************************************************************
# Description   : Base class for workspace and project creators
# Author        : Chad Elliott
# Create Date   : 5/13/2002
# ************************************************************

# ************************************************************
# Pragmas
# ************************************************************

use strict;
use FileHandle;
use File::Basename;

use Parser;

use vars qw(@ISA);
@ISA = qw(Parser);

# ************************************************************
# Data Section
# ************************************************************

my(@statekeys) = ('global', 'include', 'template', 'ti',
                  'dynamic', 'static', 'relative', 'addtemp',
                  'addproj', 'progress', 'toplevel', 'baseprojs',
                  'feature_file', 'hierarchy', 'name_modifier',
                  'apply_project',
                 );

my(%all_written) = ();

# ************************************************************
# Subroutine Section
# ************************************************************

sub new {
  my($class)     = shift;
  my($global)    = shift;
  my($inc)       = shift;
  my($template)  = shift;
  my($ti)        = shift;
  my($dynamic)   = shift;
  my($static)    = shift;
  my($relative)  = shift;
  my($addtemp)   = shift;
  my($addproj)   = shift;
  my($progress)  = shift;
  my($toplevel)  = shift;
  my($baseprojs) = shift;
  my($feature)   = shift;
  my($hierarchy) = shift;
  my($nmodifier) = shift;
  my($applypj)   = shift;
  my($type)      = shift;
  my($self)      = Parser::new($class, $inc);

  $self->{'relative'}        = $relative;
  $self->{'template'}        = $template;
  $self->{'ti'}              = $ti;
  $self->{'global'}          = $global;
  $self->{'grammar_type'}    = $type;
  $self->{'type_check'}      = $type . '_defined';
  $self->{'global_read'}     = 0;
  $self->{'current_input'}   = '';
  $self->{'progress'}        = $progress;
  $self->{'addtemp'}         = $addtemp;
  $self->{'addproj'}         = $addproj;
  $self->{'toplevel'}        = $toplevel;
  $self->{'files_written'}   = [];
  $self->{'reading_global'}  = 0;
  $self->{'global_assign'}   = {};
  $self->{'assign'}          = {};
  $self->{'baseprojs'}       = $baseprojs;
  $self->{'dynamic'}         = $dynamic;
  $self->{'static'}          = $static;
  $self->{'feature_file'}    = $feature;
  $self->{'hierarchy'}       = $hierarchy;
  $self->{'name_modifier'}   = $nmodifier;
  $self->{'apply_project'}   = $applypj;
  $self->{'convert_slashes'} = $self->convert_slashes();

  return $self;
}


sub collect_line {
  my($self)        = shift;
  my($fh)          = shift;
  my($lref)        = shift;
  my($line)        = shift;
  my($status)      = 1;
  my($errorString) = '';

  $$lref .= $self->strip_line($line);

  if ($$lref =~ /\\$/) {
    $$lref =~ s/\\$/ /;
  }
  else {
    ($status, $errorString) = $self->parse_line($fh, $$lref);
    $$lref = '';
  }

  return $status, $errorString;
}


sub generate_default_input {
  my($self)   = shift;
  my($status) = 0;
  my($error)  = '';

  ($status, $error) = $self->parse_line(undef, "$self->{'grammar_type'} {");
  ($status, $error) = $self->parse_line(undef, '}');

  if (!$status) {
    print STDERR "$error\n";
  }

  return $status;
}


sub parse_file {
  my($self)  = shift;
  my($input) = shift;
  my($oline) = $self->get_line_number();

  ## Read the input file and get the last line number
  my($status, $errorString) = $self->read_file($input);

  if (!$status) {
    print STDERR $self->getcwd() .
                 "/$input: line " . $self->get_line_number() .
                 ":\n$errorString\n";
  }
  elsif ($status && $self->{$self->{'type_check'}}) {
    ## If we are at the end of the file and the type we are looking at
    ## is still defined, then we have an error
    print STDERR $self->getcwd() .
                 "/$input: line " . $self->get_line_number() .
                 ":\nERROR: Did not " .
                 "find the end of the $self->{'grammar_type'}\n";
    $status = 0;
  }
  $self->set_line_number($oline);

  return $status;
}


sub generate {
  my($self)   = shift;
  my($input)  = shift;
  my($status) = 1;

  ## Reset the files_written array between processing each file
  $self->{'files_written'}  = [];

  ## Allow subclasses to reset values before
  ## each call to generate().
  $self->reset_values();

  ## Read the global configuration file
  if (!$self->{'global_read'}) {
    $status = $self->read_global_configuration();
    $self->{'global_read'} = 1;
  }

  if ($status) {
    $self->{'current_input'} = $input;

    ## An empty input file name says that we
    ## should generate a default input file and use that
    if ($input eq '') {
      $status = $self->generate_default_input();
    }
    else {
      $status = $self->parse_file($input);
    }
  }

  return $status;
}


sub parse_assignment {
  my($self)   = shift;
  my($line)   = shift;
  my($values) = shift;
  my($status) = 1;

  if ($line =~ /^(\w+)\s*\+=\s*(.*)?/) {
    my($name)  = lc($1);
    my($value) = $2;
    push(@$values, 'assign_add', $name, $value);
  }
  elsif ($line =~ /^(\w+)\s*=\s*(.*)?/) {
    my($name)  = lc($1);
    my($value) = $2;
    push(@$values, 'assignment', $name, $value);
  }
  elsif ($line =~ /^(\w+)\s*\-=\s*(.*)?/) {
    my($name)  = lc($1);
    my($value) = $2;
    push(@$values, 'assign_sub', $name, $value);
  }
  else {
    $status = 0;
  }

  return $status;
}


sub parse_known {
  my($self)        = shift;
  my($line)        = shift;
  my($status)      = 1;
  my($errorString) = '';
  my($type)        = $self->{'grammar_type'};
  my(@values)      = ();

  ##
  ## Each regexp that looks for the '{' looks for it at the
  ## end of the line.  It is purposely this way to decrease
  ## the amount of extra lines in each file.  This
  ## allows for the most compact file as human readably
  ## possible.
  ##
  if ($line eq '') {
  }
  elsif ($line =~ /^$type\s*(\([^\)]+\))?\s*(:.*)?\s*{$/) {
    my($name)    = $1;
    my($parents) = $2;
    if ($self->{$self->{'type_check'}}) {
      $errorString = "ERROR: Did not find the end of the $type";
      $status = 0;
    }
    else {
      if (defined $parents) {
        my(@parents) = ();
        $parents =~ s/^://;
        foreach my $parent (split(',', $parents)) {
          $parent =~ s/^\s+//;
          $parent =~ s/\s+$//;
          if ($parent ne '') {
            push(@parents, $parent);
          }
        }
        if (!defined $parents[0]) {
          ## The : was used, but no parents followed.  This
          ## is an error.
          $errorString = 'ERROR: No parents listed';
          $status = 0;
        }
        $parents = \@parents;
      }
      push(@values, $type, $name, $parents);
    }
  }
  elsif ($line =~ /^}$/) {
    if ($self->{$self->{'type_check'}}) {
      push(@values, $type, $line);
    }
    else {
      $errorString = "ERROR: Did not find the beginning of the $type";
      $status = 0;
    }
  }
  elsif ($line =~ /^(feature)\s*\(([^\)]+)\)\s*{$/) {
    my($type)  = $1;
    my($name)  = $2;
    my(@names) = split(/\s*,\s*/, $name);
    push(@values, $type, \@names);
  }
  elsif (!$self->{$self->{'type_check'}}) {
    $errorString = "ERROR: No $type was defined";
    $status = 0;
  }
  elsif ($self->parse_assignment($line, \@values)) {
    ## If this returns true, then we've found an assignment
  }
  elsif ($line =~ /^(\w+)\s*(\([^\)]+\))?\s*{$/) {
    my($comp) = lc($1);
    my($name) = $2;

    if (defined $name) {
      $name =~ s/^\(\s*//;
      $name =~ s/\s*\)$//;
    }
    else {
      $name = $self->get_default_component_name();
    }
    push(@values, 'component', $comp, $name);
  }
  else {
    $errorString = "ERROR: Unrecognized line: $line";
    $status = -1;
  }

  return $status, $errorString, @values;
}


sub parse_scope {
  my($self)        = shift;
  my($fh)          = shift;
  my($name)        = shift;
  my($type)        = shift;
  my($validNames)  = shift;
  my($flags)       = shift;
  my($status)      = 0;
  my($errorString) = "ERROR: Unable to process $name";

  if (!defined $flags) {
    $flags = {};
  }

  while(<$fh>) {
    my($line) = $self->strip_line($_);

    if ($line eq '') {
    }
    elsif ($line =~ /^}/) {
      $status = 1;
      $errorString = '';
      $self->handle_scoped_end($type, $flags);
      last;
    }
    else {
      my(@values) = ();
      if ($self->parse_assignment($line, \@values)) {
        if (defined $$validNames{$values[1]}) {
          if ($values[0] eq 'assignment') {
            $self->process_assignment($values[1], $values[2], $flags);
          }
          elsif ($values[0] eq 'assign_add') {
            $self->process_assignment_add($values[1], $values[2], $flags);
          }
          elsif ($values[0] eq 'assign_sub') {
            $self->process_assignment_sub($values[1], $values[2], $flags);
          }
        }
        else {
          $status = 0;
          $errorString = "ERROR: Invalid assignment name: $values[1]";
          last;
        }
      }
      else {
        ($status, $errorString) = $self->handle_scoped_unknown($fh,
                                                               $type,
                                                               $flags,
                                                               $line);
        if (!$status) {
          last;
        }
      }
    }
  }
  return $status, $errorString;
}


sub base_directory {
  my($self) = shift;
  return basename($self->getcwd());
}


sub generate_default_file_list {
  my($self)    = shift;
  my($dir)     = shift;
  my($exclude) = shift;
  my($dh)      = new FileHandle();
  my(@files)   = ();

  if (opendir($dh, $dir)) {
    my($need_dir) = ($dir ne '.');
    foreach my $file (grep(!/^\.\.?$/, readdir($dh))) {
      my($skip) = 0;
      ## Prefix each file name with the directory only if it's not '.'
      my($full) = ($need_dir ? "$dir/" : '') . $file;

      if (defined $$exclude[0]) {
        foreach my $exc (@$exclude) {
          if ($full eq $exc) {
            $skip = 1;
            last;
          }
        }
      }

      if (!$skip) {
        push(@files, $full);
      }
    }

    if ($self->sort_files()) {
      @files = sort { $self->file_sorter($a, $b) } @files;
    }

    closedir($dh);
  }
  return @files;
}


sub windows_crlf {
  #my($self) = shift;
  if ($^O eq 'MSWin32' || $^O eq 'cygwin') {
    return "\n";
  }
  else {
    return "\r\n";
  }
}


sub transform_file_name {
  my($self) = shift;
  my($name) = shift;

  $name =~ s/[\s\-]/_/g;
  return $name;
}


sub file_written {
  my($self) = shift;
  my($file) = shift;
  return (defined $all_written{$self->getcwd() . '/' . $file});
}


sub add_file_written {
  my($self) = shift;
  my($file) = shift;

  foreach my $written (@{$self->{'files_written'}}) {
    if ($written eq $file) {
      print "WARNING: $file has been overwritten by a " .
            "$self->{'grammar_type'} with a duplicate name.\n";
      last;
    }
    elsif (lc($written) eq lc($file)) {
      print "WARNING: $file has been overwritten by a " .
            "$self->{'grammar_type'} with different casing: $written.\n";
      last;
    }
  }
  push(@{$self->{'files_written'}}, $file);

  $all_written{$self->getcwd() . '/' . $file} = 1;
}


sub extension_recursive_input_list {
  my($self)    = shift;
  my($dir)     = shift;
  my($exclude) = shift;
  my($ext)     = shift;
  my($fh)      = new FileHandle();
  my(@files)   = ();

  if (opendir($fh, $dir)) {
    foreach my $file (grep(!/^\.\.?$/, readdir($fh))) {
      my($skip) = 0;
      my($full) = ($dir ne '.' ? "$dir/" : '') . $file;

      ## Check for command line exclusions
      if (defined $$exclude[0]) {
        foreach my $exc (@$exclude) {
          if ($full eq $exc) {
            $skip = 1;
            last;
          }
        }
      }

      ## If we are not skipping this directory or file, then check it out
      if (!$skip) {
        if (-d $full) {
          push(@files, $self->extension_recursive_input_list($full,
                                                             $exclude,
                                                             $ext));
        }
        elsif ($full =~ /$ext$/) {
          push(@files, $full);
        }
      }
    }
    closedir($fh);
  }

  return @files;
}


sub modify_assignment_value {
  my($self)  = shift;
  my($value) = shift;

  if ($self->{'convert_slashes'}) {
    $value = $self->slash_to_backslash($value);
  }
  return $value;
}


sub process_assignment {
  my($self)   = shift;
  my($name)   = shift;
  my($value)  = shift;
  my($assign) = shift;


  ## If no hash table was passed in
  if (!defined $assign) {
    my($tag) = ($self->{'reading_global'} ? 'global_assign' : 'assign');
    $assign  = $self->{$tag};

    ## If we haven't yet defined the hash table in this project
    if (!defined $assign) {
      $assign = {};
      $self->{$tag} = $assign;
    }
  }

  if (defined $value) {
    $value =~ s/^\s+//;
    $value =~ s/\s+$//;

    ## Modify the assignment value before saving it
    $$assign{$name} = $self->modify_assignment_value($value);
  }
  else {
    $$assign{$name} = undef;
  }
}


sub process_assignment_add {
  my($self)   = shift;
  my($name)   = shift;
  my($value)  = shift;
  my($assign) = shift;
  my($order)  = shift;
  my($nval)   = $self->get_assignment($name, $assign);

  if (defined $nval) {
    if ($order) {
      $nval .= " $value";
    }
    else {
      $nval = "$value $nval";
    }
  }
  else {
    $nval = $value;
  }
  $self->process_assignment($name, $nval, $assign);
  $self->process_duplicate_modification($name, $assign);
}


sub process_assignment_sub {
  my($self)   = shift;
  my($name)   = shift;
  my($value)  = shift;
  my($assign) = shift;
  my($nval)   = $self->get_assignment($name, $assign);

  if (defined $nval) {
    ## Remove double quotes if there are any
    $value =~ s/^\"(.*)\"$/$1/;

    ## Escape any regular expression special characters
    $value = $self->escape_regex_special($value);

    if ($nval =~ /$value/) {
      ## Search for the first occurrence and remove it
      $nval =~ s/$value//;

      ## Reset the value
      $self->process_assignment($name, $nval, $assign);
    }
  }
}


sub fill_type_name {
  my($self) = shift;
  my($name) = shift;
  my($def)  = shift;

  if ($name =~ /\*/) {
    my($pre)  = $def . '_';
    my($mid)  = '_' . $def . '_';
    my($post) = '_' . $def;

    ## Replace the beginning and end first then the middle
    $name =~ s/^\*/$pre/;
    $name =~ s/\*$/$post/;
    $name =~ s/\*/$mid/g;

    ## Remove any trailing underscore or any underscore that is followed
    ## by a space.  This value could be a space separated list.
    $name =~ s/_$//;
    $name =~ s/_\s/ /g;
    $name =~ s/\s_/ /g;

    ## If any one word is capitalized then capitalize each word
    if ($name =~ /[A-Z][0-9a-z_]+/) {
      ## Do the first word
      if ($name =~ /^([a-z])([^_]+)/) {
        my($first) = uc($1);
        my($rest)  = $2;
        $name =~ s/^[a-z][^_]+/$first$rest/;
      }
      ## Do subsequent words
      while($name =~ /(_[a-z])([^_]+)/) {
        my($first) = uc($1);
        my($rest)  = $2;
        $name =~ s/_[a-z][^_]+/$first$rest/;
      }
    }
  }

  return $name;
}


sub save_state {
  my($self)  = shift;
  my(%state) = ();

  ## Make a deep copy of each state value.  That way our array
  ## references and hash references do not get accidentally modified.
  foreach my $skey (@statekeys) {
    if (UNIVERSAL::isa($self->{$skey}, 'ARRAY')) {
      $state{$skey} = [];
      foreach my $element (@{$self->{$skey}}) {
        push(@{$state{$skey}}, $element);
      }
    }
    elsif (UNIVERSAL::isa($self->{$skey}, 'HASH')) {
      $state{$skey} = {};
      foreach my $key (keys %{$self->{$skey}}) {
        $state{$skey}->{$key} = $self->{$skey}->{$key};
      }
    }
    else {
      $state{$skey} = $self->{$skey};
    }
  }

  return %state;
}


sub restore_state {
  my($self)  = shift;
  my($state) = shift;

  ## Make a deep copy of each state value.  That way our array
  ## references and hash references do not get accidentally modified.
  foreach my $skey (@statekeys) {
    if (UNIVERSAL::isa($state->{$skey}, 'ARRAY')) {
      my(@arr) = @{$state->{$skey}};
      $self->{$skey} = \@arr;
    }
    elsif (UNIVERSAL::isa($state->{$skey}, 'HASH')) {
      my(%hash) = %{$state->{$skey}};
      $self->{$skey} = \%hash;
    }
    else {
      $self->{$skey} = $state->{$skey};
    }
  }
}


sub get_global_cfg {
  my($self) = shift;
  return $self->{'global'};
}


sub get_template_override {
  my($self) = shift;
  return $self->{'template'};
}


sub get_ti_override {
  my($self) = shift;
  return $self->{'ti'};
}


sub get_relative {
  my($self) = shift;
  return $self->{'relative'};
}


sub get_current_input {
  my($self) = shift;
  return $self->{'current_input'};
}


sub get_progress_callback {
  my($self) = shift;
  return $self->{'progress'};
}


sub get_addtemp {
  my($self) = shift;
  return $self->{'addtemp'};
}


sub get_addproj {
  my($self) = shift;
  return $self->{'addproj'};
}


sub get_toplevel {
  my($self) = shift;
  return $self->{'toplevel'};
}


sub get_files_written {
  my($self) = shift;
  return $self->{'files_written'};
}


sub get_assignment {
  my($self)   = shift;
  my($name)   = shift;
  my($assign) = shift;

  ## If no hash table was passed in
  if (!defined $assign) {
    my($tag) = ($self->{'reading_global'} ? 'global_assign' : 'assign');
    $assign = $self->{$tag};
  }

  return $$assign{$name};
}


sub get_baseprojs {
  my($self) = shift;
  return $self->{'baseprojs'};
}


sub get_dynamic {
  my($self) = shift;
  return $self->{'dynamic'};
}


sub get_static {
  my($self) = shift;
  return $self->{'static'};
}


sub get_default_component_name {
  #my($self) = shift;
  return 'default';
}


sub get_hierarchy {
  my($self) = shift;
  return $self->{'hierarchy'};
}


sub get_name_modifier {
  my($self) = shift;
  return $self->{'name_modifier'};
}


sub get_apply_project {
  my($self) = shift;
  return $self->{'apply_project'};
}

# ************************************************************
# Virtual Methods To Be Overridden
# ************************************************************

sub compare_output {
  #my($self) = shift;
  return 0;
}


sub handle_scoped_end {
  #my($self)  = shift;
  #my($type)  = shift;
  #my($flags) = shift;
}


sub handle_scoped_unknown {
  my($self)  = shift;
  my($fh)    = shift;
  my($type)  = shift;
  my($flags) = shift;
  my($line)  = shift;
  return 0, "ERROR: Unrecognized line: $line";
}


sub process_duplicate_modification {
  #my($self)   = shift;
  #my($name)   = shift;
  #my($assign) = shift;
}

sub generate_recursive_input_list {
  #my($self) = shift;
  #my($dir)  = shift;
  return ();
}


sub crlf {
  #my($self) = shift;
  return "\n";
}


sub reset_values {
  #my($self) = shift;
}


sub sort_files {
  #my($self) = shift;
  return 1;
}


sub file_sorter {
  my($self)  = shift;
  my($left)  = shift;
  my($right) = shift;
  return $left cmp $right;
}


sub read_global_configuration {
  #my($self)  = shift;
  #my($input) = shift;
  return 1;
}


1;
