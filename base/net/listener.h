/*
 * Author: JiangHeng
 *
 * Created: 2016-11-01 14:59 +0800
 *
 * Modified: 2016-11-01 14:59 +0800
 *
 * Description: 
 */
#ifndef WATER_NET_LISTENER_HPP
#define WATER_NET_LISTENER_HPP

#include "socket.h"
#include "connection.h"

namespace base{
namespace net{


class TcpListener : public TcpSocket
{
public:
    TYPEDEF_PTR(TcpListener);
public:
    CREATE_FUN_MAKE(TcpListener);
    explicit TcpListener() = default;
    //using TcpSocket::TcpSocket;

public:
    ~TcpListener() = default;

    void listen(int32_t backlog = 20);
    TcpConnection::Ptr accept();

private:
};


}}
#endif //#ifndef WATER_NET_LISTENER_HPP
