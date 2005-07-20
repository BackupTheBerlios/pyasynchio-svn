/*!
 *  \file Proactor.hpp
 *
 *  \brief Exploratory interface for future transport subsystem proactor
 *  wrapper class.
 *
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_PROACTOR_HPP_INCLUDED_
#define PYASYNCHIO_PROACTOR_HPP_INCLUDED_

#pragma once

#include <pyasynchio/detail/config.hpp>
#include <pyasynchio/AbstractAcceptHandler.hpp>
#include <pyasynchio/AbstractConnectHandler.hpp>
#include <pyasynchio/AbstractStreamHandler.hpp>
#include <pyasynchio/AbstractTimerHandler.hpp>
#include <pyasynchio/Signals.hpp>

class ACE_INET_Addr;
class ACE_Time_Value;

namespace pyasynchio {

/*!
 *  @brief Our proactor facade, which is based on ACE_Proactor framework. 
 *
 *  The main idea is to utilize closures, generic functions, maybe signals&slots
 *  stuff to make framework trully flexible and convenient :). And resolve
 *  those problems with pending handlers, operations cancel and handlers
 *  lifetime.
 */
class PYASYNCHIO_LINK_DECL Proactor
{
public:
    static const ACE_INET_Addr ANY_INTERFACE;

    /*! Create proactor object.
     */
    Proactor();

    /*! Destroy proactor object. No virtuality intended.
     */
    ~Proactor();

    /*! Initiate proactive accept operation.
     *  @param ctx signal handler of accept operation
     *  @param addr Bind address.
     *  @param bytesToRead How many bytes to read during accept (Windows only).
     */
    void open_stream_accept(AbstractAcceptHandlerPtr user_accept_handler
        , const ACE_INET_Addr &addr
        , size_t bytesToRead = 0);
        
    /*! Close pending proactive accept operations related to particular context.
     *  @param ctx Accept context to cancel and close any accepts on.
     */
    void close_stream_accept(AbstractAcceptHandlerPtr user_accept_handler);
    
    /*! Initiate proactive connect operation.
     *  @param ctx signal context of connect operation.
     *  @param remote Remote address to connect to.
     *  @param local Local address.
     */
    void open_stream_connect(AbstractConnectHandlerPtr user_connect_handler
        , const ACE_INET_Addr &remote
        , const ACE_INET_Addr &local = ANY_INTERFACE);
        
    /*! Close proactive connect operations related to particular context.
     *  @param ctx Connect context to cancel and close any accepts on.
     */
    void close_stream_connect(AbstractConnectHandlerPtr user_connect_handler);

    /*! Initiate proactive stream read operation on stream context.
     *  @param ctx Stream context to use.
     *  @param bytes How many bytes to read.
     *  @param act Asynchronous Completion Token.
     *  @param priority Operation priority (no-op on Windows).
     *  @param signal POSIX4 real-time signal to be used for operation.
     */  
    void open_stream_read(AbstractStreamHandlerPtr user_stream_handler
        , size_t count
        , const void * act = 0
        , int priority = 0
        , int signal = 0/*ACE_SIGRTMIN*/);
        
    /* Cancel any pending read operation on the context.
     *  @param ctx Stream context to close read operations on.
     */
    void cancel_stream_read(AbstractStreamHandlerPtr user_stream_handler);

    /*! Initiate proactive stream write operation on stream context.
     *  @param ctx Stream context to use.
     *  @param data Data to write.
     *  @param act Asynchronous Completion Token.
     *  @param priority Operation priority (no-op on Windows).
     *  @param signal POSIX4 real-time signal to be used for operation.
     */
    void open_stream_write(AbstractStreamHandlerPtr user_stream_handler
        , const buf &data
        , const void *act = 0
        , int priority = 0
        , int signal = 0/*ACE_SIGRTMIN*/);

    /*! Cancel any pending write operations on the context.
     *  @param ctx Stream context to close write operations on.
     */
    void cancel_stream_write(AbstractStreamHandlerPtr user_stream_handler);
    
    /*! Close stream handler associated activity and terminate any operations.
     *  @param ctx Stream context to terminate operations on.
     */
    void close_active_stream(AbstractStreamHandlerPtr user_stream_handler);

    /*! Schedule timer signal to specified delay.
     *  @param delay Relative delay for timer to be signalled.
     *  @param elapsed Timer signal to use.
     */
    void schedule_timer(const ACE_Time_Value &delay, AbstractTimerHandlerPtr elapsed_handler);

    /*! Cancel scheduled timer before it is being signalled.
     *  @param elapsed Timer signal to cancel.
     */
    void cancel_timer(AbstractTimerHandlerPtr elapsed_handler);

    /*! Handle events callback for single-threaded (non-active) usage.
     */
    void handle_events(const ACE_Time_Value &delay);
    void handle_events();

    class impl;
private:
    boost::shared_ptr<impl> pimpl_;
};



} // namespace pyasynchio

#endif // PYASYNCHIO_PROACTOR_HPP_INCLUDED_