This is the readme file for the python asynchronous I/O library source code.

PROJECT URL:
main: http://developer.berlios.de/projects/pyasynchio
files: http://developer.berlios.de/project/showfiles.php?group_id=4124

OS SUPPORT:
Currently only Microsoft Windows 2000 (except connect), XP (all works), 
2003 (havent tested, probably works) and probably future Windows operating 
systems seem to be supported.

COMPILING:

setup.py is a standard distutils-based script. Unfortunately at the moment
only Microsoft Visual Studio .NET 2003 C++ compiler is tested to be able to
compile this library. This library needs to locate "socketmodule.h" file
while compiling. It is a part of python's source code, although it is not
usually included in binary distributions (like ActivePython). setup.py uses
"PY_MODULES_DIR" environment variable to find "socketmodule.h", that is
"PY_MODULES_DIR" should point to directory where "socketmodule.h" is. Otherwise
setup.py will try to use join(PREFIX, 'Modules') directory, which usually
doesnt exist if you use ActivePython, and compilation will fail.

DEPENDENCIES:

This library depends on CRT (C Runtime Library) internals. More precisely,
library needs to get OS handle from CRT's FILE* pointer. Right now only
Microsoft C Runtime Library is working properly (most likely some changes
are to be made in source code to support other CRT implementations). 
This library MUST be linked against precisely same CRT version, which Python 
itself uses, otherwise file I/O will not work. Sorry.

This library also depends indirectly on socketmodule.c internal functions
(those which deal with tuple->network address and network address->tuple
conversion). Appropriate code is just copied from socketmodule.c, so if
socketmodule.c will be changed to support more network address families,
that support should be copied to this library (or maybe utilized in more
convenient manner). Sorry :)

ACKNOWLEDGEMENTS 

This library is developed mostly because of Google's summerofcode program (2005).
Mentoring organization for this project was Python Software Foundation (PSF).

I would like to express my gratitude to following people who were somehow 
involved in this library creation:

Mark Hammond, this project's mentor, for providing invaluable 
insights and somehow making me coding something that is (probably!) 
usable.

Aleksey Onopriyenko and Alexander Mizrakhi for starting some project which 
triggered something in my head which eventually became this library, and for 
some later discussions too.



AUTHOR:
Vladimir Sukhoy <randolphu@users.berlios.de>
