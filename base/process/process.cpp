#include "process.h"

#include "log/logger.h"
#include "component/scope_guard.h"
#include "component/string_kit.h"
#include "component/signal_handler.h"

#include <iostream>
#include <thread>
#include <map>


#include <unistd.h>
namespace base{
namespace process{

Process::Process(const std::string& name, int16_t num, const std::string& configDir, const std::string& logDir)
: m_processName(name), m_cfgDir(configDir), m_cfg(name, num), m_logDir(logDir)
{
}

Process::~Process()
{
    m_threads.clear();
}

void Process::lanchThreads()
{
    for(auto& item : m_dbRead)
    {
        item.second->run();
        const std::string name = item.first;
        m_threads.insert({name, item.second.get()});
        LOG_DEBUG("{} thread start ok", name);
        std::cout << "线程" << name << " 开启" << std::endl;
    }
    for(auto& item : m_dbWrite)
    {
        item.second->run();
        const std::string name = item.first;
        m_threads.insert({name, item.second.get()});
        LOG_DEBUG("{} thread start ok", name);
        std::cout << "线程" << name << " 开启" << std::endl;
    }
    if(m_privateNetServer)
    {
        m_privateNetServer->run();
        const std::string name = "private server";
        m_threads.insert({name, m_privateNetServer.get()});
        LOG_DEBUG("{} thread start ok", name);
        std::cout << "线程 m_privateNetServer 开启" << std::endl;
    }
    if(m_privateNetClient)
    {
        m_privateNetClient->run();
        const std::string name = "private client";
        m_threads.insert({name, m_privateNetClient.get()});
        LOG_DEBUG("{} thread start ok", name);
        std::cout << "线程 m_privateNetClient 开启" << std::endl;
    }
    if(m_publicNetServer)
    {
        m_publicNetServer->run();
        const std::string name =  "public server";
        m_threads.insert({name, m_publicNetServer.get()});
        LOG_DEBUG("{} thread start ok", name);
        std::cout << "线程 m_publicNetServer 开启" << std::endl;
    }
    if(m_httpServer)
    {
        const std::string serverName = "http server";
        m_httpServer->run();
        m_threads.insert({serverName, m_httpServer.get()});
        LOG_DEBUG("{} thread start ok", serverName);

        const std::string connName = "http conns";
        m_httpcons.run();
        m_threads.insert({connName, &m_timer});
        LOG_DEBUG("{} thread start ok", connName);
        std::cout << "线程 m_httpServer,m_httpcons 开启" << std::endl;
    }
    if(m_privateConnChecker)
    {
        m_privateConnChecker->run();
        const std::string name = "connection checker";
        m_threads.insert({name, m_privateConnChecker.get()});
        LOG_DEBUG("{} thread start ok", name);
        std::cout << "线程 m_privateConnChecker 开启" << std::endl;
    }
    {
        m_conns.run();
        const std::string name = "tcp conns";
        m_threads.insert({name, &m_conns});
        LOG_DEBUG("{} thread start ok", name);
        std::cout << "线程 m_conns 开启" << std::endl;
    }
    {
        m_timer.suspend(); //设为挂起状态
        m_timer.run();
        const std::string name = "main timer";
        m_threads.insert({name, &m_timer});
        LOG_DEBUG("{} thread start ok", name);
        std::cout << "线程 m_timer 开启" << std::endl;
    }
}

void Process::joinThreads()
{
    while(!m_threads.empty())
    {
        if(m_timer.isSuspend())
        {
            const ProcessConfig::ProcessInfo& cfg = m_cfg.getInfo();
            //发起私网连接全部成功，主定时器开始执行
            if(m_conns.totalPrivateConnNum() == cfg.privateNet.connect.size())
            {
                LOG_DEBUG("{} lanuch successful", getFullName());

                //touch启动好的进程
                std::string shell_cmd = "touch " + m_logDir + "/touchfile/" + getFullName();
                system(shell_cmd.c_str());

                //主定时器恢复
                m_timer.resume();
            }
        }

        for(auto& threadInfo : m_threads)
        {
            const std::string& name = threadInfo.first;
            ProcessThread* thread = threadInfo.second;

            bool threadRetValue;
            const auto waitRet = thread->wait(&threadRetValue, std::chrono::milliseconds(0));
            if(waitRet == ProcessThread::WaitRet::timeout)
                continue;

            if(threadRetValue)
            {
                LOG_TRACE("thread {}  stoped", name, getFullName());
            }
            else
            {
                LOG_ERROR("thread {}  abort", name, getFullName());
                stop();
            }
            m_threads.erase(name);
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    Logger::stopLog();
}

void Process::start()
{
    try
    {
        //init()为虚函数， 不能在constructor中调用
        init();

        ON_EXIT_SCOPE_DO(SignalHandler::resetSignalHandle({SIGINT, SIGTERM}));

        lanchThreads();

        joinThreads();
    }
    catch (const component::ExceptionBase& ex)
    {
        std::string log = base::component::format("process {} start, fatal error: [{}]", getFullName(), ex.what());
        LOG_ERROR(log);
        std::cout << log << std::endl;
        stop();
    }
    catch (const std::exception& ex)
    {
        std::string log = base::component::format("process {} start, fatal error: [{}]", getFullName(), ex.what());
        LOG_ERROR(log);
        std::cout << log << std::endl;
        stop();
    }
    catch (...)
    {
        std::string log = base::component::format("process {} start, unkonw error", getFullName());
        LOG_ERROR(log);
        std::cout << log << std::endl;
        stop();
    }
    LOG_TRACE("{} exited", getFullName());
}

void Process::stop()
{
    LOG_TRACE("exiting ...");
    for(auto& item : m_threads)
	{
        item.second->stop();
	}
    LOG_TRACE("stoped");
}

const std::string& Process::getName() const
{
    return m_processName;
}

std::string Process::getFullName() const
{
    return component::toString(getId());
}

ProcessIdentity Process::getId() const
{
    return m_cfg.getProcessId();
}

const std::string& Process::cfgDir() const
{
    return m_cfgDir;
}

void Process::regTimer(std::chrono::milliseconds interval, const ProcessTimer::EventHandler& handler)
{
    m_timer.regEventHandler(interval, handler);
    
}

void Process::init()
{
    std::cout << "进程初始化..."  << std::endl;
    {//处理linux信号
        SignalHandler::setSignalHandle({SIGINT, SIGTERM}, std::bind(&Process::stop, this));
    }

    //配置解析
    std::cout << "进程配置加载..." << std::endl;
    m_cfg.load(m_cfgDir);
    std::cout << "######进程配置加载完毕######" << std::endl << std::endl;

    //指定日志文件
    Logger::addAllLog(m_logDir + "/" + getFullName());


    {//初始化各个组件

        //据库线程
        auto dbRead = m_cfg.getdbReadCfg();
        for(auto iter = dbRead.begin(); iter != dbRead.end(); ++iter)
        {
            auto& dbcfg = *iter;
            auto dbPtr = DbServer::create(dbcfg.m_host, dbcfg.m_userName, dbcfg.m_pwd, dbcfg.m_dbName, dbcfg.m_port);
            dbPtr->init();
            m_dbRead[dbcfg.m_connectId] = dbPtr;
        }

        auto dbWrite = m_cfg.getdbReadCfg();
        for(auto iter = dbWrite.begin(); iter != dbWrite.end(); ++iter)
        {
            auto& dbcfg = *iter;
            auto dbPtr = DbServer::create(dbcfg.m_host, dbcfg.m_userName, dbcfg.m_pwd, dbcfg.m_dbName, dbcfg.m_port);
            dbPtr->init();
            m_dbWrite[dbcfg.m_connectId] = dbPtr;
        }

        const ProcessConfig::ProcessInfo& cfg = m_cfg.getInfo();
        const auto& privateNet = cfg.privateNet;

        //私网连接检查
        m_privateConnChecker = PrivateConnectionChecker::create(getId());
        //私网监听
        if(privateNet.listen != nullptr)
        {
            m_privateNetServer = TcpServer::create();
            m_privateNetServer->addLocalEndpoint(*privateNet.listen);
            std::cout << "初始化 m_privateNetServer" << std::endl;
        }

        //私网连出
        if(!privateNet.connect.empty())
        {
            m_privateNetClient = TcpClient::create();
            for(const net::Endpoint& ep : privateNet.connect)
                m_privateNetClient->addRemoteEndpoint(ep, std::chrono::seconds(5));
            std::cout << "初始化 m_privateNetClient" << std::endl;
        }

        //公网监听
        if(!cfg.publicNet.listen.empty())
        {
            m_publicNetServer = TcpServer::create();
            for(const net::Endpoint& ep : cfg.publicNet.listen)
                m_publicNetServer->addLocalEndpoint(ep);
            std::cout << "初始化 m_publicNetServer" << std::endl;
        }


        //http监听
        const auto& httpNet = cfg.httpNet;
        if (!httpNet.listen.empty())
        {
            m_httpServer = TcpServer::create();
            for(const net::Endpoint& ep : cfg.httpNet.listen)
                m_httpServer->addLocalEndpoint(ep);
            std::cout << "初始化 m_httpServer" << std::endl;
        }
        std::cout << "######初始化线程结束######" << std::endl << std::endl;
    }

    {//绑定各种核心事件的处理函数
        using namespace std::placeholders;
        //私网的新连接, 放入连接检查器
        if(m_privateNetServer)
        {
            auto checker = std::bind(&ConnectionChecker::addUncheckedConnection
                                     , m_privateConnChecker
                                     , _1
                                     , ConnectionChecker::ConnType::in);
            m_privateNetServer->e_newConn.reg(checker);
        }
        if(m_privateNetClient)
        {
            auto checker = std::bind(&ConnectionChecker::addUncheckedConnection
                                     , m_privateConnChecker
                                     , _1
                                     , ConnectionChecker::ConnType::out);
            m_privateNetClient->e_newConn.reg(checker);
        }

        if(m_publicNetServer)
        {
            //当有外部连接接入时的处理，这里级直接把新conn加入了connManager，
            //应该要改，给这里加一个checker，像privaterConnectionChecker一样，
            //checker会和clinet做通信，确认client的合法性，并确定client的id
            //然后checker再把验证过的conn加入connManager
        }


        if (m_httpServer)
        {
            m_httpServer->e_newConn.reg(std::bind(&HttpConnectionManager::addPrivateConnection, 
                                                  &m_httpcons, _1));
        }
        //通过检查的连接加入连接管理器
        m_privateConnChecker->e_connConfirmed.reg(std::bind(&TcpConnectionManager::addPrivateConnection, 
                                                            &m_conns, _1, _2));

        //处理消息接收队列
        regTimer(std::chrono::milliseconds(20), std::bind(&Process::dealTcpPackets, this, _1));

        //数据库定时注册
        for(auto& item : m_dbRead)
        {
            regTimer(std::chrono::milliseconds(20), std::bind(&DbServer::callBackTimerExec, item.second));
        }
        for(auto& item : m_dbWrite)
        {
            regTimer(std::chrono::milliseconds(20), std::bind(&DbServer::callBackTimerExec, item.second));
        }
    }
}

void Process::dealTcpPackets(const component::TimePoint& now)
{
    TcpConnectionManager::ConnectionHolder::Ptr conn;
    net::Packet::Ptr packet;
    while(m_conns.getPacket(&conn, &packet))
    {
        TcpPacket::Ptr tcpPacket = std::static_pointer_cast<TcpPacket>(packet);

        tcpPacketHandle(tcpPacket, conn, now);
    }
}


bool Process::readDB(const std::string& dbName, const std::string& sql, DBEventHandler handler, void* data)
{
    auto it = m_dbRead.find(dbName);
    if(it == m_dbRead.end())
    {
        return false;
    }

    it->second->exeStoreSql(sql, handler, data);
    return true;
}


}}

