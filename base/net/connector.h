/*
 * Author: JiangHeng
 *
 * Created: 2016-11-01 14:54 +0800
 *
 * Modified: 2016-11-01 14:54 +0800
 *
 * Description: 
 */
#ifndef WATER_NET_CONNECTOR_HPP
#define WATER_NET_CONNECTOR_HPP

#include "../component/class_helper.h"
#include "endpoint.h"
#include "connection.h"

#include <chrono>

namespace base{
namespace net{
class TcpConnector
{

public:
    TYPEDEF_PTR(TcpConnector)
public:
    CREATE_FUN_MAKE(TcpConnector)
    TcpConnector(const std::string& strIp, uint16_t port);
    TcpConnector(const Endpoint& endPoint);
    ~TcpConnector();

public:
    TcpConnection::Ptr connect();

    //带超时的连接，超市时返回nullptr
    TcpConnection::Ptr connect(const std::chrono::milliseconds& timeout);

private:
    Endpoint m_remoteEndpoint;
};

}}

#endif
