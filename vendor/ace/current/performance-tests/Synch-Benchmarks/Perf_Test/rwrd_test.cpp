// rwrd_test.cpp,v 1.3 2003/11/09 04:12:09 dhinton Exp

#define  ACE_BUILD_SVC_DLL
#include "Performance_Test_Options.h"
#include "Benchmark_Performance.h"

ACE_RCSID(Synch_Benchmarks, rwrd_test, "rwrd_test.cpp,v 1.3 2003/11/09 04:12:09 dhinton Exp")

#if defined (ACE_HAS_THREADS)

class ACE_Svc_Export RWRD_Test : public Benchmark_Performance
{
public:
  virtual int svc (void);

private:
  static ACE_RW_Mutex rw_lock;
};

ACE_RW_Mutex RWRD_Test::rw_lock;

int
RWRD_Test::svc (void)
{
  int ni = this->thr_id ();
  synch_count = 2;

  while (!this->done ())
    {
      rw_lock.acquire_read ();
      performance_test_options.thr_work_count[ni]++;
      buffer++;
      rw_lock.release ();
    }

  /* NOTREACHED */
  return 0;
}

ACE_SVC_FACTORY_DECLARE (RWRD_Test)
ACE_SVC_FACTORY_DEFINE  (RWRD_Test)

// ACE_Service_Object_Type rwrdt (&rwrd_test, "RWRD_Mutex_Test");
#endif /* ACE_HAS_THREADS */