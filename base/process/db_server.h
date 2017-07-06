/*
 * Author: JiangHeng
 *
 * Created: 2017-01-09 19:42 +0800
 *
 * Modified: 2017-01-09 19:42 +0800
 *
 * Description: 
 */
#ifndef BASE_DB_SERVER_H
#define BASE_DB_SERVER_H

//这里可以改成相对目录，mysql的目录可以 -I包含
#include "/usr/local/mysql/include/mysql.h"

#include "db/db.h"
#include "component/spinlock.h"
#include "component/lock_free_circular_queue_ss.h"
#include "process_thread.h"

#include <vector>

namespace base{
namespace process{

using namespace db;

//数据库异常
DEFINE_EXCEPTION(DbConnectFailed, component::ExceptionBase)

//句柄
using  DBEventHandler = std::function<void(std::vector<db::SelectResult>, void* data)>;
//回调数据
struct BackData
{
    BackData(){}
    BackData(DBEventHandler handle, void* data): m_data(data), m_handle(handle){}
    void* m_data;
    DBEventHandler m_handle;
};

class DbServer : public ProcessThread
{
public:
    TYPEDEF_PTR(DbServer)
    CREATE_FUN_MAKE(DbServer)

    DbServer(const std::string host, const std::string user, const std::string passwd, const std::string name, const uint16_t port);
    ~DbServer();

public:
    void init();

    bool exec() override;
    bool callBackTimerExec();
    void exeStoreSql(std::string sql, DBEventHandler handerler, void* data = nullptr);

private:
    std::string m_host;
    std::string m_user;
    std::string m_passwd;
    std::string m_name;
    uint16_t m_port;

    db::DBConnector m_conn;//数据库连接

    component::LockFreeCircularQueueSPSC<std::pair<std::string, BackData>> m_callBackSqlIn;
    component::LockFreeCircularQueueSPSC<std::pair<vector<db::SelectResult>, BackData>> m_callBackSqlOut;

};

}}


#endif
