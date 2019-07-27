// yx
#ifndef YXALPHA_ACCEPTOR_H_
#define YXALPHA_ACCEPTOR_H_

#include "socket.h"
#include "reactor/eventhandler.h"

namespace yxalp {

class Acceptor {
public:
    // typedef std::function<void (int socket, const InetAddr &)> NewConnectionCallback;
    typedef std::function<void (Socket &&sock, const InetAddr &)> NewConnectionCallback;

    Acceptor(Dispatcher *dispatcher, const InetAddr &addr);
    ~Acceptor();
    Acceptor & operator=(const Acceptor &) = delete;
    Acceptor(const Acceptor &) = delete;
    // For TcpServer 
    void set_new_connection_callback(const NewConnectionCallback& cb)
    { newConnectionCallback_ = cb; }

    bool is_listenning() const { return listenning_; }
    void Listen();

private:
    void EventHandlerCallBack();  // bind 至 EventHandler 的 CallBack function

    Dispatcher *dispatcher_;
    Socket server_socket_;
    EventHandler event_handler_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
    int idle_fd_;
};

}  // namespace yxalp

#endif  // YXALPHA_ACCEPTOR_H_