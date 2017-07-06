#ifndef BASE_DB_SELECT_RESULT_H
#define BASE_DB_SELECT_RESULT_H

#include <vector>
#include <string>
#include "stdint.h"
#include <iostream>

namespace base{
namespace db{

using namespace std;

class SelectResult
{
    //friend class DBCommond;
public:
    SelectResult(vector<vector<string>>& tempList);
    SelectResult(const SelectResult& other);
    SelectResult(SelectResult&& other);
    SelectResult& operator=(const SelectResult& other);
    ~SelectResult();
public:
    //返回查询结果的行数
    int getRowCount();

    //返回查询结果的列数
    int getColCount();

    //取某一位置的数据
    std::string getString(int row, int col);

    void setRowCount(int rowCount);
    void setColCount(int colCount);

private:
     vector<vector<string>> m_dataList;//结果集

     int m_numRow;//行数
     int m_numCol;//列数

};


}}

template<typename T>
T getDBData(base::db::SelectResult* result, int row, int col)
{
    T ret = T();
    std::stringstream ss(result->getString(row, col));

    ss >> ret;
    return ret;
}

#endif
