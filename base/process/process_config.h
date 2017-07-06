/*
 * Author: JiangHeng
 *
 * Created: 2017-01-09 15:07 +0800
 *
 * Modified: 2017-01-09 15:10 +0800
 *
 * Description: 程配置，监听的端点，要连接的端点，可接入的processType 
 */

#ifndef WATER_PROCESS_NET_CONFIG_H
#define WATER_PROCESS_NET_CONFIG_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <vector>

#include "component/exception.h"
#include "net/endpoint.h"
#include "process_id.h"

namespace base{
namespace process{

//主配置出错报异常
DEFINE_EXCEPTION(LoadProcessConfigFailed, component::ExceptionBase)
DEFINE_EXCEPTION(ProcessCfgNotExisit, component::ExceptionBase)

struct DbConfig
{
    std::string m_connectId;
    std::string m_dbName;
    std::string m_host;
    std::string m_userName;
    std::string m_pwd;
    uint32_t m_port;
};

class ProcessConfig final
{
public:

    struct ProcessInfo
    {
        struct
        {
            std::shared_ptr<net::Endpoint> listen;

            std::set<net::Endpoint> connect;
        } privateNet;

        struct
        {
            std::set<net::Endpoint> listen;
        } publicNet;

        struct
        {
            std::set<net::Endpoint> listen;
        } httpNet;

    };

    ProcessConfig(const std::string& processName, int16_t processNum);
    ~ProcessConfig() = default;

    void load(const std::string& cfgDir);

    const ProcessInfo& getInfo();
    ProcessIdentity getProcessId() const;

    bool getDbWrite()
    {
        return !m_dbWrite.empty();
    }
    bool getDbRead()
    {
        return !m_dbRead.empty();
    }

    const std::vector<DbConfig> getdbReadCfg()
    {
        return m_dbRead;
    }

    const std::vector<DbConfig> getdbWriteCfg()
    {
        return m_dbWrite;
    }

private:
    bool parsePrivateEndpoint(std::set<net::Endpoint>* ret, const std::string& str);
    bool parseProcessList(std::set<ProcessIdentity>* ret, const std::string& str);
    void parseEndpointList(std::set<net::Endpoint>* ret, const std::string& str);

private:

    const std::string m_processName;
    const uint16_t m_processNum;
    ProcessIdentity m_processId;

    std::map<ProcessIdentity, net::Endpoint> m_processIdPrivateListenEps;
    ProcessInfo m_processInfo; //下标表示num

    std::vector<DbConfig> m_dbRead;
    std::vector<DbConfig> m_dbWrite;

};

}}

#endif
