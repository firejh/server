/*
 * Author: JiangHeng
 *
 * Created: 2017-01-07 09:45 +0800
 *
 * Modified: 2017-01-07 09:45 +0800
 *
 * Description:针对单个文件的缓冲队列的控制 
 */
#ifndef BASE_LOG_FILE_WRITER_H
#define BASE_LOG_FILE_WRITER_H
#include "writer_buffer.h"
#include "file_stream.h"

#include <memory>
#include <mutex>

namespace base{
namespace log{

class FileWriter
{
public:
    typedef std::unique_ptr<WriterBuffer> BufferPtr;

    FileWriter(const std::string filename);
    ~FileWriter();

    //向缓冲写队列写入的接口
    void append(const char* msg, const uint32_t len);

    //队列缓冲操作
    void run();

    //判断两个队列是否都已经处理完毕
    bool bufferEmpty() const;

    //根据写队列是否要满，判断是否需要交换读写队列
    bool mustSwap() const;


private:
    std::string m_filename;//日志名称
    FileStream m_fileStream;//对一个文件的基础操作

    BufferPtr m_readBuf;//读出队列，读出后直接写入文件
    BufferPtr m_writeBuf;//写入缓冲队列

    std::mutex m_mutex;//写队列与读队列交换锁
};

}}
#endif
