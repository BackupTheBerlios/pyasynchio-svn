#----------------------------------------------------------------------------
#	Makefile,v 1.14 2004/01/05 02:42:26 bala Exp
#
#	Makefile for repeating token client application
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#	Local macros
#----------------------------------------------------------------------------

BIN   	= proc_sema

FILES 	= proc_sema

LSRC   	= $(addsuffix .cpp,$(FILES))
LOBJ   	= $(LSRC:%.cpp=$(VDIR)%.o)
SHOBJ	= $(addsuffix .so,$(FILES))

BUILD 	= $(VBIN)

#----------------------------------------------------------------------------
#	Include macros and targets
#----------------------------------------------------------------------------

include		$(ACE_ROOT)/include/makeinclude/wrapper_macros.GNU
include		$(ACE_ROOT)/include/makeinclude/macros.GNU
include		$(ACE_ROOT)/include/makeinclude/rules.common.GNU
include		$(ACE_ROOT)/include/makeinclude/rules.nonested.GNU
include		$(ACE_ROOT)/include/makeinclude/rules.bin.GNU
include		$(ACE_ROOT)/include/makeinclude/rules.local.GNU

#----------------------------------------------------------------------------
#	Local targets
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#	Dependencies
#----------------------------------------------------------------------------

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
# DO NOT DELETE THIS LINE -- g++dep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.


.obj/proc_sema.o .obj/proc_sema.so .shobj/proc_sema.o .shobj/proc_sema.so: proc_sema.cpp \
  $(ACE_ROOT)/ace/OS_main.h \
  $(ACE_ROOT)/ace/ACE_export.h \
  $(ACE_ROOT)/ace/pre.h \
  $(ACE_ROOT)/ace/config.h \
  $(ACE_ROOT)/ace/$(ACE_PLATFORM_CONFIG) \
  $(ACE_ROOT)/ace/post.h \
  $(ACE_ROOT)/ace/ace_wchar.h \
  $(ACE_ROOT)/ace/ace_wchar.inl \
  $(ACE_ROOT)/ace/Process_Semaphore.h \
  $(ACE_ROOT)/ace/SV_Semaphore_Complex.h \
  $(ACE_ROOT)/ace/SV_Semaphore_Simple.h \
  $(ACE_ROOT)/ace/os_include/sys/os_stat.h \
  $(ACE_ROOT)/ace/config-all.h \
  $(ACE_ROOT)/ace/os_include/sys/os_types.h \
  $(ACE_ROOT)/ace/os_include/os_stddef.h \
  $(ACE_ROOT)/ace/os_include/sys/os_ipc.h \
  $(ACE_ROOT)/ace/os_include/sys/os_sem.h \
  $(ACE_ROOT)/ace/Default_Constants.h \
  $(ACE_ROOT)/ace/SV_Semaphore_Simple.i \
  $(ACE_ROOT)/ace/Global_Macros.h \
  $(ACE_ROOT)/ace/OS_Errno.h \
  $(ACE_ROOT)/ace/os_include/os_errno.h \
  $(ACE_ROOT)/ace/OS_Errno.inl \
  $(ACE_ROOT)/ace/OS_NS_Thread.h \
  $(ACE_ROOT)/ace/Basic_Types.h \
  $(ACE_ROOT)/ace/os_include/os_limits.h \
  $(ACE_ROOT)/ace/os_include/os_unistd.h \
  $(ACE_ROOT)/ace/os_include/os_inttypes.h \
  $(ACE_ROOT)/ace/os_include/os_stdint.h \
  $(ACE_ROOT)/ace/os_include/os_stdio.h \
  $(ACE_ROOT)/ace/os_include/os_stdarg.h \
  $(ACE_ROOT)/ace/os_include/os_float.h \
  $(ACE_ROOT)/ace/os_include/os_stdlib.h \
  $(ACE_ROOT)/ace/os_include/sys/os_wait.h \
  $(ACE_ROOT)/ace/os_include/os_signal.h \
  $(ACE_ROOT)/ace/os_include/os_time.h \
  $(ACE_ROOT)/ace/os_include/os_ucontext.h \
  $(ACE_ROOT)/ace/os_include/sys/os_resource.h \
  $(ACE_ROOT)/ace/os_include/sys/os_time.h \
  $(ACE_ROOT)/ace/Basic_Types.i \
  $(ACE_ROOT)/ace/os_include/os_pthread.h \
  $(ACE_ROOT)/ace/Base_Thread_Adapter.h \
  $(ACE_ROOT)/ace/OS_Log_Msg_Attributes.h \
  $(ACE_ROOT)/ace/iosfwd.h \
  $(ACE_ROOT)/ace/OS_Log_Msg_Attributes.inl \
  $(ACE_ROOT)/ace/Base_Thread_Adapter.inl \
  $(ACE_ROOT)/ace/os_include/os_semaphore.h \
  $(ACE_ROOT)/ace/OS_Memory.h \
  $(ACE_ROOT)/ace/OS_NS_stdlib.h \
  $(ACE_ROOT)/ace/OS_NS_stdlib.inl \
  $(ACE_ROOT)/ace/Object_Manager_Base.h \
  $(ACE_ROOT)/ace/Cleanup.h \
  $(ACE_ROOT)/ace/Cleanup.inl \
  $(ACE_ROOT)/ace/Object_Manager_Base.inl \
  $(ACE_ROOT)/ace/OS_NS_string.h \
  $(ACE_ROOT)/ace/OS_NS_string.inl \
  $(ACE_ROOT)/ace/OS_NS_wchar.h \
  $(ACE_ROOT)/ace/os_include/os_wchar.h \
  $(ACE_ROOT)/ace/os_include/os_string.h \
  $(ACE_ROOT)/ace/os_include/os_ctype.h \
  $(ACE_ROOT)/ace/OS_NS_wchar.inl \
  $(ACE_ROOT)/ace/os_include/os_search.h \
  $(ACE_ROOT)/ace/OS_Memory.inl \
  $(ACE_ROOT)/ace/OS_NS_signal.h \
  $(ACE_ROOT)/ace/OS_NS_signal.inl \
  $(ACE_ROOT)/ace/OS_NS_macros.h \
  $(ACE_ROOT)/ace/OS_NS_errno.h \
  $(ACE_ROOT)/ace/OS_NS_errno.inl \
  $(ACE_ROOT)/ace/OS_NS_Thread.inl \
  $(ACE_ROOT)/ace/Time_Value.h \
  $(ACE_ROOT)/ace/Time_Value.inl \
  $(ACE_ROOT)/ace/OS_NS_sys_time.h \
  $(ACE_ROOT)/ace/OS_NS_sys_time.inl \
  $(ACE_ROOT)/ace/OS_NS_sys_mman.h \
  $(ACE_ROOT)/ace/OS_NS_sys_mman.inl \
  $(ACE_ROOT)/ace/OS_NS_fcntl.h \
  $(ACE_ROOT)/ace/os_include/os_fcntl.h \
  $(ACE_ROOT)/ace/OS_NS_fcntl.inl \
  $(ACE_ROOT)/ace/OS_NS_unistd.h \
  $(ACE_ROOT)/ace/OS_NS_unistd.inl \
  $(ACE_ROOT)/ace/OS_NS_sys_utsname.h \
  $(ACE_ROOT)/ace/os_include/sys/os_utsname.h \
  $(ACE_ROOT)/ace/OS_NS_sys_utsname.inl \
  $(ACE_ROOT)/ace/OS_NS_stdio.h \
  $(ACE_ROOT)/ace/OS_NS_stdio.inl \
  $(ACE_ROOT)/ace/OS_NS_pwd.h \
  $(ACE_ROOT)/ace/os_include/os_pwd.h \
  $(ACE_ROOT)/ace/OS_NS_pwd.inl \
  $(ACE_ROOT)/ace/os_include/sys/os_mman.h \
  $(ACE_ROOT)/ace/SV_Semaphore_Complex.i \
  $(ACE_ROOT)/ace/Process_Semaphore.inl \
  $(ACE_ROOT)/ace/Get_Opt.h \
  $(ACE_ROOT)/ace/SStringfwd.h \
  $(ACE_ROOT)/ace/Containers.h \
  $(ACE_ROOT)/ace/Containers.i \
  $(ACE_ROOT)/ace/Containers_T.h \
  $(ACE_ROOT)/ace/Node.h \
  $(ACE_ROOT)/ace/Node.cpp \
  $(ACE_ROOT)/ace/Array_Base.h \
  $(ACE_ROOT)/ace/Malloc_Base.h \
  $(ACE_ROOT)/ace/Array_Base.inl \
  $(ACE_ROOT)/ace/Array_Base.cpp \
  $(ACE_ROOT)/ace/Unbounded_Set.h \
  $(ACE_ROOT)/ace/Unbounded_Set.inl \
  $(ACE_ROOT)/ace/Unbounded_Set.cpp \
  $(ACE_ROOT)/ace/Log_Msg.h \
  $(ACE_ROOT)/ace/Log_Priority.h \
  $(ACE_ROOT)/ace/Unbounded_Queue.h \
  $(ACE_ROOT)/ace/Unbounded_Queue.inl \
  $(ACE_ROOT)/ace/Unbounded_Queue.cpp \
  $(ACE_ROOT)/ace/Containers_T.i \
  $(ACE_ROOT)/ace/Containers_T.cpp \
  $(ACE_ROOT)/ace/Get_Opt.i \
  $(ACE_ROOT)/ace/Synch_Traits.h \
  $(ACE_ROOT)/ace/Lock.h \
  $(ACE_ROOT)/ace/Lock.inl

.obj/proc_sema.o .obj/proc_sema.so .shobj/proc_sema.o .shobj/proc_sema.so: proc_sema.cpp \
  $(ACE_ROOT)/ace/OS_main.h \
  $(ACE_ROOT)/ace/ACE_export.h \
  $(ACE_ROOT)/ace/pre.h \
  $(ACE_ROOT)/ace/config.h \
  $(ACE_ROOT)/ace/$(ACE_PLATFORM_CONFIG) \
  $(ACE_ROOT)/ace/post.h \
  $(ACE_ROOT)/ace/ace_wchar.h \
  $(ACE_ROOT)/ace/ace_wchar.inl \
  $(ACE_ROOT)/ace/Process_Semaphore.h \
  $(ACE_ROOT)/ace/SV_Semaphore_Complex.h \
  $(ACE_ROOT)/ace/SV_Semaphore_Simple.h \
  $(ACE_ROOT)/ace/os_include/sys/os_stat.h \
  $(ACE_ROOT)/ace/config-all.h \
  $(ACE_ROOT)/ace/os_include/sys/os_types.h \
  $(ACE_ROOT)/ace/os_include/os_stddef.h \
  $(ACE_ROOT)/ace/os_include/sys/os_ipc.h \
  $(ACE_ROOT)/ace/os_include/sys/os_sem.h \
  $(ACE_ROOT)/ace/Default_Constants.h \
  $(ACE_ROOT)/ace/SV_Semaphore_Simple.i \
  $(ACE_ROOT)/ace/Global_Macros.h \
  $(ACE_ROOT)/ace/OS_Errno.h \
  $(ACE_ROOT)/ace/os_include/os_errno.h \
  $(ACE_ROOT)/ace/OS_Errno.inl \
  $(ACE_ROOT)/ace/OS_NS_Thread.h \
  $(ACE_ROOT)/ace/Basic_Types.h \
  $(ACE_ROOT)/ace/os_include/os_limits.h \
  $(ACE_ROOT)/ace/os_include/os_unistd.h \
  $(ACE_ROOT)/ace/os_include/os_inttypes.h \
  $(ACE_ROOT)/ace/os_include/os_stdint.h \
  $(ACE_ROOT)/ace/os_include/os_stdio.h \
  $(ACE_ROOT)/ace/os_include/os_stdarg.h \
  $(ACE_ROOT)/ace/os_include/os_float.h \
  $(ACE_ROOT)/ace/os_include/os_stdlib.h \
  $(ACE_ROOT)/ace/os_include/sys/os_wait.h \
  $(ACE_ROOT)/ace/os_include/os_signal.h \
  $(ACE_ROOT)/ace/os_include/os_time.h \
  $(ACE_ROOT)/ace/os_include/os_ucontext.h \
  $(ACE_ROOT)/ace/os_include/sys/os_resource.h \
  $(ACE_ROOT)/ace/os_include/sys/os_time.h \
  $(ACE_ROOT)/ace/Basic_Types.i \
  $(ACE_ROOT)/ace/os_include/os_pthread.h \
  $(ACE_ROOT)/ace/Base_Thread_Adapter.h \
  $(ACE_ROOT)/ace/OS_Log_Msg_Attributes.h \
  $(ACE_ROOT)/ace/iosfwd.h \
  $(ACE_ROOT)/ace/OS_Log_Msg_Attributes.inl \
  $(ACE_ROOT)/ace/Base_Thread_Adapter.inl \
  $(ACE_ROOT)/ace/os_include/os_semaphore.h \
  $(ACE_ROOT)/ace/OS_Memory.h \
  $(ACE_ROOT)/ace/OS_NS_stdlib.h \
  $(ACE_ROOT)/ace/OS_NS_stdlib.inl \
  $(ACE_ROOT)/ace/Object_Manager_Base.h \
  $(ACE_ROOT)/ace/Cleanup.h \
  $(ACE_ROOT)/ace/Cleanup.inl \
  $(ACE_ROOT)/ace/Object_Manager_Base.inl \
  $(ACE_ROOT)/ace/OS_NS_string.h \
  $(ACE_ROOT)/ace/OS_NS_string.inl \
  $(ACE_ROOT)/ace/OS_NS_wchar.h \
  $(ACE_ROOT)/ace/os_include/os_wchar.h \
  $(ACE_ROOT)/ace/os_include/os_string.h \
  $(ACE_ROOT)/ace/os_include/os_ctype.h \
  $(ACE_ROOT)/ace/OS_NS_wchar.inl \
  $(ACE_ROOT)/ace/os_include/os_search.h \
  $(ACE_ROOT)/ace/OS_Memory.inl \
  $(ACE_ROOT)/ace/OS_NS_signal.h \
  $(ACE_ROOT)/ace/OS_NS_signal.inl \
  $(ACE_ROOT)/ace/OS_NS_macros.h \
  $(ACE_ROOT)/ace/OS_NS_errno.h \
  $(ACE_ROOT)/ace/OS_NS_errno.inl \
  $(ACE_ROOT)/ace/OS_NS_Thread.inl \
  $(ACE_ROOT)/ace/Time_Value.h \
  $(ACE_ROOT)/ace/Time_Value.inl \
  $(ACE_ROOT)/ace/OS_NS_sys_time.h \
  $(ACE_ROOT)/ace/OS_NS_sys_time.inl \
  $(ACE_ROOT)/ace/OS_NS_sys_mman.h \
  $(ACE_ROOT)/ace/OS_NS_sys_mman.inl \
  $(ACE_ROOT)/ace/OS_NS_fcntl.h \
  $(ACE_ROOT)/ace/os_include/os_fcntl.h \
  $(ACE_ROOT)/ace/OS_NS_fcntl.inl \
  $(ACE_ROOT)/ace/OS_NS_unistd.h \
  $(ACE_ROOT)/ace/OS_NS_unistd.inl \
  $(ACE_ROOT)/ace/OS_NS_sys_utsname.h \
  $(ACE_ROOT)/ace/os_include/sys/os_utsname.h \
  $(ACE_ROOT)/ace/OS_NS_sys_utsname.inl \
  $(ACE_ROOT)/ace/OS_NS_stdio.h \
  $(ACE_ROOT)/ace/OS_NS_stdio.inl \
  $(ACE_ROOT)/ace/OS_NS_pwd.h \
  $(ACE_ROOT)/ace/os_include/os_pwd.h \
  $(ACE_ROOT)/ace/OS_NS_pwd.inl \
  $(ACE_ROOT)/ace/os_include/sys/os_mman.h \
  $(ACE_ROOT)/ace/SV_Semaphore_Complex.i \
  $(ACE_ROOT)/ace/Process_Semaphore.inl \
  $(ACE_ROOT)/ace/Get_Opt.h \
  $(ACE_ROOT)/ace/SStringfwd.h \
  $(ACE_ROOT)/ace/Containers.h \
  $(ACE_ROOT)/ace/Containers.i \
  $(ACE_ROOT)/ace/Containers_T.h \
  $(ACE_ROOT)/ace/Node.h \
  $(ACE_ROOT)/ace/Node.cpp \
  $(ACE_ROOT)/ace/Array_Base.h \
  $(ACE_ROOT)/ace/Malloc_Base.h \
  $(ACE_ROOT)/ace/Array_Base.inl \
  $(ACE_ROOT)/ace/Array_Base.cpp \
  $(ACE_ROOT)/ace/Unbounded_Set.h \
  $(ACE_ROOT)/ace/Unbounded_Set.inl \
  $(ACE_ROOT)/ace/Unbounded_Set.cpp \
  $(ACE_ROOT)/ace/Log_Msg.h \
  $(ACE_ROOT)/ace/Log_Priority.h \
  $(ACE_ROOT)/ace/Unbounded_Queue.h \
  $(ACE_ROOT)/ace/Unbounded_Queue.inl \
  $(ACE_ROOT)/ace/Unbounded_Queue.cpp \
  $(ACE_ROOT)/ace/Containers_T.i \
  $(ACE_ROOT)/ace/Containers_T.cpp \
  $(ACE_ROOT)/ace/Get_Opt.i \
  $(ACE_ROOT)/ace/Synch_Traits.h \
  $(ACE_ROOT)/ace/Lock.h \
  $(ACE_ROOT)/ace/Lock.inl

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
