#include "logger.h"

#include <assert.h>
#include <iostream>

std::string Logger::formatTime()
{
    time_t now;
    now = time(&now);
    struct tm vtm;
    localtime_r(&now, &vtm);

    static thread_local time_t lastSecond = 0;
    static thread_local char result[18];

    if (lastSecond != now)
    {
        lastSecond = now;
        snprintf(result, sizeof(result), "%4d%02d%02d %02d:%02d:%02d", vtm.tm_year + 1990, vtm.tm_mon + 1, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec);
        //int len = snprintf(result, sizeof(result), "%4d%02d%02d %02d:%02d:%02d", vtm.tm_year + 1990, vtm.tm_mon + 1, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec);
        //assert(len == 17);
    }
    return result;
}

const char* Logger::getLevelStr(LogLevel l)
{
    switch (l)
    {
    case LogLevel::LL_DEBUG:
        return "DEBUG";
    case LogLevel::LL_TRACE:
        return "TRACE";
    case LogLevel::LL_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
    return "UNKNOWN";

}

void Logger::addAllLog(const std::string& filename)
{
    std::cout << "加载指定日志文件:" << std::endl;
    //base

    m_basefilename = filename + m_baseFilename  + ".log"
    Logger::addLog(m_basefilename);
    std::cout << "baselog:" <<  filename + m_baseFilename  + ".log" << std::endl;

    //diamond
    m_diamondFilename = filename + m_diamondFilename  + ".log";
    Logger::addLog(m_diamondFilename);
    std::cout << "diamondlog:" << m_diamondFilename  + ".log" << std::endl;

    //开启日志
    Logger::startLog();

}

std::string Logger::m_basefilename = "BASE";
std::string Logger::m_diamondFilename = "DIAMOND";


