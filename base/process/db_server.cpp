#include "db_server.h"

#include "log/logger.h"

namespace base{
namespace process{

DbServer::DbServer(const std::string host, const std::string user, const std::string passwd, const std::string name, const uint16_t port)
{}

DbServer::~DbServer()
{
    m_conn.close();

}

void DbServer::init()
{
    m_conn.connect(m_host, m_name, m_user, m_passwd, m_port);//失败会抛出异常
}

bool DbServer::exec()
{
    try
    {
        while(checkSwitch())
        {
            while(!m_callBackSqlIn.empty())
            {
                std::pair<std::string, BackData> temp;

                db::DBCommond commond(m_conn.GetMysqlObject());

                //获取sql
                if(!m_callBackSqlIn.pop(&temp))
                {
                    LOG_ERROR("DbConnection, 存储队列非空但是pop失败");
                    return false;
                }

                //执行sql
                if(!commond.commond(temp.first))
                {
                    LOG_ERROR("DbConnection, sql error: {}", mysql_error(m_conn.GetMysqlObject()));
                    continue;
                }

                if(temp.second.m_handle != nullptr)
                    m_callBackSqlOut.push({commond.getResults(), temp.second});//可优化？不好做

            }

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
    catch(const std::exception& ex)
    {
        LOG_ERROR("DbServer异常退出,{}", ex.what());
        return false;
    }
    return true;
}

bool DbServer::callBackTimerExec()
{
    while(!m_callBackSqlOut.empty())
    {
        std::pair<vector<db::SelectResult>, BackData> temp;
        if(!m_callBackSqlOut.pop(&temp))
        {
            LOG_ERROR("DbConnection, 回调队列非空但是pop失败");
            return false;
        }

        if(temp.second.m_handle == nullptr)
        {
            //wait
            continue;
        }

        temp.second.m_handle(temp.first, temp.second.m_data);
        //wait
    }
    return true;
}

void DbServer::exeStoreSql(std::string sql, DBEventHandler handerler, void* data /*= nullptr*/)
{
    BackData backData(handerler, data);
    if(!m_callBackSqlIn.push({sql, backData}))
        LOG_ERROR("DbConnection, 存储循环队列满");
}

}}
