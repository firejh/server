/*
 * Author: JiangHeng
 *
 * Created: 2016-11-01 15:05 +0800
 *
 * Modified: 2016-11-01 15:05 +0800
 *
 * Description: 
 */
#ifndef WATER_NET_SOCKET_HPP
#define WATER_NET_SOCKET_HPP

#include "../component/class_helper.h"
#include "../component/event.h"

#include "endpoint.h"

namespace base{
namespace net{

class TcpSocket
{
public:
    static const int32_t INVALID_SOCKET_FD = -1;
public:
    TYPEDEF_PTR(TcpSocket)
protected:
    explicit TcpSocket();
    explicit TcpSocket(int32_t fd);

public:
    virtual ~TcpSocket();
    //moveable
    TcpSocket(TcpSocket&& other);
    TcpSocket& operator=(TcpSocket&& other);
    //non-copyable
    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator = (const TcpSocket&) = delete;

    void bind(uint16_t port);
    void bind(const std::string& strIp, uint16_t port);
    void bind(const Endpoint& endPoint);

    void setNonBlocking();
    void setBlocking();
    bool isNonBlocking() const;

    int32_t getFD() const;

    bool isAvaliable() const;

    void close();

public:
    component::Event<void (TcpSocket*)> e_close;
private:
    int32_t m_fd;

public:
    static bool isNonBlockingFD(int32_t fd);
};

}}
#endif //#ifndef WATER_NET_SOCKET_H
