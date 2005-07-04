// Sample_History.cpp,v 4.7 2004/04/29 05:08:14 ossama Exp

#include "ace/Sample_History.h"

#if !defined (__ACE_INLINE__)
#include "ace/Sample_History.inl"
#endif /* __ACE_INLINE__ */

#include "ace/Basic_Stats.h"
#include "ace/Log_Msg.h"
#include "ace/OS_Memory.h"

ACE_RCSID(ace, Sample_History, "Sample_History.cpp,v 4.7 2004/04/29 05:08:14 ossama Exp")

ACE_Sample_History::ACE_Sample_History (size_t max_samples)
  : max_samples_ (max_samples)
  , sample_count_ (0)
{
  ACE_NEW(this->samples_, ACE_UINT64[this->max_samples_]);
}

ACE_Sample_History::~ACE_Sample_History (void)
{
  delete[] this->samples_;
}

size_t
ACE_Sample_History::max_samples (void) const
{
  return this->max_samples_;
}

size_t
ACE_Sample_History::sample_count (void) const
{
  return this->sample_count_;
}

void
ACE_Sample_History::dump_samples (const ACE_TCHAR *msg,
                                  ACE_UINT32 scale_factor) const
{
#ifndef ACE_NLOGGING
  for (size_t i = 0; i != this->sample_count_; ++i)
    {
      const ACE_UINT64 x = this->samples_[i] / scale_factor;
      const ACE_UINT32 val = ACE_CU64_TO_CU32 (x);
      ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT("%s: %d %u\n"), msg, i, val));
    }
#endif /* ACE_NLOGGING */
}

void
ACE_Sample_History::collect_basic_stats (ACE_Basic_Stats &stats) const
{
  for (size_t i = 0; i != this->sample_count_; ++i)
    {
      stats.sample (this->samples_[i]);
    }
}
