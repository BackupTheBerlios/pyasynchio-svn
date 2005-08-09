#ifndef PYASYNCHIO_ASYNC_POLL_HPP_INCLUDED_
#define PYASYNCHIO_ASYNC_POLL_HPP_INCLUDED_

#include <pyasynchio/buf.hpp>
#include <list>
#include <map>
#include <boost/shared_ptr.hpp>

namespace pyasynchio
{
struct empty_struct {};
typedef empty_struct* SOCKET;

class async_poll
{
public:
    struct base_result
    {
        base_result(bool success, unsigned long error = 0)
            : success_(success)
            , error_(error)
        {}
        virtual ~base_result() {}

        bool success_;
        unsigned long error_;
    };
    typedef boost::shared_ptr<base_result> base_result_ptr;

    struct accept_result : base_result
    {
        accept_result(bool success, SOCKET accepted = 0
            , unsigned long error = 0)
            : base_result(success, error)
            , accepted_(accepted)
        {}
        virtual ~accept_result() {}

        SOCKET accepted;
    };
    
    struct connect_result : base_result
    {
        connect_result(bool success, unsigned long error = 0)
            : base_result(success, error)
        {}
        virtual ~connect_result();
    };

    struct send_result : base_result
    {
        send_stream_result(bool success, const buf &data
                            , unsigned long error = 0)
            : base_result(success, error)
            , data_(data)
        {}
        virtual ~send_stream_result() {}
        buf data_;
    };

    struct recv_result : base_result
    {
        recv_stream_result(bool success, const buf &data
                            , unsigned long error = 0)
            : base_result(success, error)
            , data_(data)
        {}
        virtual ~recv_stream_result() {}
        buf data_;
    };

    

    async_poll();
    ~async_poll();

    void accept(SOCKET listen_socket, SOCKET accept_socket);
    void connect(SOCKET s, const sockaddr *name, int namelen);
    void send(SOCKET s, const buf &data, flags = 0);
    void sendto(SOCKET s, const buf &data, const sockaddr *name, int namelen);
    void recv(SOCKET s, size_t bufsize, flags = 0);

    void cancel(SOCKET s);
    void close(SOCKET s);

    std::list<base_result_ptr> poll(uint32 ms = 0);

private:
                
};

} // namespace pyasynchio

#endif // PYASYNCHIO_DISPATCHER_HPP_INCLUDED_