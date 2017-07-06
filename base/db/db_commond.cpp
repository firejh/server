#include "db_commond.h"

namespace base{
namespace db{

DBCommond::DBCommond(MYSQL* mysql):
  m_ptMysql(mysql)
{}

DBCommond::~DBCommond()
{
}

bool DBCommond::commond(const string& sql)
{
    //执行
    if(mysql_real_query(m_ptMysql, sql.data(), sql.size()) != 0)
    {
        return false;
    }

    //结果获取到客户端
    MYSQL_ROW row;
    MYSQL_RES* res_set;

    //取出所有结果集，支持有多个
    do
    {
        //对一个结果的操作
        //将数据取到本地
        res_set = mysql_store_result(m_ptMysql);
        if(res_set == nullptr)
        {
            break;
        }
        
        uint32_t rowCount = 0;//行数
        vector<vector<string>> dataList;//结果的容器
        //填充结果的容器
        while((row = mysql_fetch_row(res_set)) != nullptr)
        {
            vector<string> list;//每行
            unsigned long *lengths = mysql_fetch_lengths(res_set);
            for(size_t i = 0; i < mysql_num_fields(res_set); i++)
                list.push_back(std::string(row[i], lengths[i]));//这里只有加上长度才能保证二进制数据没有错误
            dataList.emplace_back(list);
            ++rowCount;

        }

        //生成SelectResult
        SelectResult result(dataList);
        result.setColCount(mysql_num_fields(res_set));
        result.setRowCount(rowCount);

        //将生成SelectResult放入自己的结果容器
        m_resultList.emplace_back(result);
        //释放mysql的结果
        mysql_free_result(res_set);


    }while(!mysql_next_result(m_ptMysql));//==0成功有多个结果
    //至此，所有结果集已经放入m_resultList中

    return true;

}

vector<SelectResult>& DBCommond::getResults()
{
    return m_resultList;
}

}}
