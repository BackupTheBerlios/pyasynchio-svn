/*!
 *  \file ResultConversion.hpp
 *  \brief Defines interface for conversion for ACE_Asynch_Result-derived classes 
 *  to space::transport results.
 *  \author Vladimir Sukhoy
 */

#ifndef SPACE_TRANSPORT_RESULTCONVERSION_HPP_INCLUDED_
#define SPACE_TRANSPORT_RESULTCONVERSION_HPP_INCLUDED_

#pragma once

#ifdef GCCXML
#error "This header doesnt support GCCXML!"
#endif // GCCXML

#include <space/transport/Result.hpp>
#include <space/transport/AcceptResult.hpp>
#include <space/transport/ConnectResult.hpp>
#include <space/transport/StreamResults.hpp>
#include <ace/Asynch_IO.h>

namespace space {

namespace transport {

extern Result& convert(Result &lhs
    , const ACE_Asynch_Result &rhs);
extern AcceptResult& convert(AcceptResult &lhs
    , const ACE_Asynch_Accept::Result &rhs);
extern ConnectResult& convert(ConnectResult &lhs
    , const ACE_Asynch_Connect::Result &rhs);
extern ReadStreamResult& convert(ReadStreamResult &lhs
    , const ACE_Asynch_Read_Stream::Result &rhs);
extern WriteStreamResult& convert(WriteStreamResult &lhs
    , const ACE_Asynch_Write_Stream::Result &rhs);

} // namespace transport

} // namespace space

#endif // SPACE_TRANSPORT_RESULTCONVERSION_HPP_INCLUDED_