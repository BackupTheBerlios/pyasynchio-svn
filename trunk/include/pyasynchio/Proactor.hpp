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
#include <pyasynchio/StreamContext.hpp>
#include <pyasynchio/Signals.hpp>
#include <pyasynchio/AcceptContext.hpp>
#include <pyasynchio/ConnectContext.hpp>

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
     *  @param active If true then behave as active object (owns
     *  its thread). If false then user should launch event
     *  loop on its own.
     */
    Proactor(bool active = true);

    /*! Destroy proactor object. No virtuality intended.
     */
    ~Proactor();

    /*! Initiate proactive accept operation.
     *  @param ctx signal context of accept operation
     *  @param addr Bind address.
     *  @param bytesToRead How many bytes to read during accept (Windows only).
     */
    void accept(AcceptContextPtr ctx
        , const ACE_INET_Addr &addr
        , size_t bytesToRead = 0);
        
    /*! Close pending proactive accept operations related to particular context.
     *  @param ctx Accept context to cancel and close any accepts on.
     */
    void close(AcceptContextPtr ctx);
    
    /*! Initiate proactive connect operation.
     *  @param ctx signal context of connect operation.
     *  @param remote Remote address to connect to.
     *  @param local Local address.
     */
    void connect(ConnectContextPtr ctx
        , const ACE_INET_Addr &remote
        , const ACE_INET_Addr &local = ANY_INTERFACE);
        
    /*! Close proactive connect operations related to particular context.
     *  @param ctx Connect context to cancel and close any accepts on.
     */
    void close(ConnectContextPtr ctx);

    /*! Initiate proactive stream read operation on stream context.
     *  @param ctx Stream context to use.
     *  @param bytes How many bytes to read.
     *  @param act Asynchronous Completion Token.
     *  @param priority Operation priority (no-op on Windows).
     *  @param signal POSIX4 real-time signal to be used for operation.
     */  
    void read(StreamContextPtr ctx
        , size_t count
        , const void * act = 0
        , int priority = 0
        , int signal = 0/*ACE_SIGRTMIN*/);
        
    /* Cancel any pending read operation on the context.
     *  @param ctx Stream context to close read operations on.
     */
    void cancelRead(StreamContextPtr ctx);

    /*! Initiate proactive stream write operation on stream context.
     *  @param ctx Stream context to use.
     *  @param data Data to write.
     *  @param act Asynchronous Completion Token.
     *  @param priority Operation priority (no-op on Windows).
     *  @param signal POSIX4 real-time signal to be used for operation.
     */
    void write(StreamContextPtr ctx
        , const buf &data
        , const void *act = 0
        , int priority = 0
        , int signal = 0/*ACE_SIGRTMIN*/);

    /*! Cancel any pending write operations on the context.
     *  @param ctx Stream context to close write operations on.
     */
    void cancelWrite(StreamContextPtr ctx);
    
    /*! Close stream context and terminate any operations.
     *  @param ctx Stream context to terminate operations on.
     */
    void close(StreamContextPtr ctx);

    /*! Schedule timer signal to specified delay.
     *  @param delay Relative delay for timer to be signalled.
     *  @param elapsed Timer signal to use.
     */
    void scheduleTimer(const ACE_Time_Value &delay, TimerSignalPtr elapsed);

    /*! Cancel scheduled timer before it is being signalled.
     *  @param elapsed Timer signal to cancel.
     */
    void cancelTimer(TimerSignalPtr elapsed);

    /*! Handle events callback for single-threaded (non-active) usage.
     */
    void handleEvents(const ACE_Time_Value &delay);
    void handleEvents();

    /*! Shutdown active object (join thread).
     */
    void shutdown();

    class impl;
private:
    boost::shared_ptr<impl> pimpl_;
};



} // namespace pyasynchio

#endif // PYASYNCHIO_PROACTOR_HPP_INCLUDED_