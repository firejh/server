#include "process_config.h"

#include "component/xmlparse.h"
#include "component/format.h"
#include "component/string_kit.h"

namespace base{
namespace process{


ProcessConfig::ProcessConfig(const std::string& processName, int16_t processNum)
:m_processName(processName), m_processNum(processNum)
{
}

void ProcessConfig::load(const std::string& cfgDir)
{
    using component::XmlParseDoc;
    using component::XmlParseNode;

    const std::string configFile = cfgDir + "/process.xml";


    XmlParseDoc doc(configFile);
    XmlParseNode root = doc.getRoot();
    if(!root)
        EXCEPTION(LoadProcessConfigFailed, configFile + " parse root node failed");

    std::map<std::string, DbConfig> m_dbcfg;
    {//通用配置
        XmlParseNode allDBNode = root.getChild("allDB");
        if(!allDBNode)
            EXCEPTION(LoadProcessConfigFailed, "allDB node not exisit");

        for(XmlParseNode processTypeNode = allDBNode.getChild("mysql"); processTypeNode; ++processTypeNode)
        {
            DbConfig dbcfg;
            dbcfg.m_connectId = processTypeNode.getAttr<std::string>("connectId");
            dbcfg.m_dbName = processTypeNode.getAttr<std::string>("dbName");
            dbcfg.m_host = processTypeNode.getAttr<std::string>("host");
            dbcfg.m_userName = processTypeNode.getAttr<std::string>("userName");
            dbcfg.m_pwd = processTypeNode.getAttr<std::string>("pwd");
            dbcfg.m_port = processTypeNode.getAttr<uint32_t>("port");
            m_dbcfg[dbcfg.m_dbName] = dbcfg;
        }
    }

    {//进程拓扑结构
        XmlParseNode allProcessesNode = root.getChild("allProcesses");
        if(!allProcessesNode)
            EXCEPTION(LoadProcessConfigFailed, "allProcesses node not exisit");

        {//进程类型声明, 生成并建立processType和processName的映射, 记录在ProcessIdendity中

            const std::string processTypeListStr = allProcessesNode.getAttr<std::string>("typeList");
            std::vector<std::string> allProcessTypeNames = component::splitString(processTypeListStr, " ");

            for(auto& name : allProcessTypeNames)
            {
                //ProcessIdendity::type2Name
                ProcessIdentity::s_type2Name.push_back(name);

                //ProcessIdendity::name2Type
                ProcessType type = ProcessIdentity::s_type2Name.size() - 1;
                ProcessIdentity::s_name2Type[name] = type;
            }
            ProcessIdentity::s_type2Name.shrink_to_fit();
        }

        
        {   //对processType节点做一次遍历，
            //做好processId 到privateListen的Endpoint映射
            //并找到当前进程类型对应的配置结点
            XmlParseNode theProcessNode = XmlParseNode::getInvalidNode();
            for(XmlParseNode processTypeNode = allProcessesNode.getChild("processType"); processTypeNode; ++processTypeNode)
            {
                auto name = processTypeNode.getAttr<std::string>("name");
                ProcessType type = ProcessIdentity::stringToType(name);
                if(type == INVALID_PROCESS_TYPE)
                    EXCEPTION(ProcessCfgNotExisit, "{} is not exisit in allProcesses.typeList", name);

                //processType.process.private.listen
                for(XmlParseNode processNode = processTypeNode.getChild("process"); processNode; ++processNode)
                {
                    auto num = processNode.getAttr<int32_t>("num");
                    ProcessIdentity processId(name, num);
                    XmlParseNode privateNode = processNode.getChild("private");
                    if(!privateNode)
                        EXCEPTION(ProcessCfgNotExisit, "process cfg {} do not has {} node", processId, "private");

                    auto endPointStr = privateNode.getAttr<std::string>("listen");
                    std::shared_ptr<net::Endpoint> privateListen;
                    if(!endPointStr.empty())
                    {
                        privateListen.reset(new net::Endpoint(endPointStr));
                        m_processIdPrivateListenEps[processId] = *privateListen;
                    }

                    //process self node
                    if(name == m_processName && num == m_processNum)
                    {
                        m_processId.type() = type;
                        m_processId.num() = m_processNum;

                        theProcessNode = processNode;
                        m_processInfo.privateNet.listen = privateListen;
                    }
                }
            }

            if(!theProcessNode)
            {
                //出错，这此时的m_processId不可靠，日志中的processFullName要自己拼
                auto processFullName = component::format("[{}:{}]", m_processName, m_processNum);
                EXCEPTION(ProcessCfgNotExisit, "进程{}在配置文件中不存在", processFullName);
            }
            std::cout << "成功加载自己进程配置" << std::endl;

            ProcessInfo& info = m_processInfo;

            //开始开始解析私网除listen外的部分，因为上面已经解析过了
            XmlParseNode privateNode = theProcessNode.getChild("private");
            if(!privateNode) //私网配置节点必须存在
                EXCEPTION(ProcessCfgNotExisit, "进程 {} 下缺少 private 配置", m_processId);
            //私网接出
            if(!parsePrivateEndpoint(&info.privateNet.connect, privateNode.getAttr<std::string>("connectProcess")))
                EXCEPTION(LoadProcessConfigFailed, "进程 {} 属性 connectProcess 解析失败", m_processId);

            //解析公网
            XmlParseNode publicNode = theProcessNode.getChild("public");
            if(publicNode)
                parseEndpointList(&info.publicNet.listen, publicNode.getAttr<std::string>("listen"));

            //解析http
            XmlParseNode httpNode = theProcessNode.getChild("http");
            if(httpNode)
                parseEndpointList(&info.httpNet.listen, httpNode.getAttr<std::string>("listen"));

            //解析数据库
            XmlParseNode dbwriteNode = theProcessNode.getChild("dbwrite");
            XmlParseNode dbreadNode = theProcessNode.getChild("dbread");
            if(dbwriteNode)
            {
                auto names = dbwriteNode.getAttr<std::string>("name");
                std::vector<std::string> allName;
                allName = component::splitString(names, ",");
                for(auto& item : allName)
                {
                    m_dbWrite.push_back(m_dbcfg[item]);
                }
            }
            if(dbreadNode)
            {
                auto names = dbreadNode.getAttr<std::string>("name");
                std::vector<std::string> allName;
                allName = component::splitString(names, ",");
                for(auto& item : allName)
                {
                    m_dbRead.push_back(m_dbcfg[item]);
                }
            }
        }
    }
}

bool ProcessConfig::parsePrivateEndpoint(std::set<net::Endpoint>* ret, const std::string& str)
{
    ret->clear();

    std::set<ProcessIdentity> processIds;
    if(!parseProcessList(&processIds, str))
        return false;

    for(const auto& processId : processIds)
    {
        auto it = m_processIdPrivateListenEps.find(processId);
        if(it == m_processIdPrivateListenEps.end())
            return false;

        ret->insert(it->second);
    }

    return true;
}

bool ProcessConfig::parseProcessList(std::set<ProcessIdentity>* ret, const std::string& str)
{
    ret->clear();
    std::vector<std::string> items;
    component::splitString(&items, str, " ");

    for(const std::string& item : items)
    {
        ProcessIdentity id;
        std::vector<std::string> processStr = component::splitString(item, ":");
        if(processStr.size() ==  2)
        {
            id.type() = ProcessIdentity::stringToType(processStr[0]);
            if(id.type() == INVALID_PROCESS_TYPE)
                return false;

            auto processNums = component::fromString<std::vector<int16_t>>(processStr[1], ",");

            for(int32_t num : processNums)
            {
                id.num() = num;
                ret->insert(id);
            }
        }
        else if(processStr.size() == 3)
        {
            id.type() = ProcessIdentity::stringToType(processStr[1]);
            if(id.type() == INVALID_PROCESS_TYPE)
                continue;
            auto processNums = component::fromString<std::vector<int16_t>>(processStr[2], ",");

            for(int32_t num : processNums)
            {
                id.num() = num;
                ret->insert(id);
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

void ProcessConfig::parseEndpointList(std::set<net::Endpoint>* ret, const std::string& str)
{
    ret->clear();
    std::vector<std::string> endpointStrs = component::splitString(str, " ");
    for(const auto& endpointStr : endpointStrs)
    {
        net::Endpoint ep(endpointStr);
        ret->insert(ep);
    }
}

const ProcessConfig::ProcessInfo& ProcessConfig::getInfo()
{
    return m_processInfo;
}

ProcessIdentity ProcessConfig::getProcessId() const
{
    return m_processId;
}


}}

