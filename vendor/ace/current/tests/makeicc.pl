# makeicc.pl,v 4.4 2003/01/04 23:51:20 shuston Exp

@ls = `ls -1 *.cpp`;
$allicc = "icc.bat";

foreach $tmp (@ls)
{
	($a1, $a2) = split("cpp",$tmp);
	$tmp2 = "$a1\icc";

open(T,"> $a1\icc");
	print T "// \$Id\$\n";
	print T "\n";
	print T "include \"vacpp_setup.icc\"\n";
	print T "option\n";
        print T "    link(libSearchPath, platformLibSearchPath),\n";
	print T "    incl(searchPath, \"..\"),\n";
	print T "    link(linkWithMultiThreadLib,yes),\n";
	print T "    link(debug)\n";
	print T "    {\n";
	chop($a1);
	print T "	target type (exe) \"$a1\"\n";
	print T "	{\n";
	print T "	   source type (cpp) \"$a1\.cpp\"\n";
        print T '	   if ($__IBMCPP__ >= 500) {' . "\n";
        print T "	     option macros(global) {\n";
        print T "	       source type (hpp) \"test_config.h\"\n";
        print T "	     }\n";
        print T "	     source type (cpp) \"test_config.h\"\n";
        print T "	     source type (cpp) \"../ace/Singleton.cpp\"\n";
        print T "	   }\n";
        print T "	   source type (lib) platformLinkLibs\n";
	print T "	}\n";
	print T "    }\n";
close T;

open(ALLICC,">> $allicc");
	print ALLICC "vacbld -SEV=E $a1\.icc\n";
close ALLICC;
	print "$a1\n";
}

