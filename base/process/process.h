/*
 * Author: JiangHeng
 *
 * Created: 2017-01-04 19:18 +0800
 *
 * Modified: 2017-01-04 19:18 +0800
 *
 * Description: 进程框架, 所有如服务器近程的基类
 */

#ifndef WATER_PROCESS_PROCESS_H
#define WATER_PROCESS_PROCESS_H

#include "component/datetime.h"

#include "tcp_server.h"
#include "tcp_client.h"
#include "process_id.h"
#include "process_config.h"
#include "tcp_connection_manager.h"
#include "http_connection_manager.h"
#include "private_connection_checker.h"
#include "process_timer.h"
#include "db_server.h"

#include <atomic>

namespace base{
namespace process{

//class TcpMsg;

class Process
{
public:
    Process(const std::string& name, int16_t num, const std::string& configDir, const std::string& logDir);
    virtual ~Process();

    void start();
    void stop();
    const std::string& getName() const;
    std::string getFullName() const;
    ProcessIdentity getId() const;

    const std::string& cfgDir() const;
    void regTimer(std::chrono::milliseconds interval, const ProcessTimer::EventHandler& handler);

    bool readDB(const std::string& dbName, const std::string& sql, DBEventHandler handler, void* data = nullptr);

protected:
    virtual void init();
    virtual void lanchThreads();
    virtual void joinThreads();

private:
    void dealTcpPackets(const component::TimePoint& now);
    virtual void tcpPacketHandle(TcpPacket::Ptr packet, 
                                  TcpConnectionManager::ConnectionHolder::Ptr conn, 
                                  const component::TimePoint& now) = 0;

protected:
    const std::string m_processName;
    const std::string m_cfgDir;
    ProcessConfig m_cfg;
    const std::string m_logDir;

    //所有标准子线程
    std::map<std::string, ProcessThread*> m_threads;

    //私网
    TcpServer::Ptr m_privateNetServer;
    TcpClient::Ptr m_privateNetClient;

    //公网
    TcpServer::Ptr m_publicNetServer;

    //连接检查器
    ConnectionChecker::Ptr m_privateConnChecker;

    //连接管理器，提供消息接收和发送的epoll事件
    TcpConnectionManager m_conns;

    //http服务器相关
    TcpServer::Ptr m_httpServer;
    HttpConnectionManager m_httpcons;

    //数据库相关
    std::map<std::string, DbServer::Ptr> m_dbRead;
    std::map<std::string, DbServer::Ptr> m_dbWrite;

    //主定时器，处理一切业务处理
    ProcessTimer m_timer;
};

}}

#endif

