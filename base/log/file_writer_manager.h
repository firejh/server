/*
 * Author: JiangHeng
 *
 * Created: 2017-01-07 11:09 +0800
 *
 * Modified: 2017-01-07 11:09 +0800
 *
 * Description:日志文件管理，添加日志文件不支持多线程，目前没有需求，如果需要，m_logFiles的操作需要加互斥锁 
 */
#ifndef BASE_LOG_FILE_WRITER_MANAGER_H
#define BASE_LOG_FILE_WRITER_MANAGER_H

#include "file_writer.h"

#include <map>
#include <string>
#include <thread>
#include <condition_variable>

namespace base{
namespace log{

class FileWriterManager
{
public:
    ~FileWriterManager(){}
    static FileWriterManager& me();

private:
    FileWriterManager(){}

public:
    //添加日志文件与删除日志文件，这些操作必须在start（）之前进行
    bool addLogFile(const std::string& filename);
    bool delLogFile(const std::string& filename);

    void append(const std::string filename, const char* msg, const uint32_t len);

    //开启日志线程
    void start();
    void stop();
    void run();

private:
    std::map<std::string, std::shared_ptr<FileWriter>> m_logFiles;//所有的日志文件

    volatile bool m_running = false;//线程状态
    std::thread m_thread;//线程
    std::mutex m_mutexWait;//用于cond，并且对缓冲时间设置
    std::condition_variable cond;
};

}}
#endif
