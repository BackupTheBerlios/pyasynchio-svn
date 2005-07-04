/*!
 *	\file Proactor_impl_TimerHandler.hpp
 *	\brief Proactor::impl::TimerHandler interface.
 *	\author Vladimir Sukhoy
 */

#ifndef SPACE_TRANSPORT_PROACTOR_IMPL_TIMERHANDLER_HPP_INCLUDED_
#define SPACE_TRANSPORT_PROACTOR_IMPL_TIMERHANDLER_HPP_INCLUDED_

#pragma once

#include <space/transport/Proactor_impl.hpp>
#include <ace/Asynch_IO.h>

namespace space {

namespace transport {

class Proactor::impl::TimerHandler : public ACE_Handler
{
	public:
		TimerHandler(Proactor::impl *pro, TimerSignalPtr sig);
		virtual ~TimerHandler();

		void handle_time_out(const ACE_Time_Value &tv, const void *act /* = 0 */);
		void cancel();

	private:
		Proactor::impl *pro_;
		TimerSignalPtr sig_;
		bool canceled_;
};

} // namespace transport

} // namespace space

#endif // SPACE_TRANSPORT_PROACTOR_IMPL_TIMERHANDLER_HPP_INCLUDED_