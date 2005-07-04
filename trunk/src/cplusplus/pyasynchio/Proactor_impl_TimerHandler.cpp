/*!
 *	\file Proactor_impl_TimerHandler.cpp
 *	\brief Implementation for space::transport::Proactor::impl::TimerHandler.
 *	\author Vladimir Sukhoy
 */

#include <space/pch.hpp>
#include <space/transport/Proactor_impl_TimerHandler.hpp>

namespace space {

namespace transport {

Proactor::impl::TimerHandler::TimerHandler(Proactor::impl *pro, TimerSignalPtr sig)
: pro_(pro)
, sig_(sig)
, canceled_(false)
{}

Proactor::impl::TimerHandler::~TimerHandler()
{}

void Proactor::impl::TimerHandler::cancel()
{
	canceled_ = true;
}

void Proactor::impl::TimerHandler::handle_time_out(const ACE_Time_Value &tv
												  , const void *act /* = 0 */)
{
	if (!canceled_) (*sig_)(tv);
	{
		ACE_GUARD(ACE_Thread_Mutex,g, pro_->thsMutex_);
		std::pair<TimerHandlers::iterator, TimerHandlers::iterator> eqr;
		eqr = pro_->ths_.equal_range(sig_);
		TimerHandlers::iterator thi;
		for (thi = eqr.first; thi != eqr.second; ++thi) {
			if (thi->second.get() == this) {
				pro_->ths_.erase(thi);
				return;
			}
		}
	}
}

} // namespace transport 

} // namespace space
