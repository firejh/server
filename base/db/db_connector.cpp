#include "db_connector.h"

namespace base{
namespace db{

DBConnector::DBConnector()
{
    //初始化
    mysql_init(&m_mysql);
}

DBConnector::~DBConnector()
{
    close();
}

void DBConnector::connect(const string address, const string dbName, const string userName, const string passWd, const int port)
{
    //设置重连
    char value = 1;
    mysql_options(&m_mysql, MYSQL_OPT_RECONNECT, &value);

    //字符
    mysql_options(&m_mysql, MYSQL_SET_CHARSET_NAME, "utf8");

    //连接
    if(!mysql_real_connect(&m_mysql, address.c_str(), userName.c_str(), passWd.c_str(), dbName.c_str(), port, NULL, 0))
        EXCEPTION(DbConnectFailed, mysql_error(&m_mysql));

}

MYSQL* DBConnector::GetMysqlObject()
{
    return &m_mysql;
}

void DBConnector::close()
{
    mysql_close(&m_mysql);
    mysql_library_end();
}



}}
