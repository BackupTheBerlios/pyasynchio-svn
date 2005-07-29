/*!
 *  \file ResultConversion.hpp
 *  \brief Defines interface for conversion for ACE_Asynch_Result-derived classes 
 *  to space::transport results.
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_RESULTCONVERSION_HPP_INCLUDED_
#define PYASYNCHIO_RESULTCONVERSION_HPP_INCLUDED_

#pragma once

#ifdef GCCXML
#error "This header doesnt support GCCXML!"
#endif // GCCXML

#include <pyasynchio/Result.hpp>
#include <pyasynchio/AcceptResult.hpp>
#include <pyasynchio/ConnectResult.hpp>
#include <pyasynchio/StreamResults.hpp>
#include <ace/Asynch_IO.h>

namespace pyasynchio {

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

} // namespace pyasynchio

#endif // PYASYNCHIO_RESULTCONVERSION_HPP_INCLUDED_