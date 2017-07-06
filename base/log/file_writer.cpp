#include "file_writer.h"

#include <iostream>

namespace base{
namespace log{

FileWriter::FileWriter(const std::string filename)
: m_filename(filename), m_fileStream(filename), m_readBuf(new WriterBuffer), m_writeBuf(new WriterBuffer)
{
}

FileWriter::~FileWriter()
{}

void FileWriter::append(const char* msg, const uint32_t len)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if(msg == nullptr)
        return;
    if(m_writeBuf->remain() > len)
    {
        m_writeBuf->put(msg, len);
    }
    else
    {
        std::cout << "LOG ERROR:overflow writeBufs lose msg = " << msg << std::endl;
    }

}

void FileWriter::run()
{
    //先判断读是否为空，为空且写不为空那需要交换
    if(m_readBuf->empty())
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(!m_writeBuf->empty())
        {
            BufferPtr temp;
            temp = std::move(m_readBuf);
            m_readBuf = std::move(m_writeBuf);
            m_writeBuf = std::move(temp);
        }
        lock.unlock();
    } 
    //写入文件
    if(!m_readBuf->empty())
    {
        m_fileStream.append(m_readBuf->data(), m_readBuf->length());
        m_readBuf->reset();
    }
}

bool FileWriter::bufferEmpty() const
{
    return (m_readBuf->empty() && m_writeBuf->empty());
}

bool FileWriter::mustSwap() const
{
    if(m_writeBuf->remain() <= REMAIN_NOTIFY_BUFFER_SIZE)
    {
        return true;
    }
    return false;
}

}}
