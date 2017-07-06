/*
 * Author: JiangHeng
 *
 * Created: 2017-01-06 20:42 +0800
 *
 * Modified: 2017-01-06 20:42 +0800
 *
 * Description: 对文件的操作
 */
#ifndef BASE_LOG_WRITER_FILE_H
#define BASE_LOG_WRITER_FILE_H

#include "component/exception.h"

#include <string>
#include <fcntl.h>


namespace base{
namespace log{

DEFINE_EXCEPTION(OpenLogFileFailed, component::ExceptionBase)
DEFINE_EXCEPTION(WriteLogFileFailed, component::ExceptionBase)

class FileStream
{
public:
    FileStream(const std::string name);
    ~FileStream();

    void append(const char* msg, const size_t line);

private:
    bool load();

    ssize_t writeto(const char* msg, const size_t line);

    int32_t lock();

    int32_t unlock();

    std::string getFileNameBynow();

    void roll();

    tm timeNow();

private:
    std::string m_filename;
    std::string m_curFilename;
    int32_t m_fd;
    struct flock m_lock { F_WRLCK, SEEK_SET, 0, 0, 0 };
    int32_t m_logHour;
};


}}
#endif
