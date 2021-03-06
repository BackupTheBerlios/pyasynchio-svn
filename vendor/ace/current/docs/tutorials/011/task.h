// task.h,v 1.7 2003/11/09 20:44:18 dhinton Exp

#ifndef TASK_H
#define TASK_H

#include "ace/Task.h"
#include "ace/Barrier.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

class Task : public ACE_Task <ACE_MT_SYNCH>
{
public:

  typedef ACE_Task <ACE_MT_SYNCH> inherited;

  Task (size_t n_threads);
  ~Task (void);

  int open (void * = 0);

  int svc (void);

  int close (u_long flags = 0);

protected:
  ACE_Barrier barrier_;

  size_t n_threads_;
};

#endif /* TASK_H */
