#include "file_stream.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

namespace base{
namespace log{

FileStream::FileStream(const std::string filename)
: m_filename(filename),
  m_curFilename(""),
  m_fd(1),//标准输出
  m_logHour(0)
{
    roll();
}

FileStream::~FileStream()
{
    ::close(m_fd);
}

bool FileStream::load()
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    m_fd = ::open(m_curFilename.c_str(), O_WRONLY | O_APPEND | O_CREAT, mode);
    if (-1 == m_fd)
        SYS_EXCEPTION(OpenLogFileFailed, "can't open log file {}", m_curFilename);

    ::unlink(m_filename.c_str());
    ::symlink(m_curFilename.c_str(), m_filename.c_str());
    return true;
}

ssize_t FileStream::writeto(const char* msg, const size_t len)
{
    ssize_t done = ::write(m_fd, msg, len);
    if (done == -1)
        SYS_EXCEPTION(WriteLogFileFailed, "::writing file {}", m_curFilename);
    return done;
}

int32_t FileStream::lock()
{
    m_lock.l_type = F_WRLCK;
    return ::fcntl(m_fd, F_SETLK, &m_lock);
}

int32_t FileStream::unlock()
{
    m_lock.l_type = F_UNLCK;
    return ::fcntl(m_fd, F_SETLK, &m_lock);
}

void FileStream::append(const char* msg, const size_t len)
{
//注释后不支持单文件并发写入
//    lock();
//    ON_EXIT_SCOPE_DO( unlock() );

    if (m_logHour != timeNow().tm_hour)
        roll();

    ssize_t done = 0;
    while(done < static_cast<ssize_t>(len))
        done += writeto(msg + done, len - done);
}

std::string FileStream::getFileNameBynow()
{
    struct tm vtm = timeNow();
    char t_time[32];
    snprintf(t_time, sizeof(t_time), ".%4d%02d%02d-%02d", vtm.tm_year+1990, vtm.tm_mon+1, vtm.tm_mday, vtm.tm_hour);
    return m_filename + t_time;
}

void FileStream::roll()
{
    m_curFilename = getFileNameBynow();
    load();

    m_logHour = timeNow().tm_hour;
}

tm FileStream::timeNow()
{
    time_t now;
    now = time(&now);
    struct tm vtm;
    localtime_r(&now, &vtm);
    return vtm;
}

}}
