#include "file_writer_manager.h"

#include <iostream>

namespace base{
namespace log{

FileWriterManager& FileWriterManager::me()
{
    static FileWriterManager me;
    return me;
}

bool FileWriterManager::addLogFile(const std::string& filename)
{
    auto it = m_logFiles.find(filename);
    if(it != m_logFiles.end())
        return false;
    m_logFiles[filename] = std::make_shared<FileWriter>(filename);
    return true;
}

bool FileWriterManager::delLogFile(const std::string& filename)
{
    auto it = m_logFiles.find(filename);
    if(it == m_logFiles.end())
        return true;
    //it->second->stop();//因为不支持使用过程中删除日志，所以不需要这一步
    m_logFiles.erase(it);
    return true;
}

void FileWriterManager::append(const std::string filename, const char* msg, const uint32_t len)
{
    if(!m_running)
        return;

    auto it = m_logFiles.find(filename);
    if(it == m_logFiles.end())
       return;

    it->second->append(msg, len); 
}

void FileWriterManager::start()
{
    //注意，没有任何线程安全，只能一个线程负责开启与增减日志文件，且增减日志文件需在start之前

    if(!m_running)
    {
        m_running = true;
        m_thread = std::thread(std::mem_fn(&FileWriterManager::run), this);
    }
    else
        std::cout << "ERROR:已经有日志线程在运行" << std::endl;
}

void FileWriterManager::run()
{
    std::cout << "日志线程开启" << std::endl;
    while(1)
    {
        //如果sop了检测缓冲是否空，空了之后才能停止
        if(!m_running)
        {
            bool stop = true;
            for(auto& item : m_logFiles)
            {
                if(item.second->bufferEmpty())
                    continue;
                stop = false;
                break;
            }
            if(stop)
                break;
        }

        //没有stop的情况一致循环

        bool allempty = true;
        //循环进行每一个writer的队列交换和写入文件等
        for(auto& item : m_logFiles)
        {
            auto writerPtr = item.second;
            //如果都是空，过
            if(writerPtr->bufferEmpty())
                continue;
            writerPtr->run();
            allempty = false;
        }
        
        //这里要有一个cpu释放，但是如果日志量较多的时候（写入队列将满）会唤醒
        std::unique_lock<std::mutex> lockWait(m_mutexWait);
        if(allempty)
            cond.wait_for(lockWait, std::chrono::milliseconds(50));
    }

    std::cout << "准备关闭日志线程" << std::endl;
}

void FileWriterManager::stop()
{
    m_running = false;
    cond.notify_one();
    m_thread.join();

    std::cout << "关闭日志线程" << std::endl;
}


}}
