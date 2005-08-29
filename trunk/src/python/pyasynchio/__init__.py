"""\
This module provides support for asynchronous I/O (INPUT/OUTPUT) operations.
Currently it is available only on Windows (since 2000). Currently only socket 
and file I/O is supported. This module uses builtin objects for files and 
sockets. It makes interface more convenient and allows fast drop-in usage in 
existing python scripts, applications and libraries. This module provides
simple interface for asynch I/O and can be easily ported to different OS
and/or underlying AIO libraries.

Asynchronous (or "proactive") I/O is an alternative way to alleviate I/O 
bottlenecks without multithreading and without nonblocking 
select- or poll-based solutions. Usually asynchronous I/O is very efficient
in terms of resource usage, sometimes much more efficient than multithread,
select/poll or mixed solutions. AIO allows additional speed optimization on
OS and driver level and does not require operation to complete during the call 
to the function which starts the operation. 

Conventional (synchronous) I/O processing usually is done in following way:
1. Find a moment when it is possible to do I/O operation (through select/poll
or simply blocking further execution).
2. Perform that operation synchronously.

Asynchronous I/O processing is somewhat different:
1. Asynchronous operations are started on one or more I/O handles without
a need to wait until they're completed.
2. As each operation completes, their results are delivered via appropriate
mechanics (these mechanics are very much platform dependent).

Two most known implementations of asynchronous I/O are:
- Windows "overlapped" I/O.
- AIO family of functions in POSIX.4 Realtime Extension Standard (HP-UX, IRIX, Linux, LynxOS, Solaris)

Well known object oriented implementation of asynchronous I/O on top of
different low-level OS facilities is developed in ACE library 
(Doug Schmidt's "Proactor" pattern).

This module make use of "Asynchronous Completion Token" pattern to
simplify identification and processing of AIO results.

On Windows this module uses I/O completion ports and "overlapped" I/O directly
rather than relying on some library.

"""

from apoll import apoll

