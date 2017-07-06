/*
 * Author: JiangHeng - dantezhu@vip.qq.com
 *
 * Last modified: 2014-12-06 00:31 +0800
 *
 * Description: 
 */

#ifndef WATER_TCP_CLIENT_H
#define WATER_TCP_CLIENT_H

#include <atomic>
#include <map>

#include "net/endpoint.h"
#include "net/packet_connection.h"
#include "component/datetime.h"
#include "component/event.h"
#include "component/class_helper.h"
#include "process_thread.h"

namespace base{
namespace process{

class TcpClient : public ProcessThread
{
public:
    TYPEDEF_PTR(TcpClient)
    CREATE_FUN_MAKE(TcpClient)

    TcpClient();
    void addRemoteEndpoint(net::Endpoint ep, std::chrono::seconds retryInterval);
    bool exec() override;

public:
    component::Event<void (net::PacketConnection::Ptr)> e_newConn;
    component::Event<void (TcpClient*)> e_close;
//    component::Event<void (TcpClient*)> e_allConnsReady;

private:
    uint32_t readyNum() const;

private:
    struct RemoteEndpointInfo
    {
        component::TimePoint retryTimepoint;
        std::chrono::seconds retryInterval;
        net::TcpConnection::WPtr conn;
    };

    std::map<net::Endpoint, RemoteEndpointInfo> m_remoteEndpoints;
};

}}

#endif
