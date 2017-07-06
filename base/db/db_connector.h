/*
 * Author: JiangHeng
 *
 * Created: 2017-01-06 15:36 +0800
 *
 * Modified: 2017-01-06 15:36 +0800
 *
 * Description: 数据库连接
 */
#ifndef BASE_DB_CONNECTOR_H
#define BASE_DB_CONNECTOR_H

#include "/usr/local/mysql/include/mysql.h"

#include "component/exception.h"
#include <string>

namespace base{
namespace db{

using namespace std;

DEFINE_EXCEPTION(DbConnectFailed, component::ExceptionBase)
class DBConnector
{
public:
    DBConnector();
    ~DBConnector();
public:
    //数据库连接
    void connect(const string address, const string dbName, const string userName, const string passWd, const int port);

    //返回MYSQL*
    MYSQL* GetMysqlObject();

    //关闭mysql
    void close();
private:
    MYSQL m_mysql;

};


}}
#endif
