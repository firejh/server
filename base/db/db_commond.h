#ifndef BASE_DB_COMMOND_H
#define BASE_DB_COMMOND_H

#include "db_connector.h"
#include "db_select_result.h"

#include <vector>

namespace base{
namespace db{

class DBCommond
{
public:
    DBCommond(MYSQL* mysql);
    ~DBCommond();
public:
    bool commond(const string& sql);
    vector<SelectResult>& getResults();
    void clear();

private:
    MYSQL* m_ptMysql;
    MYSQL_RES* m_ptResult;
    vector<SelectResult> m_resultList;
};

}}

#endif
