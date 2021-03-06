/* -*- c++ -*- */
// Reactive_IO.h,v 1.1 2003/08/25 07:46:52 kobica Exp

#ifndef JAWS_REACTIVE_IO_H
#define JAWS_REACTIVE_IO_H

#include "ace/OS.h"
#include "ace/Message_Block.h"
#include "ace/Singleton.h"

#include "jaws3/Export.h"
#include "jaws3/IO.h"
#include "jaws3/Event_Result.h"

class JAWS_Reactive_IO;

class JAWS_Export JAWS_Reactive_IO : public JAWS_IO_Impl
{
public:

  static JAWS_Reactive_IO * instance (void)
  {
    return ACE_Singleton<JAWS_Reactive_IO, ACE_SYNCH_MUTEX>::instance ();
  }

  void send ( ACE_HANDLE handle
            , ACE_Message_Block *mb
            , JAWS_Event_Completer *completer
            , void *act = 0
            );

  void recv ( ACE_HANDLE handle
            , ACE_Message_Block *mb
            , JAWS_Event_Completer *completer
            , void *act = 0
            );

  void transmit ( ACE_HANDLE handle
                , ACE_HANDLE source
                , JAWS_Event_Completer *completer
                , void *act = 0
                , ACE_Message_Block *header = 0
                , ACE_Message_Block *trailer = 0
                );

  void send ( ACE_HANDLE handle
            , ACE_Message_Block *mb
            , JAWS_Event_Completer *completer
            , const ACE_Time_Value &tv
            , void *act = 0
            );

  void recv ( ACE_HANDLE handle
            , ACE_Message_Block *mb
            , JAWS_Event_Completer *completer
            , const ACE_Time_Value &tv
            , void *act = 0
            );

  void transmit ( ACE_HANDLE handle
                , ACE_HANDLE source
                , JAWS_Event_Completer *completer
                , const ACE_Time_Value &tv
                , void *act = 0
                , ACE_Message_Block *header = 0
                , ACE_Message_Block *trailer = 0
                );

};

#endif /* JAWS_REACTIVE_IO_H */
