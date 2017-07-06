#include "db_select_result.h"

namespace base{
namespace db{

SelectResult::SelectResult(vector<vector<string>>& tempList)
{
    m_dataList.swap(tempList);
}

SelectResult::SelectResult(const SelectResult& other)
{
    m_numRow = other.m_numRow;
    m_numCol = other.m_numCol; 
    m_dataList = other.m_dataList;
}

SelectResult::SelectResult(SelectResult&& other)
{
    m_numRow = other.m_numRow;
    m_numCol = other.m_numCol; 
    m_dataList.swap(other.m_dataList);
}

SelectResult& SelectResult::operator=(const SelectResult& other)
{
    m_numRow = other.m_numRow;
    m_numCol = other.m_numCol;
    m_dataList = other.m_dataList;
    return *this;
}

SelectResult::~SelectResult()
{}

int SelectResult::getRowCount()
{
    return m_numCol;
}

int SelectResult::getColCount()
{
    return m_numRow;
}

std::string SelectResult::getString(int row, int col)
{
    return m_dataList[row][col];
}

void SelectResult::setRowCount(int rowCount)
{
    m_numRow = rowCount;
}

void SelectResult::setColCount(int colCount)
{
    m_numCol = colCount;
}

}}
